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

define_dummy_symbol(mminput_collide);

#include "collide.h"

#include "input.h"

b32 mmCollideFF::Init(IDirectInputDevice2A* device)
{
    if (HRESULT err = device->EnumEffects(inputEnumEffectTypeProc, &EffectGuid, DIEFT_PERIODIC); FAILED(err))
    {
        Errorf("EnumEffects(Periodic Force) failed");
        return false;
    }

    ReleaseEffect();

    // TODO: Does this do anything? dwAttackTime is 0
    Envelope = {sizeof(Envelope)};
    Envelope.dwAttackLevel = DI_FFNOMINALMAX;

    Period = {};
    Period.dwMagnitude = DI_FFNOMINALMAX;
    Period.dwPeriod = static_cast<DWORD>(DI_SECONDS / GameInput()->CollideCooldown);

    Axes[0] = DIJOFS_X;
    Axes[1] = DIJOFS_Y;
    Directions[0] = 90 * DI_DEGREES;
    Directions[1] = 0;

    Effect = {sizeof(Effect)};
    Effect.dwFlags = DIEFF_OBJECTOFFSETS | DIEFF_POLAR;
    Effect.dwDuration = static_cast<DWORD>(GameInput()->CollidePeriod * DI_SECONDS);
    Effect.dwGain = DI_FFNOMINALMAX;
    Effect.dwTriggerButton = DIEB_NOTRIGGER;
    Effect.cAxes = 2;
    Effect.rgdwAxes = Axes;
    Effect.rglDirection = Directions;
    Effect.lpEnvelope = &Envelope;
    Effect.cbTypeSpecificParams = sizeof(Period);
    Effect.lpvTypeSpecificParams = &Period;

    if (HRESULT err = device->CreateEffect(EffectGuid, &Effect, &DIEffect, 0); FAILED(err))
    {
        Errorf("mmCollideFF::CreateEffect failed - %08X", err);
        return false;
    }

    IsPlaying = false;
    Enabled = true;
    return true;
}

b32 mmCollideFF::Play()
{
    if (!Enabled || (GameInput()->ForceFeedbackScale == 0.0f))
        return false;

    if (HRESULT err = DIEffect->Start(1, 0); FAILED(err))
    {
        Errorf("mmCollideFF::Play failed - %08X", err);
        return false;
    }

    return true;
}

b32 mmCollideFF::Stop()
{
    return Enabled;
}

b32 mmCollideFF::SetValues(f32 a, f32 b)
{
    return Assign(static_cast<ilong>(a * DI_FFNOMINALMAX), static_cast<ilong>(b));
}

b32 mmCollideFF::Assign(ilong gain, ilong direction)
{
    f32 scale = GameInput()->ForceFeedbackScale;

    if (!Enabled || (scale == 0.0f))
        return false;

    Directions[0] = std::clamp<ilong>(direction, 0, 360) * DI_DEGREES;
    Directions[1] = 0;

    Effect = {sizeof(Effect)};
    Effect.dwFlags = DIEFF_OBJECTOFFSETS | DIEFF_POLAR;
    Effect.dwGain = static_cast<DWORD>(scale * gain);
    Effect.cAxes = 2;
    Effect.rglDirection = Directions;

    if (HRESULT err = DIEffect->SetParameters(&Effect, DIEP_GAIN | DIEP_DIRECTION); FAILED(err))
    {
        Errorf("mmCollideFF::Assign failed - %08X", err);
        return false;
    }

    return true;
}
