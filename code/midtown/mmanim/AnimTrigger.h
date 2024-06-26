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

/*
    mmanim:AnimTrigger

    0x4C41E0 | public: __thiscall mmAnimTrigger::mmAnimTrigger(void) | ??0mmAnimTrigger@@QAE@XZ
    0x4C41F0 | public: __thiscall mmAnimTrigger::~mmAnimTrigger(void) | ??1mmAnimTrigger@@QAE@XZ
    0x4C4200 | public: void __thiscall mmAnimTrigger::Init(class mmInstance *,int) | ?Init@mmAnimTrigger@@QAEXPAVmmInstance@@H@Z
    0x4C4220 | public: void __thiscall mmAnimTrigger::Init(class Vector3 *,float *) | ?Init@mmAnimTrigger@@QAEXPAVVector3@@PAM@Z
    0x4C4240 | public: void __thiscall mmAnimTrigger::Init(int *,float) | ?Init@mmAnimTrigger@@QAEXPAHM@Z
    0x4C4270 | public: void __thiscall mmAnimTrigger::Init(float) | ?Init@mmAnimTrigger@@QAEXM@Z
    0x4C4290 | public: int __thiscall mmAnimTrigger::CheckCondition(void) | ?CheckCondition@mmAnimTrigger@@QAEHXZ
    0x4C4300 | public: int __thiscall mmAnimTrigger::CheckObjectHit(void) | ?CheckObjectHit@mmAnimTrigger@@QAEHXZ
    0x4C4320 | public: int __thiscall mmAnimTrigger::CheckTimeDelay(void) | ?CheckTimeDelay@mmAnimTrigger@@QAEHXZ
    0x4C4350 | public: int __thiscall mmAnimTrigger::CheckStateStatus(void) | ?CheckStateStatus@mmAnimTrigger@@QAEHXZ
    0x4C43A0 | public: void __thiscall mmAnimTrigger::Reset(void) | ?Reset@mmAnimTrigger@@QAEXXZ
    0x4C43B0 | public: int __thiscall mmAnimTrigger::CheckObjectDistance(void) | ?CheckObjectDistance@mmAnimTrigger@@QAEHXZ
*/

class mmInstance;

class mmAnimTrigger
{
public:
    // ??0mmAnimTrigger@@QAE@XZ
    ARTS_IMPORT mmAnimTrigger();

    // ??1mmAnimTrigger@@QAE@XZ
    ARTS_IMPORT ~mmAnimTrigger() = default;

    // ?CheckCondition@mmAnimTrigger@@QAEHXZ
    ARTS_IMPORT i32 CheckCondition();

    // ?CheckObjectDistance@mmAnimTrigger@@QAEHXZ
    ARTS_IMPORT i32 CheckObjectDistance();

    // ?CheckObjectHit@mmAnimTrigger@@QAEHXZ
    ARTS_IMPORT i32 CheckObjectHit();

    // ?CheckStateStatus@mmAnimTrigger@@QAEHXZ
    ARTS_IMPORT i32 CheckStateStatus();

    // ?CheckTimeDelay@mmAnimTrigger@@QAEHXZ
    ARTS_IMPORT i32 CheckTimeDelay();

    // ?Init@mmAnimTrigger@@QAEXM@Z | unused
    ARTS_IMPORT void Init(f32 arg1);

    // ?Init@mmAnimTrigger@@QAEXPAVVector3@@PAM@Z
    ARTS_IMPORT void Init(Vector3* arg1, f32* arg2);

    // ?Init@mmAnimTrigger@@QAEXPAVmmInstance@@H@Z | unused
    ARTS_IMPORT void Init(mmInstance* arg1, i32 arg2);

    // ?Init@mmAnimTrigger@@QAEXPAHM@Z | unused
    ARTS_IMPORT void Init(i32* arg1, f32 arg2);

    // ?Reset@mmAnimTrigger@@QAEXXZ
    ARTS_IMPORT void Reset();

    u8 gap0[0x24];
};

check_size(mmAnimTrigger, 0x24);
