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

define_dummy_symbol(mminput_road);

#include "road.h"

#include "input.h"

b32 mmRoadFF::Init(IDirectInputDevice2A* device)
{
    if (HRESULT err = device->EnumEffects(inputEnumEffectTypeProc, &EffectGuid, DIEFT_PERIODIC); FAILED(err))
    {
        Errorf("EnumEffects(Periodic Force) failed");
        return false;
    }

    ReleaseEffect();

    Period = {};
    Period.dwMagnitude = 2000;
    Period.dwPeriod = 5 * DI_SECONDS;

    Axes[0] = DIJOFS_X;
    Directions[0] = 0;

    Effect = {sizeof(Effect)};
    Effect.dwFlags = DIEFF_OBJECTOFFSETS | DIEFF_CARTESIAN;
    Effect.dwDuration = INFINITE;
    Effect.dwGain = DI_FFNOMINALMAX;
    Effect.dwTriggerButton = DIEB_NOTRIGGER;
    Effect.cAxes = 1;
    Effect.rgdwAxes = Axes;
    Effect.rglDirection = Directions;
    Effect.cbTypeSpecificParams = sizeof(Period);
    Effect.lpvTypeSpecificParams = &Period;

    if (HRESULT err = device->CreateEffect(EffectGuid, &Effect, &DIEffect, NULL); FAILED(err))
    {
        Errorf("mmRoadFF::CreateEffect failed - %08X", err);
        return false;
    }

    IsPlaying = false;
    Enabled = true;
    return true;
}

b32 mmRoadFF::Play()
{
    if (!Enabled || (GameInput()->RoadForceScale == 0.0f))
        return false;

    if (HRESULT err = DIEffect->Start(1, 0); FAILED(err))
    {
        Errorf("mmRoadFF::Play failed - %08X", err);
        return false;
    }

    IsPlaying = true;
    return true;
}

b32 mmRoadFF::Stop()
{
    if (!Enabled)
        return false;

    if (HRESULT err = DIEffect->Stop(); FAILED(err))
    {
        Errorf("mmRoadFF::Stop failed - %08X", err);
        return false;
    }

    IsPlaying = false;
    return true;
}

b32 mmRoadFF::SetValues(f32 a, f32 b)
{
    return Assign(static_cast<ilong>(a * DI_SECONDS), static_cast<ilong>(b * DI_FFNOMINALMAX));
}

b32 mmRoadFF::Assign(ilong period, ilong magnitude)
{
    f32 scale = GameInput()->RoadForceScale;

    if (!Enabled || scale == 0.0f)
        return false;

    Period = {};
    Period.dwMagnitude = static_cast<DWORD>(scale * magnitude);
    Period.dwPeriod = period;

    Effect = {sizeof(Effect)};
    Effect.lpvTypeSpecificParams = &Period;
    Effect.cbTypeSpecificParams = sizeof(Period);

    if (HRESULT err = DIEffect->SetParameters(&Effect, DIEP_TYPESPECIFICPARAMS); FAILED(err))
    {
        Errorf("mmRoadFF::Assign failed - %08X", err);
        return false;
    }

    return true;
}
