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
#include "mmcity/positions.h"
#include "mmcityinfo/state.h"
#include "vector7/matrix34.h"

// Conversion constants
static constexpr f32 RAD_TO_DEG = 57.295776f;   // 180 / PI
static constexpr f32 DEG_TO_RAD = 0.017453292f; // PI / 180

// Maximum checkpoints for Blitz mode
static constexpr i32 MAX_BLITZ_CHECKPOINTS = 6;

// Default waypoint radius values
static constexpr f32 DEFAULT_RADIUS_NORMAL = 7.5f;
static constexpr f32 DEFAULT_RADIUS_BLITZ_EVENT_8 = 7.0f;

void mmWaypoints::Cull()
{}

void mmWaypoints::AllocateWaypointArrays(i32 count)
{
    Positions = new Vector3[count];
    Waypoints = new mmWaypointObject*[count];
    GatePointsLeft = new Vector2[count];
    GatePointsRight = new Vector2[count];
}

void mmWaypoints::SetStartPosition(i32 pos_idx)
{
    Vector4& pos = GetPositionVector4(pos_idx);
    StartPos.x = pos.x;
    StartPos.y = pos.y;
    StartPos.z = pos.z;
}

mmWaypointObject* mmWaypoints::CreateWaypointObject(
    Vector4& position, const char* type_name, i32 wp_idx, f32 radius, f32 default_rad)
{
    i32 ident_mask = 1 << wp_idx;
    i32 type = 3;

    return new mmWaypointObject(position, const_cast<char*>(type_name), ident_mask, radius, type, default_rad);
}

f32 mmWaypoints::CalculateHeadingBetweenPositions(i32 from_idx, i32 to_idx)
{
    f32 dx = Positions[from_idx].x - Positions[to_idx].x;
    f32 dz = Positions[from_idx].z - Positions[to_idx].z;

    return std::atan2(dx, dz) * -RAD_TO_DEG;
}

void mmWaypoints::SetupGatePointsForWaypoint(i32 wp_idx, i32 pos_idx)
{
    mmWaypointObject* wp = Waypoints[wp_idx];
    f32 heading_rad = wp->Heading * DEG_TO_RAD;

    Vector2 left_point, right_point;
    CalculateGatePoints(Positions[pos_idx], heading_rad, wp->Radius, &left_point, &right_point);

    GatePointsLeft[wp_idx] = left_point;
    GatePointsRight[wp_idx] = right_point;
}

void mmWaypoints::SetupGatePointsForPreviousWaypoint(i32 wp_idx, i32 pos_idx)
{
    if (wp_idx <= 1)
        return;

    i32 prev_idx = wp_idx - 1;
    mmWaypointObject* prev_wp = Waypoints[prev_idx];

    // Auto-calculate heading if not set
    if (prev_wp->Heading == 0.0f)
    {
        f32 heading = CalculateHeadingBetweenPositions(pos_idx - 1, pos_idx);
        prev_wp->SetHeading(heading);
    }

    f32 heading_rad = prev_wp->Heading * DEG_TO_RAD;

    Vector2 left_point, right_point;
    CalculateGatePoints(Positions[pos_idx - 1], heading_rad, prev_wp->Radius, &left_point, &right_point);

    GatePointsLeft[prev_idx] = left_point;
    GatePointsRight[prev_idx] = right_point;
}

