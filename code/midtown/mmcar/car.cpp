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

define_dummy_symbol(mmcar_car);

#include "car.h"

#include "agi/dlptmpl.h"
#include "agi/getdlp.h"
#include "arts7/sim.h"
#include "data7/timer.h"
#include "mmcity/cullcity.h"
#include "mmcityinfo/state.h"
#include "mmcityinfo/vehlist.h"
#include "mminput/input.h"
#include "mmphysics/joint3dof.h"

#include "playercaraudio.h"
#include "trailer.h"

static mem::cmd_param PARAM_aiphysics {"aiphysics"};

// ?ffval@@3MA
ARTS_EXPORT f32 ffval = 0.0f;

static f32 fftime = 0.0f;

void mmCar::ApplyAiPhysics()
{
    if (!Sim.FrontLeft.OnGround && !Sim.FrontRight.OnGround && !Sim.BackLeft.OnGround && !Sim.BackRight.OnGround)
    {
        Sim.ICS.AngularMomentum *= 0.1f;
        CHEATING = true;
    }
}

mmCar::mmCar()
{
    // FIXME: mmCar is already oversampled by mmPhysicsMGR
    AddChild(&OverSample);
    OverSample.AddChild(&Sim);
    OverSample.RealTime(35.0f);

    AddChild(&FLSkid);
    AddChild(&FRSkid);
    AddChild(&BLSkid);
    AddChild(&BRSkid);
}

void mmCar::TranslateFlags(i32 info_flags)
{
    u32 flags = CAR_FLAG_ACTIVE;

    flags |= (info_flags & VEH_INFO_FLAG_6_WHEELS) ? CAR_FLAG_6_WHEELS : 0;
    flags |= (info_flags & VEH_INFO_FLAG_TRAILER) ? CAR_FLAG_TRAILER : 0;
    flags |= (info_flags & VEH_INFO_FLAG_FENDERS) ? CAR_FLAG_FENDERS : 0;
    flags |= (info_flags & VEH_INFO_FLAG_SIREN) ? CAR_FLAG_SIREN : 0;
    flags |= (info_flags & VEH_INFO_FLAG_LARGE) ? CAR_FLAG_LARGE : 0;
    flags |= (info_flags & VEH_INFO_FLAG_AXLES) ? CAR_FLAG_AXLES : 0;

    Model.CarFlags = flags;
}

void mmCar::Update()
{
#ifdef ARTS_DEV_BUILD
    Timer t;
#endif

    OverSample.Update();

    if (PARAM_aiphysics && MMSTATE.NetworkStatus == 0)
        ApplyAiPhysics();

#ifdef ARTS_DEV_BUILD
    f32 elapsed = t.Time();
    mmCar::UpdateTime += elapsed;
    mmCar::TotalUpdateTime += elapsed;
#endif
}

b32 mmCar::IsDrivingDisabled()
{
    return (Sim.ICS.Constraints & (ICS_CONSTRAIN_TX | ICS_CONSTRAIN_TZ | ICS_CONSTRAIN_RY)) != 0;
}

void mmCar::PostUpdate()
{
#ifdef ARTS_DEV_BUILD
    Timer t;
#endif

    if (Model.HasTrailer())
    {
        Trailer->PostUpdate();

        // FIXME: If the trailer isn't active, the car won't move either
        if (TrailerJoint->IsNodeActive())
            TrailerJoint->MoveICS();
    }
    else
    {
        Sim.ICS.MoveICS();
    }

    FLSkid.Update();
    FRSkid.Update();
    BLSkid.Update();
    BRSkid.Update();

    Shards.Update();

    Sim.SmokeParticles.Update();
    Sim.GrassParticles.Update();
    Sim.ExplosionParticles.Update();

#ifdef ARTS_DEV_BUILD
    f32 elapsed = t.Time();
    mmCar::PostUpdateTime += elapsed;
    mmCar::TotalUpdateTime += elapsed;
#endif

    CullCity()->ReparentObject(&Model);
}

