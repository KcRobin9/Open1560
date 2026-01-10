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

#include "effect.h"

class mmCollideFF final : public mmEffectFF
{
public:
    // ?Init@mmCollideFF@@UAEHPAUIDirectInputDevice2A@@@Z
    b32 Init(IDirectInputDevice2A* device) override;

    // ?Play@mmCollideFF@@UAEHXZ
    b32 Play() override;

    // ?Stop@mmCollideFF@@UAEHXZ
    b32 Stop() override;

    // ?SetValues@mmCollideFF@@UAEHMM@Z
    b32 SetValues(f32 a, f32 b) override;

    // ?Assign@mmCollideFF@@QAEHJJ@Z
    b32 Assign(ilong gain, ilong direction);

private:
    DIPERIODIC Period {};
    u32 field_94 {};
};

check_size(mmCollideFF, 0x98);
