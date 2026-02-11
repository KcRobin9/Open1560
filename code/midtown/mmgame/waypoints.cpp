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

define_dummy_symbol(mmgame_waypoints);

#include "waypoints.h"

#include "wpobject.h"

#include "data7/printer.h"
#include "localize/localize.h"
#include "mmaudio/sound.h"
#include "mmcity/cullcity.h"
#include "mmcity/positions.h"
#include "mmcityinfo/state.h"
#include "mmgame/hud.h"
#include "mmgame/player.h"
#include "vector7/matrix34.h"

static constexpr f32 PI = 3.1415926;
static constexpr f32 DEG_TO_RAD = PI / 180.0f;
static constexpr f32 RAD_TO_DEG = 180.0f / PI;
static constexpr i32 MAX_BLITZ_CHECKPOINTS = 6;
static constexpr f32 DEFAULT_RADIUS_NORMAL = 7.5f;
static constexpr f32 DEFAULT_RADIUS_BLITZ_EVENT_8 = 7.0f;
static constexpr f32 DEFAULT_RECALL_RADIUS = 15.0f;

mmWaypoints::mmWaypoints()
{
    WaypointSound = nullptr;
    LastWaypointSound = nullptr;
    dword8C = 0;
    PositionCount = 0;
    Disabled = 0;
    LastClearedWP = 0;
    NumLaps = 0;
    IdentMask = 1;
    GatePointsLeft = nullptr;
    GatePointsRight = nullptr;
    Positions = nullptr;
    Waypoints = nullptr;

    WaypointSound = arnew AudSound(AudSound::Get2DFlags(), 1, -1);
    LastWaypointSound = arnew AudSound(AudSound::Get2DFlags(), 1, -1);

    // Build pipe-delimited checkpoint name string: "Checkpoint 1|Checkpoint 2|...|Checkpoint 20"
    stringA4 = LOC_STR(MM_IDS_CHECKPOINT_1);
    for (u32 id = MM_IDS_CHECKPOINT_2; id <= MM_IDS_CHECKPOINT_20; ++id)
    {
        stringA4 += "|";
        stringA4 += AngelReadString(id)->Text;
    }

    VoiceCommentary = nullptr;
}

mmWaypoints::~mmWaypoints()
{
    RemoveAllChildren();

    if (PositionCount > 0)
    {
        delete[] Positions;
        delete[] GatePointsLeft;
        delete[] GatePointsRight;

        // Delete waypoints in reverse order (matching original)
        for (i32 i = PositionCount - 1; i >= 0; --i)
        {
            delete Waypoints[i];
        }

        delete[] Waypoints;
    }

    // Left as i32 until type is identified
    if (dword8C)
        delete reinterpret_cast<asNode*>(dword8C);
}

b32 mmWaypoints::AIWPHit(i32 wp_index, i32 /* arg2 */, Matrix34 matrix, Vector3 dimensions, f32 tolerance)
{
    Vector2 gate_left = GatePointsLeft[wp_index];
    Vector2 gate_right = GatePointsRight[wp_index];

    f32 tol_width = tolerance + dimensions.x;  // Tests 1 & 2: widened by car width
    f32 tol_length = tolerance + dimensions.z; // Test 3: widened by car length

    // Test 1: Z-axis pair (front/rear of car)
    {
        Vector3 front = Vector3 {0.0f, 0.0f, dimensions.z} ^ matrix;
        Vector3 rear = Vector3 {0.0f, 0.0f, -dimensions.z} ^ matrix;

        if (LineIntersect({front.x, front.z}, {rear.x, rear.z}, gate_left, gate_right, tol_width))
            return true;
    }

    // Test 2: Y-axis pair (catches tilted/sloped car scenarios)
    {
        Vector3 top = Vector3 {0.0f, dimensions.y, 0.0f} ^ matrix;
        Vector3 bottom = Vector3 {0.0f, -dimensions.y, 0.0f} ^ matrix;

        if (LineIntersect({top.x, top.z}, {bottom.x, bottom.z}, gate_left, gate_right, tol_width))
            return true;
    }

    // Test 3: X-axis pair (left/right sides of car)
    {
        Vector3 right = Vector3 {dimensions.x, 0.0f, 0.0f} ^ matrix;
        Vector3 left = Vector3 {-dimensions.x, 0.0f, 0.0f} ^ matrix;

        return LineIntersect({right.x, right.z}, {left.x, left.z}, gate_left, gate_right, tol_length);
    }
}