void mmCar::ReInit(aconst char* name, i32 variant)
{
    mmVehInfo* veh_info = VehList()->GetVehicleInfo(name);

    if (!veh_info)
    {
        Errorf("This vehicle doens't exist");
        return;
    }

    TranslateFlags(veh_info->Flags);

    Sim.ReInit(name);
    Model.Init(name, this, variant);

    FLSkid.ReInit(&Sim.FrontLeft);
    FRSkid.ReInit(&Sim.FrontRight);
    BLSkid.ReInit(&Sim.BackLeft);
    BRSkid.ReInit(&Sim.BackRight);

    CullCity()->ReparentObject(&Model);

    if (Model.HasTrailer())
    {
        Vector3 trailer_pos {};
        Vector3 joint_pos {0.0f, 0.7f, 3.0f};

        DLPTemplate* dlp = GetDLPTemplate(arts_formatf<128>("%s_trailer", name));

        dlp->GetCentroid(trailer_pos, "TRAILER_H"_xconst);

        Trailer->Init(name, &Sim, trailer_pos);

        trailer_pos.y += Sim.LCS.Matrix.m3.y;
        TrailerJoint->InitJoint3Dof(&Sim.ICS, joint_pos, &Trailer->ICS, joint_pos - trailer_pos);

        Trailer->Activate();
        CullCity()->ReparentObject(&Trailer->Inst);
        TrailerJoint->UnbreakJoint();

        Sim.EnableExhaust = true;

        dlp->Release();
    }
    else
    {
        Trailer->Deactivate();

        if (Trailer->Inst.IsParented())
            CullCity()->ObjectsChain.Unparent(&Trailer->Inst);

        TrailerJoint->BreakJoint();

        Sim.EnableExhaust = false;
    }

    Reset();
}

void mmCar::ReleaseTrailer()
{
    if (Model.HasTrailer())
    {
        TrailerJoint->BreakJoint();
        Trailer->SetHackedImpactParams();
    }
}

void mmCar::RemoveVehicleAudio()
{
    Sim.RemoveNetVehicleAudio();
}

void mmCar::Reset()
{
    if (Model.HasSiren())
        Model.CarFlags &= ~CAR_FLAG_SIREN_ON;

    Sim.Reset();
    Model.Reset();
    ClearDamage();

    if (Model.HasTrailer())
    {
        TrailerJoint->UnbreakJoint();

        Sim.Reset();
        Trailer->Reset();
        TrailerJoint->Reset();
    }

    FLSkid.Reset();
    FRSkid.Reset();
    BLSkid.Reset();
    BRSkid.Reset();

    fftime = 0.0f;
}

void mmCar::StartSiren()
{
    Model.CarFlags |= CAR_FLAG_SIREN_ON;
    Sim.PlayerCarAudio->StartSiren();
}

void mmCar::StopSiren()
{
    Model.CarFlags &= ~CAR_FLAG_SIREN_ON;
    Sim.PlayerCarAudio->StopSiren();
}

void mmCar::ToggleSiren()
{
    if (Model.CarFlags & CAR_FLAG_SIREN_ON)
        StopSiren();
    else
        StartSiren();
}

void mmCar::ClearDamage()
{
    Sim.CurrentDamage = 0.0f;
    Model.ClearDamage(false);

    Sim.Splash.Reset();
    Sim.SetGlobalTuning(1.0f, 1.0f);
}

void mmCar::EnableDriving(b32 enabled)
{
    if (enabled)
    {
        Sim.ICS.Constraints &= ~ICS_CONSTRAIN_ALL;

        if (Model.HasTrailer())
            Trailer->ICS.Constraints &= ~ICS_CONSTRAIN_ALL;
    }
    else
    {
        Sim.ICS.Constraints |= ICS_CONSTRAIN_TX | ICS_CONSTRAIN_TZ | ICS_CONSTRAIN_RY;

        // TODO: Should this be TX|TZ|RY as well?
        if (Model.HasTrailer())
            Trailer->ICS.Constraints |= ICS_CONSTRAIN_ALL;
    }
}