void mmWaypoints::LoadBlitzWaypoints(i32 reverse)
{
    // Validate checkpoint count for Blitz mode
    if (PositionCount > MAX_BLITZ_CHECKPOINTS)
    {
        Errorf("Blitz checkpoint count greater than 6!  Fred lied to me.");
        PositionCount = MAX_BLITZ_CHECKPOINTS;
    }

    // Clamp NumLaps to valid range
    if (NumLaps > PositionCount || NumLaps <= 0)
        NumLaps = PositionCount;

    i32 wp_count = NumLaps + 1;
    AllocateWaypointArrays(wp_count);

    // Set starting position
    i32 start_idx = reverse ? NumLaps : 0;
    SetStartPosition(start_idx);

    i32 wp_idx = 0;
    i32 pos_idx = 0;

    for (i32 csv_idx = 0; csv_idx < PositionCount; ++csv_idx)
    {
        // Skip waypoints that should be removed (based on Blitz rules)
        if (BlitzRemove(csv_idx))
            continue;

        // Get position from CSV (handle reverse direction)
        i32 read_idx = reverse ? (PositionCount - csv_idx - 1) : csv_idx;
        Vector4 pos = GetPositionVector4(read_idx);

        // Store position
        Positions[pos_idx].x = pos.x;
        Positions[pos_idx].y = pos.y;
        Positions[pos_idx].z = pos.z;

        // Get waypoint radius from position data
        Matrix34 matrix;
        i32 radius = 0;
        i32 recall_idx = reverse ? (PositionCount - csv_idx - 1) : csv_idx;
        RecallPosition(recall_idx, &matrix, &radius, nullptr);

        if (radius == 0)
            radius = 15;

        f32 wp_rad = static_cast<f32>(radius);

        // Create waypoint object
        mmWaypointObject* wp = nullptr;
        bool is_last_wp = (csv_idx == PositionCount - 1);

        if (is_last_wp)
        {
            // Last waypoint is the finish
            f32 default_rad = (MMSTATE.EventId == 8) ? DEFAULT_RADIUS_BLITZ_EVENT_8 : DEFAULT_RADIUS_NORMAL;
            wp = CreateWaypointObject(pos, "pt_finish", wp_idx, wp_rad, default_rad);
        }
        else
        {
            // Intermediate checkpoint
            wp = CreateWaypointObject(pos, "pt_check", wp_idx, wp_rad, DEFAULT_RADIUS_NORMAL);
        }

        Waypoints[wp_idx] = wp;

        // Setup gate points for previous waypoint
        SetupGatePointsForPreviousWaypoint(wp_idx, pos_idx);

        // Setup gate points for last waypoint
        if (is_last_wp)
        {
            SetupGatePointsForWaypoint(wp_idx, pos_idx);
        }

        // Add waypoint as child node
        AddChild(wp);

        Displayf("LoadCSV: Blitz Vert %d = %f, %f, %f", wp_idx, Positions[pos_idx].x, Positions[pos_idx].y,
            Positions[pos_idx].z);

        ++wp_idx;
        ++pos_idx;
    }

    // Update position count to actual waypoint count
    PositionCount = NumLaps + 1;
}

void mmWaypoints::LoadCheckpointWaypoints(i32 reverse)
{
    AllocateWaypointArrays(PositionCount);

    // Set starting position
    i32 start_idx = reverse ? (PositionCount - 1) : 0;
    SetStartPosition(start_idx);

    for (i32 i = 0; i < PositionCount; ++i)
    {
        // Get position from CSV (handle reverse direction)
        i32 read_idx = reverse ? (PositionCount - i - 1) : i;
        Vector4 pos = GetPositionVector4(read_idx);

        // Store position
        Positions[i].x = pos.x;
        Positions[i].y = pos.y;
        Positions[i].z = pos.z;

        // Get waypoint radius from position data
        Matrix34 matrix;
        i32 radius = 0;
        RecallPosition(read_idx, &matrix, &radius, nullptr);

        if (radius == 0)
            radius = 15;

        f32 wp_rad = static_cast<f32>(radius);

        // Create waypoint object
        mmWaypointObject* wp = nullptr;
        bool is_last_wp = (i == PositionCount - 1);

        if (is_last_wp)
        {
            // Last waypoint is the finish
            wp = CreateWaypointObject(pos, "pt_finish", i, wp_rad, DEFAULT_RADIUS_NORMAL);
        }
        else
        {
            // Intermediate checkpoint
            wp = CreateWaypointObject(pos, "pt_check", i, wp_rad, DEFAULT_RADIUS_NORMAL);
        }

        Waypoints[i] = wp;

        // Setup gate points for previous waypoint
        SetupGatePointsForPreviousWaypoint(i, i);

        // Setup gate points for last waypoint
        if (is_last_wp)
        {
            SetupGatePointsForWaypoint(i, i);
        }

        // Add waypoint as child node
        AddChild(wp);

        Displayf("LoadCSV: Waypoint Vert = %f, %f, %f", Positions[i].x, Positions[i].y, Positions[i].z);
    }
}