i32 mmWaypoints::AnyAIWPHit(u32& hit_mask, i32 arg2, Matrix34 matrix, Vector3 dimensions, f32 tolerance)
{
    if (PositionCount <= 1)
        return 0;

    for (i32 i = 1; i < PositionCount; ++i)
    {
        // Skip waypoints already hit this race
        if (hit_mask & (1 << i))
            continue;

        // Cull waypoints further than 50 units away
        Vector3 delta = matrix.m3 - Positions[i];
        if (delta.Mag2() > 50.0f * 50.0f)
            continue;

        if (AIWPHit(i, arg2, matrix, dimensions, tolerance))
        {
            hit_mask |= (1 << i);
            return 1;
        }
    }

    return 0;
}

i32 mmWaypoints::AnyWPHits(i32 /* chain_id */)
{
    if (PositionCount <= 0)
        return -1;

    asInertialCS* ics = Player->Car.GetICS();

    // Test 2: Y-axis pair - pre-compute once before the loop
    Vector3 y_bottom_w = Vector3 {0.0f, -Size.y, 0.0f} ^ ics->Matrix;
    Vector3 y_top_w = Vector3 {0.0f, Size.y, 0.0f} ^ ics->Matrix;
    Vector2 y_bottom = {y_bottom_w.x, y_bottom_w.z};
    Vector2 y_top = {y_top_w.x, y_top_w.z};

    // Test 3: X-axis pair - pre-compute once before the loop
    Vector3 x_left_w = Vector3 {-Size.x, 0.0f, 0.0f} ^ ics->Matrix;
    Vector3 x_right_w = Vector3 {Size.x, 0.0f, 0.0f} ^ ics->Matrix;
    Vector2 x_left = {x_left_w.x, x_left_w.z};
    Vector2 x_right = {x_right_w.x, x_right_w.z};

    for (i32 i = 0; i < PositionCount; ++i)
    {
        if (Waypoints[i]->Initialized)
            continue;

        Vector2 gate_left = GatePointsLeft[i];
        Vector2 gate_right = GatePointsRight[i];

        // Test 1: Front/rear of car (pre-computed in UpdateCarBounds)
        if (LineIntersect(CarRearPos, CarFrontPos, gate_left, gate_right, Size.x))
            return i;

        // Test 2: Y-axis pair (catches tilted car scenarios)
        if (LineIntersect(y_bottom, y_top, gate_left, gate_right, Size.x))
            return i;

        // Test 3: X-axis pair (left/right sides) - no tolerance
        if (LineIntersect(x_left, x_right, gate_left, gate_right, 0.0f))
            return i;
    }

    return -1;
}

b32 mmWaypoints::BlitzRemove(i32 index)
{
    i32 last_pos = PositionCount - 1;

    // Always keep the start position
    if (index == 0)
        return false;

    // Always keep the finish, or if using all available checkpoints
    if (last_pos == NumLaps || index == last_pos)
        return false;

    // Remove second-to-last to improve spacing
    if (index == last_pos - 1)
        return true;

    // When using 3 checkpoints: specifically keep checkpoint at position 1
    if (NumLaps == 3 && index == 1)
        return false;

    // When there are 2 positions between selected count and finish:
    // remove all except the one right before finish
    if (last_pos - NumLaps == 2 && index != last_pos - 2)
        return true;

    // When using only 1 checkpoint: remove everything except start, one middle, and finish
    if (NumLaps == 1)
        return true;

    // Special case for 6 total positions with 2 checkpoints selected:
    // Keep positions 1 and 5 for even distribution across the map
    return last_pos == 5 && NumLaps == 2 && index != 1 && index != 5;
}

void mmWaypoints::CalculateGatePoints(
    Vector3 center, f32 heading_rad, f32 radius, Vector2* out_left, Vector2* out_right)
{
    f32 offset_x = std::cos(heading_rad) * radius;
    f32 offset_z = std::sin(heading_rad) * radius;

    out_left->x = center.x + offset_x;
    out_left->y = center.z + offset_z;

    out_right->x = center.x - offset_x;
    out_right->y = center.z - offset_z;
}

void mmWaypoints::ClearWaypoint(i32 wp_index)
{
    LastClearedWP = wp_index;

    mmWaypointObject* wp = Waypoints[wp_index];

    if (!wp->Initialized)
        IdentMask |= wp->IdentMask;

    wp->Initialized = true;
    wp->Deactivate();

    ++LastWaypoint;

    // Play sounds based on race type
    if ((RaceType == mmGameMode::CnR || RaceType == mmGameMode::Circuit) && LastWaypoint != PositionCount - 1)
    {
        WaypointSound->PlayOnce(-1.0f, -1.0f);
    }

    if (RaceType == mmGameMode::Checkpoint)
    {
        // Play lap completion sound at start/finish, regular sound otherwise
        if ((LastWaypoint - 1) % PositionCount == 0)
            LastWaypointSound->PlayOnce(-1.0f, -1.0f);
        else
            WaypointSound->PlayOnce(-1.0f, -1.0f);
    }

    DisplayHUDMessage(mmHUDMessageType::CurrentTime, wp_index);
}