void mmCar::Impact(mmIntersection* isect, Vector3* velocity, f32 energy, i32 audio_id, Vector3* impulse)
{
    if (Sim.Splash.IsNodeActive())
        return;

    // velocity is the relative velocity of the two objects colliding
    // impulse is the change in momentum of the object, based on velocity, normals, friction, etc.
    // damage is the magnitude of the impulse

    // TODO: Maybe remove the deflection calculation entirely? It's already factored into the impulse/damage value
    f32 deflection = -(*velocity ^ isect->Normal);
    f32 abs_deflection = std::abs(deflection);

    Vector3 where = isect->Position ^ Sim.LCS.World.FastInverse();

    if (Sim.EnableDamage)
    {
        if (deflection > 4.0f)
            Sim.CurrentDamage += energy;

        if (energy >= (Sim.MaxDamageScaled * 0.005f))
            Model.ApplyDamage(where, 0.5f);
    }

    if ((energy > 10000.0f) && (audio_id == 9))
        Sim.OnImpact.Call();

    if (Sim.IsPlayer() && GameInput()->DoingFF() && (Sim.SpeedMPH > 5.0f))
    {
        Vector3 impulse_direction = *impulse ^ Sim.LCS.World.FastInverse();
        f32 impulse_angle = std::atan2f(impulse_direction.x, impulse_direction.y) * -ARTS_RAD_TO_DEG;

        if (impulse_angle < 0.0f)
            impulse_angle = 360.0f - std::fmodf(-impulse_angle, 360.0f);
        else if (impulse_angle > 360.0f)
            impulse_angle = std::fmodf(impulse_angle - 360.0f, 360.0f);

        // Accumulate impacts over a short amount of time
        static const f32 ff_dropoff = 100.0f;
        ffval = std::max(ffval - (::Sim()->GetElapsed() - fftime) * ff_dropoff, 0.0f);
        fftime = ::Sim()->GetElapsed();

        f32 ff_scale = std::sqrtf(energy) * 0.01f;
        ffval = std::min(ffval + ff_scale, 1.0f);

        GameInput()->FFSetValues(MM_FF_COLLIDE, std::max(ffval, 0.1f), impulse_angle);
        GameInput()->FFPlay(MM_FF_COLLIDE);
    }

    if (CullCity()->IsPolyWater(isect->HitPoly) && !Sim.Splash.IsNodeActive())
    {
        Sim.PlayImpactAudio(MM_IMPACT_AUDIO_26, isect, velocity);

        Sim.Splash.Activate(isect->Position.y);

        if (Model.HasTrailer())
            Trailer->Splash.Activate(isect->Position.y);

        if (Sim.IsPlayer())
            HitWaterTimer = 1.0f;
    }
    else
    {
        if ((abs_deflection > 0.25f) && (audio_id == MM_IMPACT_AUDIO_1 || audio_id == MM_IMPACT_AUDIO_9))
        {
            const f32 spark_density = 0.5f;

            Sim.Model->Sparks.RadialBlast(
                static_cast<i32>(abs_deflection * spark_density), isect->Position, isect->Normal);
        }

        if ((energy > 300.0f) && (Sim.CurrentDamage > Sim.MaxDamageScaled))
        {
            // TODO: Improve shard creation and create them even when not fully damaged.
            Shards.EmitShards(isect->Position, energy, abs_deflection);
            Model.Impact(&where);
        }

        Sim.PlayImpactAudio(static_cast<i16>(audio_id), isect, velocity);
    }
}

static const char* EggPlayerNames[14] {
    "vasedans",
    "vasedanl",
    "vavan",
    "vadiesels",
    "vacompact",
    "vapickup",
    "vabus",
    "vadelivery",
    "valimo",
    "valimoblack",
    "vataxi",
    "vataxicheck",
    "valimoangel",
    "vaboeing_small",
};

[[maybe_unused]] static const char* EggPlayerVehicles[14] {
    "vpcaddie",
    "vpbullet",
    "vpford",
    "vpbus",
    "vpbug",
    "vpford",
    "vpbus",
    "vpford",
    "vpmustang99",
    "vpmustang99",
    "vpcaddie",
    "vpcaddie",
    "vpmustang99",
    "vpbus",
};

i32 EggNameIndex(char* name)
{
    for (i32 i = 0; i < ARTS_SSIZE32(EggPlayerNames); ++i)
    {
        if (!std::strcmp(name, EggPlayerNames[i]))
            return i;
    }

    return -1;
}