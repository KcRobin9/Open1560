/*
    Open1560 - An Open Source Re-Implementation of Midtown Madness 1 Beta
    Copyright (C) 2020 Brick

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "sdldinput.h"

#include <SDL3/SDL_gamepad.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>

#include <vector>

static HRESULT DoFunctionNotImplemented(const char* name, usize times)
{
    if (times < 5)
        Errorf("%s is not implemented (called %zu times)", name, times);

    return E_NOTIMPL;
}

#define FunctionNotImplemented()                 \
    DoFunctionNotImplemented(ARTS_FUNCTION, [] { \
        static usize counter = 0;                \
        return ++counter;                        \
    }());

class ioRumbleEffect;

class ioGamepad
{
public:
    ioGamepad(std::vector<SDL_Gamepad*> gamepads)
        : Gamepads(std::move(gamepads))
    {
        for (SDL_Gamepad* gamepad : Gamepads)
        {
            SDL_PropertiesID props = SDL_GetGamepadProperties(gamepad);

            if (SDL_GetBooleanProperty(props, SDL_PROP_GAMEPAD_CAP_RUMBLE_BOOLEAN, false))
                CanRumble = true;
        }
    }

    ~ioGamepad()
    {
        for (SDL_Gamepad* gamepad : Gamepads)
            SDL_CloseGamepad(gamepad);
    }

    void RegisterEffect(ioRumbleEffect* effect)
    {
        Effects.emplace_back(effect);
    }

    void UnregisterEffect(ioRumbleEffect* effect)
    {
        Effects.erase(std::remove(Effects.begin(), Effects.end(), effect), Effects.end());
    }

    SDL_Gamepad* Active() const
    {
        return Gamepads.front();
    }

    void Update();

    std::vector<SDL_Gamepad*> Gamepads {};
    std::vector<ioRumbleEffect*> Effects {};
    bool CanRumble {};
};

static inline f32 lerp(f32 a, f32 b, f32 t)
{
    return (1 - t) * a + t * b;
}

class ioRumbleEffect
{
public:
    static u64 Now()
    {
        return SDL_GetTicksNS() / 1000;
    }

    ioRumbleEffect(ioGamepad* owner)
        : Owner(owner)
    {
        Owner->RegisterEffect(this);
    }

    virtual ~ioRumbleEffect() = 0
    {
        Owner->UnregisterEffect(this);
    }

    virtual void SetParameters(const DIEFFECT& params, DWORD dwFlags)
    {
        if (dwFlags & DIEP_DURATION)
            Params.dwDuration = params.dwDuration;
        if (dwFlags & DIEP_SAMPLEPERIOD)
            Params.dwSamplePeriod = params.dwSamplePeriod;
        if (dwFlags & DIEP_GAIN)
            Params.dwGain = params.dwGain;
        if (dwFlags & DIEP_TRIGGERBUTTON)
            Params.dwTriggerButton = params.dwTriggerButton;
        if (dwFlags & DIEP_TRIGGERREPEATINTERVAL)
            Params.dwTriggerRepeatInterval = params.dwTriggerRepeatInterval;

        if (dwFlags & DIEP_AXES)
        {
            // TODO: Copy the axes
            // Params.cAxes = params.cAxes;
            // Params.rgdwAxes = params.rgdwAxes;
        }

        if (dwFlags & DIEP_DIRECTION)
        {
            // TODO: Copy the directions
            // Params.cAxes = params.cAxes;
            // Params.rglDirection = params.rglDirection;
            Params.dwFlags ^= (Params.dwFlags ^ params.dwFlags) & (DIEFF_CARTESIAN | DIEFF_POLAR | DIEFF_SPHERICAL);
        }

        if (dwFlags & DIEP_ENVELOPE)
        {
            if (params.lpEnvelope)
            {
                Envelope = *params.lpEnvelope;
                Params.lpEnvelope = &Envelope;
            }
            else
            {
                Params.lpEnvelope = NULL;
            }
        }

        // TODO: Handle DIEP_NORESTART | DIEP_NODOWNLOAD

        if (dwFlags & DIEP_START)
        {
            Start(Now(), 1);
        }
    }

    virtual f32 GetSustainLevel() const
    {
        return 1.0f;
    }

    virtual f32 Calculate(u32 now, u32 duration) = 0;

    void Start(u64 now, u32 iters)
    {
        StartTime = now;
        Iters = iters;
    }

    void Stop()
    {
        StartTime = 0;
        Iters = 0;
    }

    f32 Update(u64 now)
    {
        while (Iters > 0)
        {
            u32 duration = Params.dwDuration;
            u64 end = StartTime + duration;

            if (now < end)
            {
                u32 here = static_cast<u32>(now - StartTime);
                f32 level = Calculate(here, duration);
                f32 gain = static_cast<f32>(Params.dwGain) / 10000.0f;
                f32 sustain = GetSustainLevel();

                if (Params.lpEnvelope)
                {
                    if (here < Envelope.dwAttackTime)
                    {
                        f32 attack = static_cast<f32>(Envelope.dwAttackLevel) / 10000.0f;
                        sustain =
                            lerp(attack, sustain, static_cast<f32>(here) / static_cast<f32>(Envelope.dwAttackTime));
                    }
                    else if (u32 fade_start = duration - Envelope.dwFadeTime; here >= fade_start)
                    {
                        f32 fade = static_cast<f32>(Envelope.dwFadeLevel) / 10000.0f;
                        sustain = lerp(
                            sustain, fade, static_cast<f32>(here - fade_start) / static_cast<f32>(Envelope.dwFadeTime));
                    }
                }

                return level * gain * sustain;
            }

            StartTime = end;
            --Iters;
        }

        return 0.0f;
    }

protected:
    ioGamepad* Owner;
    DIEFFECT Params {};
    DIENVELOPE Envelope {};

    u64 StartTime {};
    u32 Iters {};
};

void ioGamepad::Update()
{
    SDL_UpdateGamepads();

    for (usize i = 1; i < Gamepads.size(); ++i)
    {
        SDL_Gamepad* gamepad = Gamepads[i];

        bool active = false;

        for (int j = 0; j < SDL_GAMEPAD_BUTTON_COUNT; ++j)
        {
            if (SDL_GetGamepadButton(gamepad, static_cast<SDL_GamepadButton>(j)))
            {
                active = true;
                break;
            }
        }

        if (active)
        {
            SDL_RumbleGamepad(gamepad, 0, 0, 0);
            std::swap(Gamepads[0], Gamepads[i]);
            break;
        }
    }

    if (CanRumble)
    {
        u64 now = ioRumbleEffect::Now();
        f32 scale = 0.0f;

        for (ioRumbleEffect* effect : Effects)
            scale += effect->Update(now);

        u16 rumble = static_cast<u16>(std::clamp(scale * 65535.0f, 0.0f, 65535.0f));

        SDL_RumbleGamepad(Active(), rumble, rumble, rumble ? 1000 : 0);
    }
}

class ioPeriodicRumbleEffect : public ioRumbleEffect
{
public:
    using ioRumbleEffect::ioRumbleEffect;

    virtual f32 CalculatePeriodic(f32 offset) = 0;

    virtual f32 GetSustainLevel() const
    {
        return static_cast<f32>(Period.dwMagnitude) / 10000.0f;
    }

    f32 Calculate(u32 now, u32 /*duration*/) final override
    {
        now %= Period.dwPeriod;
        now = (now * 36000) / Period.dwPeriod;
        now = (now + Period.dwPhase) % 36000;
        return CalculatePeriodic(static_cast<f32>(now) / 36000.0f);
    }

    void SetParameters(const DIEFFECT& params, DWORD dwFlags) override
    {
        if (dwFlags & DIEP_TYPESPECIFICPARAMS)
        {
            ArAssert(params.cbTypeSpecificParams == sizeof(DIPERIODIC), "Invalid effect");
            Period = *(LPDIPERIODIC) params.lpvTypeSpecificParams;
        }

        ioRumbleEffect::SetParameters(params, dwFlags);
    }