void mmWaypoints::Cull()
{}

void mmWaypoints::CycleCurrentWaypoint(i32 direction)
{
    i32 last_wp = PositionCount - 1;

    if (LastWaypoint == last_wp)
    {
        CurrentWaypoint = last_wp;
        return;
    }

    i32 step = (direction == 1) ? 1 : -1;
    i32 next_wp = CurrentWaypoint;

    while (true)
    {
        next_wp = (next_wp + step) % PositionCount;

        // Handle negative modulo for backwards cycling
        if (next_wp < 0)
            next_wp += PositionCount;

        // Skip start (0) and finish (last_wp) waypoints in cycling
        if (next_wp == 0 || next_wp == last_wp)
        {
            next_wp = (step == 1) ? 1 : PositionCount - 2;
        }

        // Found an uninitialized (available) waypoint
        if (!Waypoints[next_wp]->Initialized)
            break;

        // Cycled back to starting point without finding available waypoint
        if (next_wp == CurrentWaypoint)
            return;
    }

    SetCurrentGoals(next_wp);
}

void mmWaypoints::DeactivateFinish()
{
    Waypoints[PositionCount - 1]->Deactivate();
}

void mmWaypoints::DisplayHUDMessage(mmHUDMessageType msg_type, i32 /* wp_index */)
{
    char time_buffer[16];

    if (msg_type == mmHUDMessageType::CurrentTime)
    {
        f32 time = Player->Hud.LapTimer.GetTime();

        if (MMSTATE.GameMode == mmGameMode::Blitz)
            time -= LapStartTime;

        Player->Hud.GetTime(time_buffer, time);
        Player->Hud.SetMessage(LOC_TEXT(time_buffer), 1.0f, false);
    }
    else if (msg_type == mmHUDMessageType::LapTime)
    {
        f32 elapsed = Player->Hud.LapTimer.GetTime() - LapStartTime;

        Player->Hud.GetTime(time_buffer, elapsed);
        Player->Hud.SetMessage(LOC_STRING(MM_IDS_LAP_TIME), 1.0f, false);
        Player->Hud.SetMessage2(LOC_TEXT(time_buffer));
    }
    else if (msg_type == mmHUDMessageType::FinalLap)
    {
        f32 elapsed = Player->Hud.LapTimer.GetTime() - LapStartTime;

        Player->Hud.GetTime(time_buffer, elapsed);
        Player->Hud.SetMessage(LOC_STRING(MM_IDS_FINAL_LAP), 1.0f, false);
        Player->Hud.SetMessage2(LOC_TEXT(time_buffer));
    }
}

//void mmWaypoints::GenerateHitRooms()
//{
//    for (i32 i = 0; i < PositionCount; ++i)
//    {
//        Vector3 pos = Waypoints[i]->Position;
//        Waypoints[i]->HitId = CullCity()->GetHitId(pos);
//    }
//}


void mmWaypoints::GenerateHitRooms()
{
    for (i32 i = 0; i < PositionCount; ++i) // zie hierbovens
    {
        Vector3 pos = Waypoints[i]->Position;
        Waypoints[i-1]->HitId = CullCity()->GetHitId(pos);
    }
}


void mmWaypoints::GetClosestWaypoint()
{
    i32 best_idx = CurrentWaypoint;
    f32 min_dist_sq = 1000000000.0f;
    i32 limit = PositionCount - 1;
    i32 found = 0;

    Vector3 car_pos = Player->Car.Sim.ICS.Matrix.m3;

    if (limit > 1)
    {
        for (i32 i = 1; i < limit; ++i)
        {
            if (!Waypoints[i]->Initialized)
            {
                // f32 dist_sq = car_pos.Dist2(Waypoints[i]->Position);
                f32 dist_sq = car_pos.Dist2(Positions[i]);
                found++;

                if (dist_sq < min_dist_sq && best_idx != 0 && best_idx != limit)
                {
                    min_dist_sq = dist_sq;
                    best_idx = i;
                }
            }
        }
    }

    if (!found)
        best_idx = limit;

    SetCurrentGoals(best_idx);
}

f32 mmWaypoints::GetHeading(i32 index)
{
    if (index >= PositionCount)
        return 0.0f;
    return Waypoints[index]->Heading;
}

