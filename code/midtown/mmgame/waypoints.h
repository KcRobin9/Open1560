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
#include "mmaudio/sound.h"
#include "mmaudio/mmvoicecommentary.h"
#include "vector7/vector2.h"
#include "vector7/vector3.h"
#include "vector7/vector4.h"

class mmPlayer;

class mmWaypoints final : public asNode
{
public:
    // ??0mmWaypoints@@QAE@XZ
    ARTS_IMPORT mmWaypoints();

    // ??1mmWaypoints@@UAE@XZ
    ARTS_IMPORT ~mmWaypoints() override;

    // ?AIWPHit@mmWaypoints@@QAEHHHVMatrix34@@VVector3@@M@Z
    ARTS_IMPORT i32 AIWPHit(i32 arg1, i32 arg2, Matrix34 arg3, Vector3 arg4, f32 arg5);

    // ?AnyAIWPHit@mmWaypoints@@QAEHAAIHVMatrix34@@VVector3@@M@Z
    ARTS_IMPORT i32 AnyAIWPHit(u32& arg1, i32 arg2, Matrix34 arg3, Vector3 arg4, f32 arg5);

    // ?AnyWPHits@mmWaypoints@@QAEHH@Z
    ARTS_IMPORT i32 AnyWPHits(i32 arg1);

    // ?BlitzRemove@mmWaypoints@@QAEHH@Z
    ARTS_IMPORT i32 BlitzRemove(i32 arg1);

    // ?CalculateGatePoints@mmWaypoints@@QAEXVVector3@@MMPAVVector2@@1@Z
    ARTS_IMPORT void CalculateGatePoints(Vector3 arg1, f32 arg2, f32 arg3, Vector2* arg4, Vector2* arg5);

    // ?ClearWaypoint@mmWaypoints@@QAEXH@Z
    ARTS_IMPORT void ClearWaypoint(i32 arg1);

    // ?Cull@mmWaypoints@@UAEXXZ
    ARTS_EXPORT void Cull() override;

    // ?CycleCurrentWaypoint@mmWaypoints@@QAEXH@Z
    ARTS_IMPORT void CycleCurrentWaypoint(i32 arg1);

    // ?DeactivateFinish@mmWaypoints@@QAEXXZ
    ARTS_IMPORT void DeactivateFinish();

    // ?DisplayHUDMessage@mmWaypoints@@QAEXHH@Z
    ARTS_IMPORT void DisplayHUDMessage(i32 arg1, i32 arg2);

    // ?GenerateHitRooms@mmWaypoints@@QAEXXZ
    ARTS_IMPORT void GenerateHitRooms();

    // ?GetClosestWaypoint@mmWaypoints@@QAEXXZ
    ARTS_IMPORT void GetClosestWaypoint();

    // ?GetHeading@mmWaypoints@@QAEMH@Z
    ARTS_IMPORT f32 GetHeading(i32 arg1);

    // ?GetHitRoom@mmWaypoints@@QAEHH@Z
    ARTS_IMPORT i32 GetHitRoom(i32 arg1);

    // ?GetLastWaypoint@mmWaypoints@@QAEXXZ
    ARTS_IMPORT void GetLastWaypoint();

    // ?GetNextWaypoint@mmWaypoints@@QAEXXZ
    ARTS_IMPORT void GetNextWaypoint();

    // ?GetStart@mmWaypoints@@QAEXAAVVector3@@@Z
    ARTS_IMPORT void GetStart(Vector3& arg1);

    // ?GetStartAngle@mmWaypoints@@QAEMXZ
    ARTS_IMPORT f32 GetStartAngle();

    // ?GetWaypoint@mmWaypoints@@QAEXHAAVVector3@@@Z
    ARTS_IMPORT void GetWaypoint(i32 arg1, Vector3& arg2);

    // ?Init@mmWaypoints@@QAEHPAVmmPlayer@@PADHHHH@Z
    ARTS_IMPORT i32 Init(mmPlayer* arg1, char* arg2, i32 arg3, i32 arg4, i32 arg5, i32 arg6);

