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

#include "wpobject.h"

#include "arts7/node.h"
#include "data7/str.h"
#include "mmaudio/mmvoicecommentary.h"
#include "mmaudio/sound.h"
#include "vector7/vector2.h"
#include "vector7/vector3.h"
#include "vector7/vector4.h"

class mmPlayer;

class mmWaypoints final : public asNode
{
public:
    // ??0mmWaypoints@@QAE@XZ
    ARTS_EXPORT mmWaypoints();

    // ??1mmWaypoints@@UAE@XZ
    ARTS_EXPORT ~mmWaypoints() override;

    // ?AIWPHit@mmWaypoints@@QAEHHHVMatrix34@@VVector3@@M@Z
    ARTS_IMPORT i32 AIWPHit(i32 arg1, i32 arg2, Matrix34 arg3, Vector3 arg4, f32 arg5);

    // ?AnyAIWPHit@mmWaypoints@@QAEHAAIHVMatrix34@@VVector3@@M@Z
    ARTS_IMPORT i32 AnyAIWPHit(u32& arg1, i32 arg2, Matrix34 arg3, Vector3 arg4, f32 arg5);

    // ?AnyWPHits@mmWaypoints@@QAEHH@Z
    ARTS_IMPORT i32 AnyWPHits(i32 chain_id);

    // ?BlitzRemove@mmWaypoints@@QAEHH@Z
    ARTS_EXPORT b32 BlitzRemove(i32 index);

    // ?CalculateGatePoints@mmWaypoints@@QAEXVVector3@@MMPAVVector2@@1@Z
    ARTS_EXPORT void CalculateGatePoints(Vector3 center, f32 heading_rad, f32 radius, Vector2* out_left, Vector2* out_right);

    // ?ClearWaypoint@mmWaypoints@@QAEXH@Z
    ARTS_EXPORT void ClearWaypoint(i32 wp_index);

    // ?Cull@mmWaypoints@@UAEXXZ
    ARTS_EXPORT void Cull() override;

    // ?CycleCurrentWaypoint@mmWaypoints@@QAEXH@Z
    ARTS_EXPORT void CycleCurrentWaypoint(i32 direction);

    // ?DeactivateFinish@mmWaypoints@@QAEXXZ
    ARTS_EXPORT void DeactivateFinish();

    // ?DisplayHUDMessage@mmWaypoints@@QAEXHH@Z
    ARTS_EXPORT void DisplayHUDMessage(mmHUDMessageType msg_type, i32 wp_index);

    // ?GenerateHitRooms@mmWaypoints@@QAEXXZ
    ARTS_EXPORT void GenerateHitRooms();

    // ?GetClosestWaypoint@mmWaypoints@@QAEXXZ
    ARTS_EXPORT void GetClosestWaypoint();

    // ?GetHeading@mmWaypoints@@QAEMH@Z
    ARTS_EXPORT f32 GetHeading(i32 index);

    // ?GetHitRoom@mmWaypoints@@QAEHH@Z
    ARTS_EXPORT i32 GetHitRoom(i32 index);

    // ?GetLastWaypoint@mmWaypoints@@QAEXXZ
    ARTS_EXPORT void GetLastWaypoint();

    // ?GetNextWaypoint@mmWaypoints@@QAEXXZ
    ARTS_EXPORT void GetNextWaypoint();

    // ?GetStart@mmWaypoints@@QAEXAAVVector3@@@Z
    ARTS_EXPORT void GetStart(Vector3& out_pos);

    // ?GetStartAngle@mmWaypoints@@QAEMXZ
    ARTS_EXPORT f32 GetStartAngle();

    // ?GetWaypoint@mmWaypoints@@QAEXHAAVVector3@@@Z
    ARTS_EXPORT void GetWaypoint(i32 index, Vector3& out_pos);

    // ?Init@mmWaypoints@@QAEHPAVmmPlayer@@PADHHHH@Z
    ARTS_EXPORT i32 Init(mmPlayer* player, char* race_name, mmGameMode race_type, i32 reverse, i32 total_laps, i32 num_laps);

    // ?LineIntersect@mmWaypoints@@QAEHVVector2@@000M@Z
    ARTS_IMPORT i32 LineIntersect(Vector2 arg1, Vector2 arg2, Vector2 arg3, Vector2 arg4, f32 arg5);