i32 mmWaypoints::GetHitRoom(i32 index)
{
    return Waypoints[index]->HitId;
}

void mmWaypoints::GetLastWaypoint()
{
    CycleCurrentWaypoint(0);
}

void mmWaypoints::GetNextWaypoint()
{
    CycleCurrentWaypoint(1);
}

void mmWaypoints::GetStart(Vector3& out_pos)
{
    out_pos = PositionCount > 0 ? StartPos : Vector3 {0.0f, 0.0f, 0.0f};
}

f32 mmWaypoints::GetStartAngle()
{
    if (Waypoints)
        return StartAngle;
    return 0.0f;
}

void mmWaypoints::GetWaypoint(i32 index, Vector3& out_pos)
{
    out_pos = index < PositionCount ? Waypoints[index]->Position : Vector3 {0.0f, 0.0f, 0.0f};
}

i32 mmWaypoints::Init(mmPlayer* player, char* race_name, i32 race_type, i32 reverse, i32 total_laps, i32 num_laps)
{
    Player = player;
    RaceType = static_cast<mmGameMode>(race_type); // RaceType = race_type;

    if (RaceType == mmGameMode::Circuit)
        NumLaps = num_laps;

    // WaypointSound->Load((char*) "Waypoint", 0);
    // WaypointSound->Load("Waypoint"_xconst, 0);

    char waypointName[] = "Waypoint";
    WaypointSound->Load(waypointName, 0);

    WaypointSound->SetVolume(0.91f, -1);
    WaypointSound->SetPriority(23);

    // LastWaypointSound->Load((char*) "Lastwaypoint", 0);
    // LastWaypointSound->Load("Lastwaypoint"_xconst, 0);

    char lastWaypointName[] = "Lastwaypoint";
    LastWaypointSound->Load(lastWaypointName, 0);

    LastWaypointSound->SetVolume(0.91f, -1);
    LastWaypointSound->SetPriority(23);

    char dest[80];
    arts_sprintf(dest, "%swaypoints", race_name);

    if (LoadCSV(race_name, reverse))
    {
        GenerateHitRooms();
        TotalLaps = total_laps;
        Reset();

        Vector3& dim = Player->Car.Sim.Dimensions;
        Size = dim;
        Size.x *= 0.5f; // make shorter?
        Size.z *= 0.5f; // make shorter?
        return 1;
    }

    Errorf("No waypoints specified");
    return 0;
}

b32 mmWaypoints::LineIntersect(Vector2 p1, Vector2 p2, Vector2 p3, Vector2 p4, f32 tolerance)
{
    // Padded bounding boxes for both segments
    f32 min_x1 = (p1.x < p2.x ? p1.x : p2.x) - tolerance;
    f32 min_y1 = (p1.y < p2.y ? p1.y : p2.y) - tolerance;
    f32 max_x1 = (p1.x > p2.x ? p1.x : p2.x) + tolerance;
    f32 max_y1 = (p1.y > p2.y ? p1.y : p2.y) + tolerance;

    f32 min_x2 = (p3.x < p4.x ? p3.x : p4.x) - tolerance;
    f32 min_y2 = (p3.y < p4.y ? p3.y : p4.y) - tolerance;
    f32 max_x2 = (p3.x > p4.x ? p3.x : p4.x) + tolerance;
    f32 max_y2 = (p3.y > p4.y ? p3.y : p4.y) + tolerance;

    // Compute slopes and y-intercepts: y = slope * x + intercept
    f32 dx1 = p1.x - p2.x;
    f32 slope1 = (p1.y - p2.y) / dx1;
    f32 slope2 = (p3.y - p4.y) / (p3.x - p4.x);

    f32 intercept1 = p1.y - slope1 * p1.x;
    f32 intercept2 = p3.y - slope2 * p3.x;

    // Find intersection point
    f32 ix, iy;

    if (slope1 == slope2)
    {
        // Parallel: project p1 onto line 2
        ix = p1.x;
        iy = slope2 * ix + intercept2;
    }
    else if (dx1 == 0.0f)
    {
        // Line 1 is vertical: project p3 onto line 1
        ix = p3.x;
        iy = slope1 * ix + intercept1;
    }
    else
    {
        // General case: solve slope1 * x + b1 = slope2 * x + b2
        ix = (intercept2 - intercept1) / (slope1 - slope2);
        iy = slope1 * ix + intercept1;
    }

    // Check intersection point lies within both padded bounding boxes
    return ix >= min_x1 && ix <= max_x1 && iy >= min_y1 && iy <= max_y1 && ix >= min_x2 && ix <= max_x2 &&
        iy >= min_y2 && iy <= max_y2;
}