void mmWaypoints::LoadCircuitWaypoints(i32 reverse)
{
    AllocateWaypointArrays(PositionCount);

    // Set starting position (always from position 0 for circuit)
    SetStartPosition(0);

    for (i32 i = 0; i < PositionCount; ++i)
    {
        // Get position from CSV (handle reverse direction)
        // Note: For circuit races, reverse uses (count - i) not (count - i - 1)
        i32 read_idx = reverse ? (PositionCount - i) : i;
        Vector4 pos = GetPositionVector4(read_idx);

        // Store position
        Positions[i].x = pos.x;
        Positions[i].y = pos.y;
        Positions[i].z = pos.z;

        // Get waypoint radius from position data
        Matrix34 matrix;
        i32 radius = 0;
        i32 recall_idx = reverse ? (PositionCount - i) : i;
        RecallPosition(recall_idx, &matrix, &radius, nullptr);

        if (radius == 0)
            radius = 15;

        f32 wp_rad = static_cast<f32>(radius);

        // Create waypoint object
        mmWaypointObject* wp = nullptr;

        if (i == 0)
        {
            // First waypoint is the start/finish line for circuit races
            wp = CreateWaypointObject(pos, "pt_finish", i, wp_rad, DEFAULT_RADIUS_NORMAL);
        }
        else
        {
            // Intermediate checkpoint
            wp = CreateWaypointObject(pos, "pt_check", i, wp_rad, DEFAULT_RADIUS_NORMAL);
        }

        Waypoints[i] = wp;

        // Setup gate points for first waypoint (start/finish)
        if (i == 0)
        {
            SetupGatePointsForWaypoint(0, 0);
        }

        // Setup gate points for previous waypoint
        SetupGatePointsForPreviousWaypoint(i, i);

        // Setup gate points for last waypoint
        if (i == PositionCount - 1)
        {
            // Auto-calculate heading pointing back to start if not set
            if (pos.w == 0.0f)
            {
                f32 heading = CalculateHeadingBetweenPositions(i, 0);
                Waypoints[i]->SetHeading(heading);
            }

            f32 heading_rad = Waypoints[i]->Heading * DEG_TO_RAD;

            Vector2 left_point, right_point;
            CalculateGatePoints(Positions[i], heading_rad, wp_rad, &left_point, &right_point);

            GatePointsLeft[i] = left_point;
            GatePointsRight[i] = right_point;
        }

        // Add waypoint as child node
        AddChild(wp);

        Displayf("LoadCSV: Waypoint Vert = %f, %f, %f", Positions[i].x, Positions[i].y, Positions[i].z);
    }
}

i32 mmWaypoints::LoadCSV(char* race_name, i32 reverse)
{
    // Build path to waypoints CSV file
    char path[128];
    arts_sprintf(path, "race\\%swaypoints.csv", race_name);

    // Initialize and load position data from CSV
    InitPositions();
    LoadPositions(path);

    PositionCount = GetPositionCount();

    if (PositionCount == 0)
        return 0;

    // Load waypoints based on race type (RaceType stores mmGameMode value)
    switch (static_cast<mmGameMode>(RaceType))
    {
        case mmGameMode::Checkpoint: LoadCheckpointWaypoints(reverse); break;
        case mmGameMode::Circuit: LoadCircuitWaypoints(reverse); break;
        case mmGameMode::Blitz: LoadBlitzWaypoints(reverse); break;
        default: break;
    }

    // Store starting angle from first waypoint
    if (Waypoints && Waypoints[0])
    {
        StartAngle = Waypoints[0]->Heading;
    }

    // Clean up position system
    InitPositions();

    return 1;
}