    // ?LoadCSV@mmWaypoints@@QAEHPADH@Z
    ARTS_EXPORT i32 LoadCSV(char* race_name, i32 reverse);

    // ?Reset@mmWaypoints@@UAEXXZ
    ARTS_EXPORT void Reset() override;

    // ?ResetAllTags@mmWaypoints@@QAEXXZ
    ARTS_EXPORT void ResetAllTags();

    // ?SetArrow@mmWaypoints@@QAEXXZ
    ARTS_EXPORT void SetArrow();

    // ?SetCurrentGoals@mmWaypoints@@QAEXH@Z
    ARTS_EXPORT void SetCurrentGoals(i32 wp_idx);

    // ?Update@mmWaypoints@@UAEXXZ
    ARTS_EXPORT void Update() override;

    // ?UpdateWPHUD@mmWaypoints@@QAEXXZ
    ARTS_EXPORT void UpdateWPHUD();

    // ?WPHit@mmWaypoints@@QAEHHVVector3@@HH@Z
    ARTS_IMPORT i32 WPHit(i32 wp_index, Vector3 car_pos, i32 chain_id, i32 arg4);

private:
    void LoadBlitzWaypoints(i32 reverse);
    void LoadCheckpointWaypoints(i32 reverse);
    void LoadCircuitWaypoints(i32 reverse);

    void AllocateWaypointArrays(i32 count);
    i32 ResolveCsvIndex(i32 csv_idx, i32 reverse);
    void ReadPosition(i32 csv_idx, i32 wp_idx, Vector4& out_pos, f32& out_radius, i32 reverse);
    mmWaypointObject* CreateWaypoint(Vector4& pos, const char* type, i32 wp_idx, f32 radius, f32 default_rad);
    void AddWaypoint(i32 wp_idx, i32 pos_idx, mmWaypointObject* wp, b32 is_last);

    void SetStartPosition(i32 pos_idx);
    f32 RecallRadius(i32 csv_idx);
    f32 CalcHeading(i32 from_idx, i32 to_idx);
    void ComputeGatePoints(i32 wp_idx, i32 pos_idx);
    void ComputePrevGatePoints(i32 wp_idx, i32 pos_idx);

    void UpdateCarBounds();
    void HandleWaypointHit(i32 hit);

public:
    mmGameMode RaceType; // 0x20
    i32 field_24;        // 0x24

    i32 CurrentWaypoint; // 0x28
    i32 PositionCount;   // 0x2C

    i32 Finished; // 0x30
    i32 HitId;    // 0x34

    i32 field_38;   // 0x38
    i32 TotalLaps;  // 0x3C
    i32 CurrentLap; // 0x40

    i32 NumLaps;       // 0x44
    i32 Disabled;      // 0x48
    i32 LastClearedWP; // 0x4C

    f32 LapStartTime; // 0x50
    i32 LastWaypoint; // 0x54

    i32 IdentMask;      // 0x58
    Vector3* Positions; // 0x5C

    Vector2* GatePointsLeft;  // 0x60
    Vector2* GatePointsRight; // 0x64

    Vector3 StartPos; // 0x68

    Vector2 CarRearPos;  // 0x74
    Vector2 CarFrontPos; // 0x7C

    f32 StartAngle; // 0x84

    mmWaypointObject** Waypoints; // 0x88
    i32 dword8C;                  // 0x8C

    mmPlayer* Player;            // 0x90
    Ptr<AudSound> WaypointSound; // 0x94

    i32 field_98; // 0x98

    Ptr<AudSound> LastWaypointSound;    // 0x9C
    mmVoiceCommentary* VoiceCommentary; // 0xA0

    string stringA4; // 0xA4
};

check_size(mmWaypoints, 0xAC);

struct mmGate
{
public:
    // ?CloseEnough@mmGate@@QAEHMM@Z
    ARTS_IMPORT i32 CloseEnough(f32 arg1, f32 arg2);

    // ?LineEq@mmGate@@QAEMMM@Z
    ARTS_IMPORT f32 LineEq(f32 arg1, f32 arg2);

    u8 gap0[0x18];
};

check_size(mmGate, 0x18);

// ?Size@@3VVector3@@A
ARTS_IMPORT extern Vector3 Size;