i32 mmWaypoints::LoadCSV(char* race_name, i32 reverse)
{
    char path[128];
    arts_sprintf(path, "race\\%swaypoints.csv", race_name);

    InitPositions();
    LoadPositions(path);

    PositionCount = GetPositionCount();

    if (PositionCount == 0)
        return 0;

    switch (static_cast<mmGameMode>(RaceType))
    {
        case mmGameMode::Checkpoint: LoadCheckpointWaypoints(reverse); break;
        case mmGameMode::Circuit: LoadCircuitWaypoints(reverse); break;
        case mmGameMode::Blitz: LoadBlitzWaypoints(reverse); break;
        default: break;
    }

    if (Waypoints && Waypoints[0])
        StartAngle = Waypoints[0]->Heading;

    InitPositions();

    return 1;
}

void mmWaypoints::Reset()
{
    if (!PositionCount)
        return;

    Disabled = 0;
    CurrentWaypoint = 1;
    LastClearedWP = 0;
    Finished = 0;
    field_38 = 0;
    field_24 = 0;
    IdentMask = 1;

    HitId = GetHitRoom(1);
    ResetAllTags();

    switch (RaceType)
    {
        case mmGameMode::CnR:
            Waypoints[0]->Deactivate();
            if (PositionCount > 2)
                Waypoints[PositionCount - 1]->Deactivate();
            break;

        case mmGameMode::Checkpoint:
            for (i32 i = 0; i < PositionCount; ++i)
            {
                Waypoints[i]->Activate();
                Player->Hud.SetWPCleared(0, PositionCount);
            }
            Waypoints[CurrentWaypoint]->Activate();
            CurrentLap = 0;
            LapStartTime = 0.0f;
            break;

        case mmGameMode::Blitz: Waypoints[0]->Deactivate(); break;
    }

    if (RaceType != mmGameMode::Checkpoint)
        Waypoints[0]->Initialized = true;

    LastWaypoint = 1;
}

void mmWaypoints::ResetAllTags()
{
    for (i32 i = 0; i < PositionCount; ++i)
    {
        Waypoints[i]->Initialized = false;
        Waypoints[i]->Activate();
    }
}

void mmWaypoints::SetArrow()
{
    Player->Hud.Arrow.SetInterest(&Positions[CurrentWaypoint]);
}

void mmWaypoints::SetCurrentGoals(i32 wp_idx)
{
    i32 max_idx = PositionCount - 1;
    i32 target = wp_idx;

    if (target > 0)
    {
        if (target >= max_idx)
            target = max_idx;

        CurrentWaypoint = target;
        HitId = GetHitRoom(target);
    }
    else
    {
        CurrentWaypoint = 0;
        HitId = GetHitRoom(0);
    }
}

void mmWaypoints::Update()
{
    if (!PositionCount || Disabled)
        return;

    UpdateCarBounds();

    if (RaceType == mmGameMode::CnR || RaceType == mmGameMode::Circuit)
    {
        i32 hit = AnyWPHits(Player->Car.Model.ChainId);

        if (hit >= 0 && !Finished)
            HandleWaypointHit(hit);
    }
    else if (RaceType == mmGameMode::Checkpoint)
    {
        // Always update lap time display
        if (!Finished)
        {
            f32 elapsed = Player->Hud.LapTimer.GetTime() - LapStartTime;
            Player->Hud.SetLapTime(CurrentLap, elapsed, false);
        }

        Vector3 car_pos = Player->Car.GetICS()->Matrix.m3;
        i32 chain_id = Player->Car.Model.ChainId;

        if (WPHit(CurrentWaypoint, car_pos, chain_id, false) && !Finished)
        {
            Waypoints[CurrentWaypoint]->Deactivate();
            ClearWaypoint(CurrentWaypoint);

            if (CurrentWaypoint == 0)
            {
                // Crossed the start/finish line - completed a lap
                ++CurrentLap;

                if (CurrentLap == TotalLaps)
                {
                    Finished = true;
                }
                else if (CurrentLap == TotalLaps - 1)
                {
                    DisplayHUDMessage(mmHUDMessageType::FinalLap, 0);
                    if (VoiceCommentary)
                        VoiceCommentary->PlayFinalLap();
                }
                else
                {
                    DisplayHUDMessage(mmHUDMessageType::LapTime, 0);
                }

                // Record lap time for completed lap
                f32 elapsed = Player->Hud.LapTimer.GetTime() - LapStartTime;
                Player->Hud.SetLapTime(CurrentLap - 1, elapsed, true);

                // Update lap start time
                LapStartTime = Player->Hud.LapTimer.GetTime();

                if (!Finished)
                    ResetAllTags();
            }
            else
            {
                // Cleared an intermediate checkpoint
                if (CurrentWaypoint == PositionCount - 1 && CurrentLap == TotalLaps - 1)
                {
                    if (VoiceCommentary)
                        VoiceCommentary->PlayFinalCheckPoint();
                }
            }

            if (!Finished)
                Player->Hud.SetWPCleared(CurrentWaypoint, PositionCount);

            // Advance to next waypoint
            ++CurrentWaypoint;
            if (CurrentWaypoint == PositionCount)
                CurrentWaypoint = 0;

            if (CurrentWaypoint < PositionCount)
                HitId = GetHitRoom(CurrentWaypoint);
        }
    }

    if (!Finished)
    {
        SetArrow();
        UpdateWPHUD();
    }

    asNode::Update();
}

