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
    mmcar:networkcaraudio

    0x483440 | public: __thiscall mmNetworkCarAudio::mmNetworkCarAudio(class mmCarSim *) | ??0mmNetworkCarAudio@@QAE@PAVmmCarSim@@@Z
    0x4834A0 | public: void __thiscall mmNetworkCarAudio::Init(class mmCarSim *,unsigned short) | ?Init@mmNetworkCarAudio@@QAEXPAVmmCarSim@@G@Z
    0x483630 | public: virtual __thiscall mmNetworkCarAudio::~mmNetworkCarAudio(void) | ??1mmNetworkCarAudio@@UAE@XZ
    0x4836A0 | public: virtual void __thiscall mmNetworkCarAudio::Update(void) | ?Update@mmNetworkCarAudio@@UAEXXZ
    0x4836E0 | public: void __thiscall mmNetworkCarAudio::UpdateAudio(void) | ?UpdateAudio@mmNetworkCarAudio@@QAEXXZ
    0x483810 | public: void __thiscall mmNetworkCarAudio::FluctuateSlowSiren(void) | ?FluctuateSlowSiren@mmNetworkCarAudio@@QAEXXZ
    0x483A60 | public: void __thiscall mmNetworkCarAudio::FluctuateFastSiren(void) | ?FluctuateFastSiren@mmNetworkCarAudio@@QAEXXZ
    0x483E10 | public: void __thiscall mmNetworkCarAudio::DeactivateAudio(void) | ?DeactivateAudio@mmNetworkCarAudio@@QAEXXZ
    0x483EC0 | public: void __thiscall mmNetworkCarAudio::CalculateDistToPlayer2(void) | ?CalculateDistToPlayer2@mmNetworkCarAudio@@QAEXXZ
    0x483F80 | public: void __thiscall mmNetworkCarAudio::CalculateDoppler(void) | ?CalculateDoppler@mmNetworkCarAudio@@QAEXXZ
    0x483FA0 | public: void __thiscall mmNetworkCarAudio::UpdateDoppler(void) | ?UpdateDoppler@mmNetworkCarAudio@@QAEXXZ
    0x484250 | public: void __thiscall mmNetworkCarAudio::StopSiren(void) | ?StopSiren@mmNetworkCarAudio@@QAEXXZ
    0x484290 | public: void __thiscall mmNetworkCarAudio::StartSiren(void) | ?StartSiren@mmNetworkCarAudio@@QAEXXZ
    0x4842E0 | public: virtual void __thiscall mmNetworkCarAudio::Reset(void) | ?Reset@mmNetworkCarAudio@@UAEXXZ
    0x484300 | public: void __thiscall mmNetworkCarAudio::RemoveFromManager(void) | ?RemoveFromManager@mmNetworkCarAudio@@QAEXXZ
    0x484320 | public: void __thiscall mmNetworkCarAudio::LoadSucessful(class mmCarSim *) | ?LoadSucessful@mmNetworkCarAudio@@QAEXPAVmmCarSim@@@Z
    0x4843A0 | public: static void __cdecl mmNetworkCarAudio::DeclareFields(void) | ?DeclareFields@mmNetworkCarAudio@@SAXXZ
    0x484570 | public: virtual class MetaClass * __thiscall mmNetworkCarAudio::GetClass(void) | ?GetClass@mmNetworkCarAudio@@UAEPAVMetaClass@@XZ
    0x4846A0 | public: virtual void * __thiscall mmNetworkCarAudio::`vector deleting destructor'(unsigned int) | ??_EmmNetworkCarAudio@@UAEPAXI@Z
    0x484700 | public: __thiscall mmNetworkCarAudio::mmNetworkCarAudio(void) | ??0mmNetworkCarAudio@@QAE@XZ
    0x61CA30 | const mmNetworkCarAudio::`vftable' | ??_7mmNetworkCarAudio@@6B@
    0x6A81F0 | class MetaClass mmNetworkCarAudioMetaClass | ?mmNetworkCarAudioMetaClass@@3VMetaClass@@A
    0x6A822C | public: static short mmNetworkCarAudio::s_iPlayingFluctuator | ?s_iPlayingFluctuator@mmNetworkCarAudio@@2FA
*/

#include "arts7/node.h"

class mmCarSim;

class mmNetworkCarAudio final : public asNode
{
public:
    // ??0mmNetworkCarAudio@@QAE@XZ | inline
    ARTS_IMPORT mmNetworkCarAudio();

    // ??0mmNetworkCarAudio@@QAE@PAVmmCarSim@@@Z
    ARTS_IMPORT mmNetworkCarAudio(mmCarSim* arg1);

    // ??_EmmNetworkCarAudio@@UAEPAXI@Z
    // ??1mmNetworkCarAudio@@UAE@XZ
    ARTS_IMPORT ~mmNetworkCarAudio() override;

    // ?CalculateDistToPlayer2@mmNetworkCarAudio@@QAEXXZ
    ARTS_IMPORT void CalculateDistToPlayer2();

    // ?CalculateDoppler@mmNetworkCarAudio@@QAEXXZ
    ARTS_IMPORT void CalculateDoppler();

    // ?DeactivateAudio@mmNetworkCarAudio@@QAEXXZ
    ARTS_IMPORT void DeactivateAudio();

    // ?FluctuateFastSiren@mmNetworkCarAudio@@QAEXXZ
    ARTS_IMPORT void FluctuateFastSiren();

    // ?FluctuateSlowSiren@mmNetworkCarAudio@@QAEXXZ
    ARTS_IMPORT void FluctuateSlowSiren();

    // ?GetClass@mmNetworkCarAudio@@UAEPAVMetaClass@@XZ
    ARTS_IMPORT MetaClass* GetClass() override;

    // ?Init@mmNetworkCarAudio@@QAEXPAVmmCarSim@@G@Z
    ARTS_IMPORT void Init(mmCarSim* arg1, u16 arg2);

    // ?LoadSucessful@mmNetworkCarAudio@@QAEXPAVmmCarSim@@@Z
    ARTS_IMPORT void LoadSucessful(mmCarSim* arg1);

    // ?RemoveFromManager@mmNetworkCarAudio@@QAEXXZ
    ARTS_IMPORT void RemoveFromManager();

    // ?Reset@mmNetworkCarAudio@@UAEXXZ
    ARTS_IMPORT void Reset() override;

    // ?StartSiren@mmNetworkCarAudio@@QAEXXZ
    ARTS_IMPORT void StartSiren();

    // ?StopSiren@mmNetworkCarAudio@@QAEXXZ
    ARTS_IMPORT void StopSiren();

    // ?Update@mmNetworkCarAudio@@UAEXXZ
    ARTS_IMPORT void Update() override;

    // ?UpdateAudio@mmNetworkCarAudio@@QAEXXZ
    ARTS_IMPORT void UpdateAudio();

    // ?UpdateDoppler@mmNetworkCarAudio@@QAEXXZ
    ARTS_IMPORT void UpdateDoppler();

    // ?DeclareFields@mmNetworkCarAudio@@SAXXZ
    ARTS_IMPORT static void DeclareFields();

    // ?s_iPlayingFluctuator@mmNetworkCarAudio@@2FA
    ARTS_IMPORT static i16 s_iPlayingFluctuator;

    u8 gap20[0xB4];
};

check_size(mmNetworkCarAudio, 0xD4);