private:
    DIPERIODIC Period {};
};

class ioSquareRumbleEffect final : public ioPeriodicRumbleEffect
{
public:
    using ioPeriodicRumbleEffect::ioPeriodicRumbleEffect;

    f32 CalculatePeriodic(f32 /*offset*/) override
    {
        return 1.0f;
    }
};

class ioSawtoothDownRumbleEffect final : public ioPeriodicRumbleEffect
{
public:
    using ioPeriodicRumbleEffect::ioPeriodicRumbleEffect;

    f32 CalculatePeriodic(f32 offset) override
    {
        return 1.0f - offset;
    }
};

class ioRumbleEffect_DIEffect final : public IDirectInputEffect
{
public:
    ioRumbleEffect_DIEffect(GUID guid, Ptr<ioRumbleEffect> effect)
        : Guid(guid)
        , Effect(std::move(effect))
    {}

    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(REFIID /*riid*/, LPVOID* /*ppvObj*/) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD_(ULONG, AddRef)() override
    {
        return ++RefCount;
    }

    STDMETHOD_(ULONG, Release)() override
    {
        ULONG refs = --RefCount;

        if (refs == 0)
        {
            ArWithStaticHeap static_heap;

            delete this;
        }

        return refs;
    }

    /*** IDirectInputEffect methods ***/
    STDMETHOD(Initialize)(HINSTANCE, DWORD, REFGUID)
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(GetEffectGuid)(LPGUID pguid)
    {
        *pguid = Guid;
        return DI_OK;
    }