void mmWaypoints::UpdateWPHUD()
{
    Player->Hud.WaypointDist = Waypoints[CurrentWaypoint]->Position.Dist(Player->Car.Sim.ICS.Matrix.m3);
}

b32 mmWaypoints::WPHit(i32 wp_index, Vector3 /* pos */, i32 /* chain_id */, i32 /* arg4 */)
{
    Vector2 gate_left = GatePointsLeft[wp_index];
    Vector2 gate_right = GatePointsRight[wp_index];

    // Test 1: Front/rear of car (pre-computed in UpdateCarBounds)
    if (LineIntersect(CarRearPos, CarFrontPos, gate_left, gate_right, Size.x))
        return true;

    asInertialCS* ics = Player->Car.GetICS();

    // Test 2: Y-axis pair (catches tilted car scenarios)
    {
        Vector3 y_bottom_w = Vector3 {0.0f, -Size.y, 0.0f} ^ ics->Matrix;
        Vector3 y_top_w = Vector3 {0.0f, Size.y, 0.0f} ^ ics->Matrix;

        if (LineIntersect({y_bottom_w.x, y_bottom_w.z}, {y_top_w.x, y_top_w.z}, gate_left, gate_right, Size.x))
            return true;
    }

    // Test 3: X-axis pair (left/right sides) - no tolerance
    {
        Vector3 x_left_w = Vector3 {-Size.x, 0.0f, 0.0f} ^ ics->Matrix;
        Vector3 x_right_w = Vector3 {Size.x, 0.0f, 0.0f} ^ ics->Matrix;

        return LineIntersect({x_left_w.x, x_left_w.z}, {x_right_w.x, x_right_w.z}, gate_left, gate_right, 0.0f);
    }
}

// ---------------------------------------------------------
// Internal / Private Helpers
// ---------------------------------------------------------

void mmWaypoints::LoadBlitzWaypoints(i32 reverse)
{
    if (PositionCount > MAX_BLITZ_CHECKPOINTS)
    {
        Errorf("Blitz checkpoint count greater than 6!  Fred lied to me.");
        PositionCount = MAX_BLITZ_CHECKPOINTS;
    }

    if (NumLaps > PositionCount || NumLaps <= 0)
        NumLaps = PositionCount;

    i32 wp_count = NumLaps + 1;
    AllocateWaypointArrays(wp_count);
    SetStartPosition(reverse ? NumLaps : 0);

    i32 wp_idx = 0;
    i32 pos_idx = 0;

    for (i32 i = 0; i < PositionCount; ++i)
    {
        if (BlitzRemove(i))
            continue;

        Vector4 pos;
        f32 wp_rad;
        ReadPosition(i, pos_idx, pos, wp_rad, reverse);

        b32 is_last = (i == PositionCount - 1);

        mmWaypointObject* wp;
        if (is_last)
        {
            f32 default_rad = (MMSTATE.EventId == 8) ? DEFAULT_RADIUS_BLITZ_EVENT_8 : DEFAULT_RADIUS_NORMAL;
            wp = CreateWaypoint(pos, "pt_finish", wp_idx, wp_rad, default_rad);
        }
        else
        {
            wp = CreateWaypoint(pos, "pt_check", wp_idx, wp_rad, DEFAULT_RADIUS_NORMAL);
        }

        AddWaypoint(wp_idx, pos_idx, wp, is_last);

        Displayf("LoadCSV: Blitz Vert %d = %f, %f, %f", wp_idx, Positions[pos_idx].x, Positions[pos_idx].y,
            Positions[pos_idx].z);

        ++wp_idx;
        ++pos_idx;
    }

    PositionCount = NumLaps + 1;
}


