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

#include "mmphysics/entity.h"
#include "mmphysics/osample.h"

#include "carmodel.h"
#include "carsim.h"
#include "shard.h"
#include "skid.h"

class Joint3Dof;
class mmTrailer;

#define CAR_TYPE_PLAYER 0
#define CAR_TYPE_NETOBJECT 1
#define CAR_TYPE_OPPONENT 2
#define CAR_TYPE_POLICE 3

class mmCar final : public mmPhysEntity
{
public:
    // ??0mmCar@@QAE@XZ
    ARTS_EXPORT mmCar();

    // ??1mmCar@@UAE@XZ | inline
    ARTS_EXPORT ~mmCar() override = default;

#ifdef ARTS_DEV_BUILD
    // ?AddWidgets@mmCar@@UAEXPAVBank@@@Z
    ARTS_IMPORT void AddWidgets(Bank* arg1) override;
#endif

    // ?ClearDamage@mmCar@@QAEXXZ
    ARTS_EXPORT void ClearDamage();

    // ?EnableDriving@mmCar@@QAEXH@Z
    ARTS_EXPORT void EnableDriving(b32 enabled);

    // ?GetBound@mmCar@@UAEPAVasBound@@XZ | inline
    asBound* GetBound() override
    {
        return &Sim.Bound;
    }

    // ?GetClass@mmCar@@UAEPAVMetaClass@@XZ
    ARTS_IMPORT MetaClass* GetClass() override;

    // ?GetICS@mmCar@@UAEPAVasInertialCS@@XZ | inline
    asInertialCS* GetICS() override
    {
        return &Sim.ICS;
    }

    // ?Impact@mmCar@@QAEXPAVmmIntersection@@PAVVector3@@MH1@Z
    ARTS_EXPORT void Impact(mmIntersection* isect, Vector3* velocity, f32 energy, i32 audio_id, Vector3* impulse);

    // ?Init@mmCar@@QAEXPADHH@Z
    ARTS_IMPORT void Init(aconst char* arg1, i32 arg2, i32 arg3);

    // ?IsDrivingDisabled@mmCar@@QAEHXZ
    ARTS_EXPORT b32 IsDrivingDisabled();

    // ?PostUpdate@mmCar@@UAEXXZ
    void PostUpdate() override;

    // ?ReInit@mmCar@@QAEXPADH@Z
    ARTS_EXPORT void ReInit(aconst char* name, i32 variant);

    // ?ReleaseTrailer@mmCar@@QAEXXZ
    void ReleaseTrailer();

    // ?RemoveVehicleAudio@mmCar@@QAEXXZ
    void RemoveVehicleAudio();

    // ?Reset@mmCar@@UAEXXZ
    void Reset() override;

    // ?StartSiren@mmCar@@QAEXXZ
    ARTS_EXPORT void StartSiren();

    // ?StopSiren@mmCar@@QAEXXZ
    ARTS_EXPORT void StopSiren();

    // ?ToggleSiren@mmCar@@QAEXXZ
    void ToggleSiren();

    // ?TranslateFlags@mmCar@@QAEXH@Z
    ARTS_EXPORT void TranslateFlags(i32 info_flags);

    // ?Update@mmCar@@UAEXXZ
    void Update() override;

    // ?VehNameRemap@mmCar@@QAEPADPADH@Z
    ARTS_IMPORT char* VehNameRemap(char* arg1, i32 arg2);

    // ?DeclareFields@mmCar@@SAXXZ
    ARTS_IMPORT static void DeclareFields();

    void ApplyAiPhysics();

#ifdef ARTS_DEV_BUILD
    // ?PostUpdateTime@mmCar@@2MA
    ARTS_IMPORT static f32 PostUpdateTime;

    // ?ProbeTime@mmCar@@2MA
    ARTS_IMPORT static f32 ProbeTime;

    // ?TotalUpdateTime@mmCar@@2MA
    ARTS_IMPORT static f32 TotalUpdateTime;

    // ?UpdateTime@mmCar@@2MA
    ARTS_IMPORT static f32 UpdateTime;
#endif

    asOverSample OverSample {};
    mmCarSim Sim {};
    mmCarModel Model {};
    mmSkidManager FLSkid {};
    mmSkidManager FRSkid {};
    mmSkidManager BLSkid {};
    mmSkidManager BRSkid {};
    mmShardManager Shards {};
    b32 TrailerJoined {};
    Joint3Dof* TrailerJoint {};
    mmTrailer* Trailer {};
};

check_size(mmCar, 0x230C);

// ?EggNameIndex@@YAHPAD@Z
ARTS_EXPORT i32 EggNameIndex(char* name);