    STDMETHOD(GetParameters)(LPDIEFFECT, DWORD)
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(SetParameters)(LPCDIEFFECT peff, DWORD dwFlags)
    {
        Effect->SetParameters(*peff, dwFlags);
        return DI_OK;
    }

    STDMETHOD(Start)(DWORD dwIterations, DWORD /*dwFlags*/)
    {
        Effect->Start(ioRumbleEffect::Now(), dwIterations);
        return DI_OK;
    }

    STDMETHOD(Stop)()
    {
        Effect->Stop();

        return DI_OK;
    }

    STDMETHOD(GetEffectStatus)(LPDWORD)
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(Download)()
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(Unload)()
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(Escape)(LPDIEFFESCAPE)
    {
        return FunctionNotImplemented();
    }

private:
    ULONG RefCount {1};
    GUID Guid {};
    Ptr<ioRumbleEffect> Effect {};
};

class ioGamepad_DIDevice2A final : public IDirectInputDevice2A
{
public:
    ioGamepad_DIDevice2A(std::vector<SDL_Gamepad*> gamepads)
        : Gamepad(std::move(gamepads))
    {}

    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID* ppvObj) override
    {
        if (riid == IID_IDirectInputDevice2A)
        {
            *ppvObj = static_cast<IDirectInputDevice2A*>(this);
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    STDMETHOD_(ULONG, AddRef)() override
    {
        return ++RefCount;
    }

    STDMETHOD_(ULONG, Release)() override
    {
        ULONG refs = --RefCount;

        if (refs == 0)
        {
            ArWithStaticHeap static_heap;

            delete this;
        }

        return refs;
    }

    /*** IDirectInputDeviceA methods ***/
    STDMETHOD(GetCapabilities)(LPDIDEVCAPS lpDIDevCaps) override
    {
        if (lpDIDevCaps->dwSize != sizeof(*lpDIDevCaps))
            return DIERR_INVALIDPARAM;

        *lpDIDevCaps = {sizeof(*lpDIDevCaps)};
        lpDIDevCaps->dwDevType = MAKEWORD(DIDEVTYPE_JOYSTICK, DIDEVTYPEJOYSTICK_GAMEPAD);
        lpDIDevCaps->dwAxes = 5;
        lpDIDevCaps->dwPOVs = 1;
        lpDIDevCaps->dwButtons = 10;

        if (Gamepad.CanRumble)
            lpDIDevCaps->dwFlags |= DIDC_FORCEFEEDBACK;

        return DI_OK;
    }

    STDMETHOD(EnumObjects)(LPDIENUMDEVICEOBJECTSCALLBACKA, LPVOID, DWORD) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(GetProperty)(REFGUID, LPDIPROPHEADER) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(SetProperty)(REFGUID, LPCDIPROPHEADER) override
    {
        // TODO: Handle setting properties

        return DI_OK;

        // return FunctionNotImplemented();
    }

    STDMETHOD(Acquire)() override
    {
        return DI_OK;
    }

    STDMETHOD(Unacquire)() override
    {
        return DI_OK;
    }

    STDMETHOD(GetDeviceState)(DWORD cbData, LPVOID lpvData) override
    {
        if (cbData != sizeof(DIJOYSTATE))
            return DIERR_INVALIDPARAM;

        DIJOYSTATE* state = (DIJOYSTATE*) lpvData;

        *state = {};

        SDL_Gamepad* gamepad = Gamepad.Active();

        const auto get_axis = [gamepad](SDL_GamepadAxis axis, LONG min, LONG max) -> LONG {
            return (((SDL_GetGamepadAxis(gamepad, axis) + 32768) * (max - min)) / 65535) + min;
        };

        state->lX = get_axis(SDL_GAMEPAD_AXIS_LEFTX, -2000, 2000); // XAxis
        state->lY = get_axis(SDL_GAMEPAD_AXIS_LEFTY, -2000, 2000); // YAxis

        state->lZ = get_axis(SDL_GAMEPAD_AXIS_LEFT_TRIGGER, -2000, 2000) -
            get_axis(SDL_GAMEPAD_AXIS_RIGHT_TRIGGER, -2000, 2000); // ZAxis

        state->lRx = get_axis(SDL_GAMEPAD_AXIS_RIGHTX, -2000, 2000); // UAxis
        state->lRy = get_axis(SDL_GAMEPAD_AXIS_RIGHTY, -2000, 2000); // VAxis

        const auto get_button = [gamepad](SDL_GamepadButton button) -> Uint8 {
            return SDL_GetGamepadButton(gamepad, button);
        };

        Uint8 dpad_py = get_button(SDL_GAMEPAD_BUTTON_DPAD_UP);
        Uint8 dpad_ny = get_button(SDL_GAMEPAD_BUTTON_DPAD_DOWN);
        Uint8 dpad_nx = get_button(SDL_GAMEPAD_BUTTON_DPAD_LEFT);
        Uint8 dpad_px = get_button(SDL_GAMEPAD_BUTTON_DPAD_RIGHT);

        const DWORD pov_rot[16] {0xFFFFFFFF, 27000, 18000, 22500, 9000, 0xFFFFFFFF, 13500, 18000, 0, 31500, 0xFFFFFFFF,
            27000, 4500, 0, 9000, 0xFFFFFFFF};

        state->rgdwPOV[0] = pov_rot[(dpad_py << 3) | (dpad_px << 2) | (dpad_ny << 1) | (dpad_nx << 0)];

        state->rgbButtons[0] = get_button(SDL_GAMEPAD_BUTTON_SOUTH) ? 0x80 : 0x00;
        state->rgbButtons[1] = get_button(SDL_GAMEPAD_BUTTON_EAST) ? 0x80 : 0x00;
        state->rgbButtons[2] = get_button(SDL_GAMEPAD_BUTTON_WEST) ? 0x80 : 0x00;
        state->rgbButtons[3] = get_button(SDL_GAMEPAD_BUTTON_NORTH) ? 0x80 : 0x00;
        state->rgbButtons[4] = get_button(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER) ? 0x80 : 0x00;
        state->rgbButtons[5] = get_button(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER) ? 0x80 : 0x00;
        state->rgbButtons[6] = get_button(SDL_GAMEPAD_BUTTON_BACK) ? 0x80 : 0x00;
        state->rgbButtons[7] = get_button(SDL_GAMEPAD_BUTTON_START) ? 0x80 : 0x00;
        state->rgbButtons[8] = get_button(SDL_GAMEPAD_BUTTON_LEFT_STICK) ? 0x80 : 0x00;
        state->rgbButtons[9] = get_button(SDL_GAMEPAD_BUTTON_RIGHT_STICK) ? 0x80 : 0x00;

        return DI_OK;
    }

    STDMETHOD(GetDeviceData)(DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(SetDataFormat)(LPCDIDATAFORMAT lpdf) override
    {
        LPCDIDATAFORMAT joy_fmt = &c_dfDIJoystick; // DIJOYSTATE

        if (lpdf->dwSize != joy_fmt->dwSize || lpdf->dwObjSize != joy_fmt->dwObjSize ||
            lpdf->dwFlags != joy_fmt->dwFlags || lpdf->dwDataSize != joy_fmt->dwDataSize ||
            lpdf->dwNumObjs != joy_fmt->dwNumObjs)
            return DIERR_INVALIDPARAM;

        for (DWORD i = 0; i < lpdf->dwNumObjs; ++i)
        {
            LPDIOBJECTDATAFORMAT obj1 = &lpdf->rgodf[i];
            LPDIOBJECTDATAFORMAT obj2 = &joy_fmt->rgodf[i];

            if (((obj1->pguid && obj2->pguid) ? (*obj1->pguid != *obj2->pguid) : (obj1->pguid || obj2->pguid)) ||
                obj1->dwOfs != obj2->dwOfs || obj1->dwType != obj2->dwType || obj1->dwFlags != obj2->dwFlags)
                return DIERR_INVALIDPARAM;
        }

        return DI_OK;
    }

    STDMETHOD(SetEventNotification)(HANDLE) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(SetCooperativeLevel)(HWND /*hwnd*/, DWORD /*dwFlags*/) override
    {
        return DI_OK;
    }

    STDMETHOD(GetObjectInfo)(LPDIDEVICEOBJECTINSTANCEA, DWORD, DWORD) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(GetDeviceInfo)(LPDIDEVICEINSTANCEA pdidi) override
    {
        if (pdidi->dwSize != sizeof(*pdidi))
            return DIERR_INVALIDPARAM;

        *pdidi = {sizeof(*pdidi)};

        pdidi->guidInstance = GUID_Joystick;
        const char* name = (Gamepad.Gamepads.size() == 1) ? SDL_GetGamepadName(Gamepad.Gamepads[0]) : nullptr;
        arts_sprintf(pdidi->tszInstanceName, "%s (SDL)", name ? name : "Unified Game Controller");
        pdidi->dwDevType = MAKEWORD(DIDEVTYPE_JOYSTICK, DIDEVTYPEJOYSTICK_GAMEPAD);

        return DI_OK;
    }

    STDMETHOD(RunControlPanel)(HWND, DWORD) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(Initialize)(HINSTANCE, DWORD, REFGUID) override
    {
        return FunctionNotImplemented();
    }

    /*** IDirectInputDevice2A methods ***/
    STDMETHOD(CreateEffect)(
        REFGUID rguid, LPCDIEFFECT lpeff, LPDIRECTINPUTEFFECT* ppdeff, LPUNKNOWN /*punkOuter*/) override
    {
        ArWithStaticHeap static_heap;

        *ppdeff = nullptr;

        Ptr<ioRumbleEffect> effect;

        if (rguid == GUID_Square)
            effect = arnew ioSquareRumbleEffect(&Gamepad);
        else if (rguid == GUID_SawtoothDown)
            effect = arnew ioSawtoothDownRumbleEffect(&Gamepad);
        else
            return DIERR_INVALIDPARAM;

        if (lpeff)
        {
            DWORD dwFlags = DIEP_ALLPARAMS;

            if (!lpeff->lpvTypeSpecificParams)
                dwFlags &= ~DIEP_TYPESPECIFICPARAMS;

            effect->SetParameters(*lpeff, dwFlags);
        }

        *ppdeff = new ioRumbleEffect_DIEffect(rguid, std::move(effect));

        return DI_OK;
    }

    STDMETHOD(EnumEffects)(LPDIENUMEFFECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwEffType) override
    {
        if (dwEffType == DIEFT_PERIODIC)
        {
            DIEFFECTINFOA effect {sizeof(effect), GUID_Square, DIEFT_PERIODIC, 0, 0, "Square"};
            lpCallback(&effect, pvRef);
            return DI_OK;
        }

        return DIERR_UNSUPPORTED;
    }

    STDMETHOD(GetEffectInfo)(LPDIEFFECTINFOA, REFGUID) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(GetForceFeedbackState)(LPDWORD) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(SendForceFeedbackCommand)(DWORD) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(EnumCreatedEffectObjects)(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK, LPVOID, DWORD) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(Escape)(LPDIEFFESCAPE) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(Poll)() override
    {
        Gamepad.Update();

        return DI_OK;
    }

    STDMETHOD(SendDeviceData)(DWORD, LPDIDEVICEOBJECTDATA, LPDWORD, DWORD) override
    {
        return FunctionNotImplemented();
    }

private:
    ULONG RefCount {1};
    ioGamepad Gamepad;
};

class ioGamepad_DI2A final : public IDirectInput2A
{
public:
    ioGamepad_DI2A()
    {
        if (!SDL_WasInit(SDL_INIT_GAMEPAD))
            SDL_InitSubSystem(SDL_INIT_GAMEPAD);
    }

    /*** IUnknown methods ***/
    STDMETHOD(QueryInterface)(REFIID /*riid*/, LPVOID* /*ppvObj*/) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD_(ULONG, AddRef)() override
    {
        return ++RefCount;
    }

    STDMETHOD_(ULONG, Release)() override
    {
        ULONG refs = --RefCount;

        if (refs == 0)
        {
            ArWithStaticHeap static_heap;

            delete this;
        }

        return refs;
    }

    /*** IDirectInputA methods ***/
    STDMETHOD(CreateDevice)
    (REFGUID rguid, LPDIRECTINPUTDEVICEA* lplpDirectInputDevice, LPUNKNOWN /*pUnkOuter*/) override
    {
        HRESULT result = DIERR_DEVICENOTREG;

        *lplpDirectInputDevice = nullptr;

        if (rguid == GUID_Joystick)
        {
            ArWithStaticHeap static_heap;

            int num_gamepads = 0;
            SDL_JoystickID* gamepads_ids = SDL_GetGamepads(&num_gamepads);

            if (num_gamepads > 0)
            {
                std::vector<SDL_Gamepad*> gamepads;

                for (int i = 0; i < num_gamepads; ++i)
                {
                    if (SDL_Gamepad* gamepad = SDL_OpenGamepad(gamepads_ids[i]))
                        gamepads.push_back(gamepad);
                }

                if (!gamepads.empty())
                {
                    *lplpDirectInputDevice = new ioGamepad_DIDevice2A(std::move(gamepads));
                    result = DI_OK;
                }
            }

            SDL_free(gamepads_ids);
        }

        return result;
    }

    STDMETHOD(EnumDevices)(DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags) override
    {
        if (dwDevType == DIDEVTYPE_JOYSTICK && dwFlags == DIEDFL_ATTACHEDONLY)
        {
            if (SDL_HasGamepad())
            {
                DIDEVICEINSTANCEA dev_inst = {sizeof(dev_inst)};
                dev_inst.guidInstance = GUID_Joystick;
                arts_strncpy(dev_inst.tszInstanceName, "SDL Unified Game Controller", ARTS_TRUNCATE);
                dev_inst.dwDevType = MAKEWORD(DIDEVTYPE_JOYSTICK, DIDEVTYPEJOYSTICK_GAMEPAD);

                lpCallback(&dev_inst, pvRef);
            }

            return DI_OK;
        }

        return FunctionNotImplemented();
    }

    STDMETHOD(GetDeviceStatus)(REFGUID) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(RunControlPanel)(HWND, DWORD) override
    {
        return FunctionNotImplemented();
    }

    STDMETHOD(Initialize)(HINSTANCE, DWORD) override
    {
        return FunctionNotImplemented();
    }

    /*** IDirectInput2A methods ***/
    STDMETHOD(FindDevice)(REFGUID, LPCSTR, LPGUID) override
    {
        return FunctionNotImplemented();
    }

private:
    ULONG RefCount {1};
};

IDirectInputA* Create_SDL_IDirectInput2A()
{
    ArWithStaticHeap static_heap;

    return new ioGamepad_DI2A();
}