// old
//void mmWaypoints::LoadCheckpointWaypoints(i32 reverse)
//{
//    AllocateWaypointArrays(PositionCount);
//    SetStartPosition(reverse ? (PositionCount - 1) : 0);
//
//    for (i32 i = 0; i < PositionCount; ++i)
//    {
//        Vector4 pos;
//        f32 wp_rad;
//        ReadPosition(i, i, pos, wp_rad, reverse);
//
//        b32 is_last = (i == PositionCount - 1);
//        const char* type = is_last ? "pt_finish" : "pt_check";
//        mmWaypointObject* wp = CreateWaypoint(pos, type, i, wp_rad, DEFAULT_RADIUS_NORMAL);
//
//        AddWaypoint(i, i, wp, is_last);
//
//        Displayf("LoadCSV: Waypoint Vert = %f, %f, %f", Positions[i].x, Positions[i].y, Positions[i].z);
//    }
//}


// new unfinished
void mmWaypoints::LoadCheckpointWaypoints(i32 reverse)
{
    AllocateWaypointArrays(PositionCount);

    // Original always uses index 0 for start position
    Vector4& startPos = GetPositionVector4(0);
    StartPos.x = startPos.x;
    StartPos.y = startPos.y;
    StartPos.z = startPos.z;

    for (i32 i = 0; i < PositionCount; ++i)
    {
        i32 read_idx = reverse ? (PositionCount - i) : i; // NOT PositionCount-i-1
        Vector4 pos = GetPositionVector4(read_idx);

        Positions[i].x = pos.x;
        Positions[i].y = pos.y;
        Positions[i].z = pos.z;

        f32 wp_rad = RecallRadius(reverse ? (PositionCount - i) : i);

        // i==0 is finish/start line, rest are checkpoints
        const char* type = (i == 0) ? "pt_finish" : "pt_check";
        i32 ident_mask = (i == 0) ? 1 : (1 << i); // hardcoded 1 for finish
        mmWaypointObject* wp =
            new mmWaypointObject(pos, const_cast<char*>(type), ident_mask, wp_rad, 3, DEFAULT_RADIUS_NORMAL);

        Waypoints[i] = wp;
        ComputePrevGatePoints(i, i);
        if (i == PositionCount - 1)
            ComputeGatePoints(i, i);
        AddChild(wp);

        Displayf("LoadCSV: Waypoint Vert = %f, %f, %f", Positions[i].x, Positions[i].y, Positions[i].z);
    }
}




void mmWaypoints::LoadCircuitWaypoints(i32 reverse)
{
    AllocateWaypointArrays(PositionCount);
    SetStartPosition(0);

    for (i32 i = 0; i < PositionCount; ++i)
    {
        // Circuit reverse uses (count - i), not (count - i - 1)
        i32 read_idx = reverse ? (PositionCount - i) : i;
        Vector4 pos = GetPositionVector4(read_idx);

        Positions[i].x = pos.x;
        Positions[i].y = pos.y;
        Positions[i].z = pos.z;

        f32 wp_rad = RecallRadius(read_idx);

        // First waypoint is the start/finish line
        const char* type = (i == 0) ? "pt_finish" : "pt_check";
        mmWaypointObject* wp = CreateWaypoint(pos, type, i, wp_rad, DEFAULT_RADIUS_NORMAL);

        Waypoints[i] = wp;

        if (i == 0)
            ComputeGatePoints(0, 0);

        ComputePrevGatePoints(i, i);

        // Last waypoint: auto-calculate heading pointing back to start
        if (i == PositionCount - 1)
        {
            if (pos.w == 0.0f)
                Waypoints[i]->SetHeading(CalcHeading(i, 0));

            f32 heading_rad = Waypoints[i]->Heading * DEG_TO_RAD;

            Vector2 left, right;
            CalculateGatePoints(Positions[i], heading_rad, wp_rad, &left, &right);

            GatePointsLeft[i] = left;
            GatePointsRight[i] = right;
        }

        AddChild(wp);

        Displayf("LoadCSV: Waypoint Vert = %f, %f, %f", Positions[i].x, Positions[i].y, Positions[i].z);
    }
}

void mmWaypoints::AllocateWaypointArrays(i32 count)
{
    Positions = new Vector3[count];
    Waypoints = new mmWaypointObject*[count];
    GatePointsLeft = new Vector2[count];
    GatePointsRight = new Vector2[count];
}

i32 mmWaypoints::ResolveCsvIndex(i32 csv_idx, i32 reverse)
{
    return reverse ? (PositionCount - csv_idx - 1) : csv_idx;
}

