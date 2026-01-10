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

#pragma once

#include <dinput.h>

// ?inputEnumEffectTypeProc@@YGHPBUDIEFFECTINFOA@@PAX@Z
ARTS_EXPORT BOOL PASCAL inputEnumEffectTypeProc(LPCDIEFFECTINFOA pdei, LPVOID pvRef);

class mmEffectFF
{
public:
    // ??0mmEffectFF@@QAE@XZ
    mmEffectFF();

#ifdef ARTS_STANDALONE
    virtual
#endif
        // ??1mmEffectFF@@QAE@XZ
        ~mmEffectFF();

    virtual b32 Init(IDirectInputDevice2A* device) = 0;

    // ?Play@mmEffectFF@@UAEHXZ | inline
    virtual b32 Play();

    // ?Stop@mmEffectFF@@UAEHXZ | inline
    virtual b32 Stop();

    // ?SetValues@mmEffectFF@@UAEHMM@Z | inline
    virtual b32 SetValues(f32 a, f32 b);

protected:
    void ReleaseEffect();

    i32 field_4 {};
    i32 field_8 {};
    i32 field_C {};
    DIEFFECT Effect {};
    DIENVELOPE Envelope {};
    DWORD Axes[2] {};
    LONG Directions[2] {};
    GUID EffectGuid {};
    IDirectInputEffect* DIEffect {};
    b32 Enabled {};
    b32 IsPlaying {};
};

check_size(mmEffectFF, 0x84);
