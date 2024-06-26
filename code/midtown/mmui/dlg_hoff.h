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
    mmui:dlg_hoff

    0x49E3E0 | public: __thiscall Dialog_HallOfFame::Dialog_HallOfFame(int,float,float,float,float,char *) | ??0Dialog_HallOfFame@@QAE@HMMMMPAD@Z
    0x49E7E0 | public: virtual __thiscall Dialog_HallOfFame::~Dialog_HallOfFame(void) | ??1Dialog_HallOfFame@@UAE@XZ
    0x49E7F0 | public: virtual void __thiscall Dialog_HallOfFame::PreSetup(void) | ?PreSetup@Dialog_HallOfFame@@UAEXXZ
    0x49E800 | public: void __thiscall Dialog_HallOfFame::AddRaceRecord(int,char *,char *,char *,int,char *,char *) | ?AddRaceRecord@Dialog_HallOfFame@@QAEXHPAD00H00@Z
    0x49E940 | public: void __thiscall Dialog_HallOfFame::ResetRaceRecord(void) | ?ResetRaceRecord@Dialog_HallOfFame@@QAEXXZ
    0x49E9D0 | public: void __thiscall Dialog_HallOfFame::SetSortState(void) | ?SetSortState@Dialog_HallOfFame@@QAEXXZ
    0x49EA50 | public: void __thiscall Dialog_HallOfFame::InitRaceRecord(void) | ?InitRaceRecord@Dialog_HallOfFame@@QAEXXZ
    0x49EBE0 | public: virtual void * __thiscall Dialog_HallOfFame::`scalar deleting destructor'(unsigned int) | ??_GDialog_HallOfFame@@UAEPAXI@Z
    0x49EBE0 | public: virtual void * __thiscall Dialog_HallOfFame::`vector deleting destructor'(unsigned int) | ??_EDialog_HallOfFame@@UAEPAXI@Z
    0x61D608 | const Dialog_HallOfFame::`vftable' | ??_7Dialog_HallOfFame@@6B@
*/

#include "pu_menu.h"

class Dialog_HallOfFame final : public PUMenuBase
{
public:
    // ??0Dialog_HallOfFame@@QAE@HMMMMPAD@Z
    ARTS_IMPORT Dialog_HallOfFame(i32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, char* arg6);

    // ??_EDialog_HallOfFame@@UAEPAXI@Z
    // ??_GDialog_HallOfFame@@UAEPAXI@Z
    // ??1Dialog_HallOfFame@@UAE@XZ
    ARTS_IMPORT ~Dialog_HallOfFame() override = default;

    // ?AddRaceRecord@Dialog_HallOfFame@@QAEXHPAD00H00@Z
    ARTS_IMPORT void AddRaceRecord(i32 arg1, char* arg2, char* arg3, char* arg4, i32 arg5, char* arg6, char* arg7);

    // ?InitRaceRecord@Dialog_HallOfFame@@QAEXXZ
    ARTS_IMPORT void InitRaceRecord();

    // ?PreSetup@Dialog_HallOfFame@@UAEXXZ
    ARTS_IMPORT void PreSetup() override;

    // ?ResetRaceRecord@Dialog_HallOfFame@@QAEXXZ
    ARTS_IMPORT void ResetRaceRecord();

    // ?SetSortState@Dialog_HallOfFame@@QAEXXZ
    ARTS_IMPORT void SetSortState();

    u8 gapC0[0x24];
};

check_size(Dialog_HallOfFame, 0xE4);