void mmWaypoints::ReadPosition(i32 csv_idx, i32 wp_idx, Vector4& out_pos, f32& out_radius, i32 reverse)
{
    i32 read_idx = ResolveCsvIndex(csv_idx, reverse);
    out_pos = GetPositionVector4(read_idx);

    Positions[wp_idx].x = out_pos.x;
    Positions[wp_idx].y = out_pos.y;
    Positions[wp_idx].z = out_pos.z;

    out_radius = RecallRadius(read_idx);
}

mmWaypointObject* mmWaypoints::CreateWaypoint(Vector4& pos, const char* type, i32 wp_idx, f32 radius, f32 default_rad)
{
    i32 ident_mask = 1 << wp_idx;
    return new mmWaypointObject(pos, const_cast<char*>(type), ident_mask, radius, 3, default_rad);
}

void mmWaypoints::AddWaypoint(i32 wp_idx, i32 pos_idx, mmWaypointObject* wp, b32 is_last)
{
    Waypoints[wp_idx] = wp;
    ComputePrevGatePoints(wp_idx, pos_idx);
    if (is_last)
        ComputeGatePoints(wp_idx, pos_idx);
    AddChild(wp);
}

void mmWaypoints::SetStartPosition(i32 pos_idx)
{
    Vector4& pos = GetPositionVector4(pos_idx);
    StartPos.x = pos.x;
    StartPos.y = pos.y;
    StartPos.z = pos.z;
}

f32 mmWaypoints::RecallRadius(i32 csv_idx)
{
    Matrix34 matrix;
    i32 radius = 0;
    RecallPosition(csv_idx, &matrix, &radius, nullptr);

    if (radius == 0)
        return DEFAULT_RECALL_RADIUS;

    return (f32) radius;
}

f32 mmWaypoints::CalcHeading(i32 from_idx, i32 to_idx)
{
    Vector3 delta = Positions[from_idx] - Positions[to_idx];
    return std::atan2(delta.x, delta.z) * -RAD_TO_DEG;
}

void mmWaypoints::ComputeGatePoints(i32 wp_idx, i32 pos_idx)
{
    mmWaypointObject* wp = Waypoints[wp_idx];
    f32 heading_rad = wp->Heading * DEG_TO_RAD;

    Vector2 left, right;
    CalculateGatePoints(Positions[pos_idx], heading_rad, wp->Radius, &left, &right);

    GatePointsLeft[wp_idx] = left;
    GatePointsRight[wp_idx] = right;
}

void mmWaypoints::ComputePrevGatePoints(i32 wp_idx, i32 pos_idx)
{
    if (wp_idx <= 1)
        return;

    i32 prev_idx = wp_idx - 1;
    mmWaypointObject* prev_wp = Waypoints[prev_idx];

    if (prev_wp->Heading == 0.0f)
        prev_wp->SetHeading(CalcHeading(pos_idx - 1, pos_idx));

    f32 heading_rad = prev_wp->Heading * DEG_TO_RAD;

    Vector2 left, right;
    CalculateGatePoints(Positions[pos_idx - 1], heading_rad, prev_wp->Radius, &left, &right);

    GatePointsLeft[prev_idx] = left;
    GatePointsRight[prev_idx] = right;
}

void mmWaypoints::UpdateCarBounds()
{
    // Project car's front and rear points into world space (X/Z plane only)
    Vector3 rear_local {0.0f, 0.0f, -Size.z};
    Vector3 front_local {0.0f, 0.0f, Size.z + 2.0f};

    asInertialCS* ics = Player->Car.GetICS();

    Vector3 rear_world = rear_local ^ ics->Matrix;
    Vector3 front_world = front_local ^ ics->Matrix;

    CarRearPos = {rear_world.x, rear_world.z};
    CarFrontPos = {front_world.x, front_world.z};
}

void mmWaypoints::HandleWaypointHit(i32 hit)
{
    i32 last_idx = PositionCount - 1;

    if (hit == last_idx && LastWaypoint == last_idx)
    {
        // Crossed the finish line - race complete
        mmWaypointObject* wp = Waypoints[hit];
        IdentMask = wp->Initialized ? IdentMask : (wp->IdentMask | IdentMask);
        Finished = true;
    }
    else if (hit > 0 && hit < last_idx)
    {
        // Cleared an intermediate checkpoint
        ClearWaypoint(hit);

        if (hit == CurrentWaypoint)
            GetNextWaypoint();

        if (LastWaypoint == last_idx)
        {
            // Just cleared the last intermediate - approaching finish
            LastWaypointSound->PlayOnce(-1.0f, -1.0f);
            Waypoints[CurrentWaypoint]->Activate();

            if (VoiceCommentary)
                VoiceCommentary->PlayFinalCheckPoint();
        }
    }
}