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
    mmcamcs:viewcs

    0x4F8FB0 | public: __thiscall mmViewCS::mmViewCS(void) | ??0mmViewCS@@QAE@XZ
    0x4F9070 | public: virtual __thiscall mmViewCS::~mmViewCS(void) | ??1mmViewCS@@UAE@XZ
    0x4F9080 | public: static class mmViewCS * __cdecl mmViewCS::Instance(class asCamera *) | ?Instance@mmViewCS@@SAPAV1@PAVasCamera@@@Z
    0x4F9110 | public: void __thiscall mmViewCS::SetCurrentCam(class CarCamCS *) | ?SetCurrentCam@mmViewCS@@QAEXPAVCarCamCS@@@Z
    0x4F9180 | public: int __thiscall mmViewCS::NewCam(class CarCamCS *,int,float,class Callback) | ?NewCam@mmViewCS@@QAEHPAVCarCamCS@@HMVCallback@@@Z
    0x4F9230 | public: virtual void __thiscall mmViewCS::Update(void) | ?Update@mmViewCS@@UAEXXZ
    0x4F9260 | public: void __thiscall mmViewCS::Init(void) | ?Init@mmViewCS@@QAEXXZ
    0x4F9270 | public: void __thiscall mmViewCS::SetCamera(class asCamera *) | ?SetCamera@mmViewCS@@QAEXPAVasCamera@@@Z
    0x4F92C0 | public: virtual void __thiscall mmViewCS::Reset(void) | ?Reset@mmViewCS@@UAEXXZ
    0x4F9320 | public: void __thiscall mmViewCS::ForceMatrixDelta(class Matrix34 const &) | ?ForceMatrixDelta@mmViewCS@@QAEXABVMatrix34@@@Z
    0x4F9340 | public: void __thiscall mmViewCS::ForceMatrixDelta(class Vector3 const &) | ?ForceMatrixDelta@mmViewCS@@QAEXABVVector3@@@Z
    0x4F9360 | public: void __thiscall mmViewCS::OneShot(void) | ?OneShot@mmViewCS@@QAEXXZ
    0x4F9380 | public: static void __cdecl mmViewCS::DeclareFields(void) | ?DeclareFields@mmViewCS@@SAXXZ
    0x4F94F0 | public: virtual class MetaClass * __thiscall mmViewCS::GetClass(void) | ?GetClass@mmViewCS@@UAEPAVMetaClass@@XZ
    0x4F9500 | public: virtual void * __thiscall mmViewCS::`vector deleting destructor'(unsigned int) | ??_EmmViewCS@@UAEPAXI@Z
    0x4F9560 | public: virtual void __thiscall mmViewCS::AddWidgets(class Bank *) | ?AddWidgets@mmViewCS@@UAEXPAVBank@@@Z
    0x620118 | const mmViewCS::`vftable' | ??_7mmViewCS@@6B@
    0x7193A8 | class MetaClass mmViewCSMetaClass | ?mmViewCSMetaClass@@3VMetaClass@@A
*/

#include "arts7/linear.h"

class asCamera;
class CarCamCS;

class mmViewCS final : public asLinearCS
{
public:
    // ??0mmViewCS@@QAE@XZ
    ARTS_IMPORT mmViewCS();

    // ??_EmmViewCS@@UAEPAXI@Z
    // ??1mmViewCS@@UAE@XZ
    ARTS_IMPORT ~mmViewCS() override = default;

#ifdef ARTS_DEV_BUILD
    // ?AddWidgets@mmViewCS@@UAEXPAVBank@@@Z | inline
    ARTS_EXPORT void AddWidgets(Bank* arg1) override;
#endif

    // ?ForceMatrixDelta@mmViewCS@@QAEXABVMatrix34@@@Z | unused
    ARTS_IMPORT void ForceMatrixDelta(const Matrix34& arg1);

    // ?ForceMatrixDelta@mmViewCS@@QAEXABVVector3@@@Z | unused
    ARTS_IMPORT void ForceMatrixDelta(const Vector3& arg1);

    // ?GetClass@mmViewCS@@UAEPAVMetaClass@@XZ
    ARTS_IMPORT MetaClass* GetClass() override;

    // ?Init@mmViewCS@@QAEXXZ
    ARTS_EXPORT void Init();

    // ?NewCam@mmViewCS@@QAEHPAVCarCamCS@@HMVCallback@@@Z
    ARTS_IMPORT i32 NewCam(CarCamCS* arg1, i32 arg2, f32 arg3, Callback arg4);

    // ?OneShot@mmViewCS@@QAEXXZ | unused
    ARTS_IMPORT void OneShot();

    // ?Reset@mmViewCS@@UAEXXZ
    ARTS_IMPORT void Reset() override;

    // ?SetCamera@mmViewCS@@QAEXPAVasCamera@@@Z
    ARTS_IMPORT void SetCamera(asCamera* arg1);

    // ?SetCurrentCam@mmViewCS@@QAEXPAVCarCamCS@@@Z
    ARTS_IMPORT void SetCurrentCam(CarCamCS* arg1);

    // ?Update@mmViewCS@@UAEXXZ
    ARTS_IMPORT void Update() override;

    // ?DeclareFields@mmViewCS@@SAXXZ
    ARTS_IMPORT static void DeclareFields();

    // ?Instance@mmViewCS@@SAPAV1@PAVasCamera@@@Z
    ARTS_IMPORT static mmViewCS* Instance(asCamera* arg1);

    u8 gap88[0x38];
};

check_size(mmViewCS, 0xC0);
