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
    mmcityinfo:mapdata

    0x4D3270 | public: __thiscall mmMapData::mmMapData(void) | ??0mmMapData@@QAE@XZ
    0x4D3290 | public: virtual __thiscall mmMapData::~mmMapData(void) | ??1mmMapData@@UAE@XZ
    0x4D32F0 | public: void __thiscall mmMapData::NewItems(int) | ?NewItems@mmMapData@@QAEXH@Z
    0x4D3320 | public: void __thiscall mmMapData::SetItem(int,char *) | ?SetItem@mmMapData@@QAEXHPAD@Z
    0x4D3340 | public: char * __thiscall mmMapData::GetItem(int) | ?GetItem@mmMapData@@QAEPADH@Z
    0x4D3370 | public: int __thiscall mmMapData::GetNumItems(void) | ?GetNumItems@mmMapData@@QAEHXZ
    0x4D3380 | public: void __thiscall mmMapData::Append(char *) | ?Append@mmMapData@@QAEXPAD@Z
    0x4D3400 | public: static void __cdecl mmMapData::DeclareFields(void) | ?DeclareFields@mmMapData@@SAXXZ
    0x4D35B0 | public: virtual class MetaClass * __thiscall mmMapData::GetClass(void) | ?GetClass@mmMapData@@UAEPAVMetaClass@@XZ
    0x4D35C0 | public: virtual void * __thiscall mmMapData::`vector deleting destructor'(unsigned int) | ??_EmmMapData@@UAEPAXI@Z
    0x61F930 | const mmMapData::`vftable' | ??_7mmMapData@@6B@
    0x708690 | class MetaClass mmMapDataMetaClass | ?mmMapDataMetaClass@@3VMetaClass@@A
*/

#include "infobase.h"

class mmMapData final : public mmInfoBase
{
public:
    // ??0mmMapData@@QAE@XZ
    ARTS_IMPORT mmMapData();

    // ??_EmmMapData@@UAEPAXI@Z
    // ??1mmMapData@@UAE@XZ
    ARTS_IMPORT ~mmMapData() override;

    // ?Append@mmMapData@@QAEXPAD@Z | unused
    ARTS_IMPORT void Append(char* arg1);

    // ?GetClass@mmMapData@@UAEPAVMetaClass@@XZ
    ARTS_IMPORT MetaClass* GetClass() override;

    // ?GetItem@mmMapData@@QAEPADH@Z
    ARTS_IMPORT char* GetItem(i32 arg1);

    // ?GetNumItems@mmMapData@@QAEHXZ
    ARTS_IMPORT i32 GetNumItems();

    // ?NewItems@mmMapData@@QAEXH@Z
    ARTS_IMPORT void NewItems(i32 arg1);

    // ?SetItem@mmMapData@@QAEXHPAD@Z
    ARTS_IMPORT void SetItem(i32 arg1, char* arg2);

    // ?DeclareFields@mmMapData@@SAXXZ
    ARTS_IMPORT static void DeclareFields();

    u8 gap88[0x8];
};

check_size(mmMapData, 0x90);
