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

define_dummy_symbol(mminput_effect);

#include "effect.h"

mmEffectFF::mmEffectFF()
{
    // TODO: Remove this, initialize Effect during Init
    Effect.dwSize = sizeof(Effect);
    Effect.dwGain = DI_FFNOMINALMAX;
    Effect.dwTriggerButton = DIEB_NOTRIGGER;
    Effect.rgdwAxes = Axes;
    Effect.rglDirection = Directions;
}

mmEffectFF::~mmEffectFF()
{
    ReleaseEffect();
}

b32 mmEffectFF::Play()
{
    return false;
}

b32 mmEffectFF::Stop()
{
    return false;
}

b32 mmEffectFF::SetValues(f32 /*a*/, f32 /*b*/)
{
    return true;
}

void mmEffectFF::ReleaseEffect()
{
    if (DIEffect)
    {
        DIEffect->Release();
        DIEffect = nullptr;
    }
}

BOOL PASCAL inputEnumEffectTypeProc(LPCDIEFFECTINFOA pdei, LPVOID pvRef)
{
    if (pvRef)
    {
        *static_cast<LPGUID>(pvRef) = pdei->guid;
        Displayf("Enum Effect GUID = %x", pdei->guid.Data1);
    }

    return FALSE;
}