    // ?LineIntersect@mmWaypoints@@QAEHVVector2@@000M@Z
    ARTS_IMPORT i32 LineIntersect(Vector2 arg1, Vector2 arg2, Vector2 arg3, Vector2 arg4, f32 arg5);

    // ?LoadCSV@mmWaypoints@@QAEHPADH@Z
    ARTS_EXPORT i32 LoadCSV(char* race_name, i32 reverse);

    // ?Reset@mmWaypoints@@UAEXXZ
    ARTS_IMPORT void Reset() override;

    // ?ResetAllTags@mmWaypoints@@QAEXXZ
    ARTS_IMPORT void ResetAllTags();

    // ?SetArrow@mmWaypoints@@QAEXXZ
    ARTS_IMPORT void SetArrow();

    // ?SetCurrentGoals@mmWaypoints@@QAEXH@Z
    ARTS_IMPORT void SetCurrentGoals(i32 arg1);

    // ?Update@mmWaypoints@@UAEXXZ
    ARTS_IMPORT void Update() override;

    // ?UpdateWPHUD@mmWaypoints@@QAEXXZ
    ARTS_IMPORT void UpdateWPHUD();

    // ?WPHit@mmWaypoints@@QAEHHVVector3@@HH@Z
    ARTS_IMPORT i32 WPHit(i32 arg1, Vector3 arg2, i32 arg3, i32 arg4);

private:
    // Helper methods for LoadCSV
    void AllocateWaypointArrays(i32 count);
    void SetStartPosition(i32 positionIndex);
    mmWaypointObject* CreateWaypointObject(Vector4& position, const char* typeName, i32 waypointIndex, f32 radius, f32 defaultRadius);
    void SetupGatePointsForWaypoint(i32 waypointIndex, i32 positionIndex);
    void SetupGatePointsForPreviousWaypoint(i32 waypointIndex, i32 positionIndex);
    f32 CalculateHeadingBetweenPositions(i32 fromIndex, i32 toIndex);

    void LoadBlitzWaypoints(i32 reverse);
    void LoadCheckpointWaypoints(i32 reverse);
    void LoadCircuitWaypoints(i32 reverse);

public:
    i32 RaceType;                       // 0x20: Type of race
    i32 field_24;                       // 0x24

    i32 CurrentWaypoint;                // 0x28: Current waypoint index
    i32 PositionCount;                  // 0x2C: Number of waypoint positions

    i32 field_30;                       // 0x30
    i32 HitId;                          // 0x34: ID of last hit waypoint

    i32 field_38;                       // 0x38
    i32 field_3C;                       // 0x3C
    i32 field_40;                       // 0x40

    i32 NumLaps;                        // 0x44: Number of laps for circuit races
    i32 dword48;                        // 0x48
    i32 dword4C;                        // 0x4C

    i32 field_50;                       // 0x50
    i32 LastWaypoint;                   // 0x54: Last waypoint index

    i32 dword58;                        // 0x58
    Vector3* Positions;                 // 0x5C: Array of waypoint positions

    Vector2* GatePointsLeft;            // 0x60: Left gate points for each waypoint
    Vector2* GatePointsRight;           // 0x64: Right gate points for each waypoint

    Vector3 StartPos;                   // 0x68: Starting position

    Vector2 field_74;                   // 0x74

    Vector2 field_7C;                   // 0x7C

    f32 StartAngle;                     // 0x84: Starting heading angle

    mmWaypointObject** Waypoints;       // 0x88: Array of waypoint objects
    i32 dword8C;                        // 0x8C

    mmPlayer* Player;                   // 0x90: Player reference
    AudSound* WaypointSound;            // 0x94: Sound for waypoint hits

    i32 field_98;                       // 0x98

    AudSound* LastWaypointSound;        // 0x9C: Sound for last waypoint
    mmVoiceCommentary* VoiceCommentary; // 0xA0: Voice commentary system

    string stringA4;                     // 0xA4: String storage
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
