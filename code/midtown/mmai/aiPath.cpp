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

define_dummy_symbol(mmai_aiPath);

#include "aiPath.h"

#include "aiIntersection.h"



#include "aiVehicleOpponent.h"
#include "aiVehicleSpline.h"


Vector3* aiPath::CenterVertice(i32 index)
{
    if (index >= 0 && index < VertexCount)
        return &CenterVerts[index];

    Warningf("Returning a NULL CenterVertice vector.");
    Warningf("Bad Idx: %d, Path Id: %d", index, Id);
    return nullptr;
}

void aiPath::Dump()
{
    Displayf("Path %d, Vertex Count = %d, Num Lanes = %d", Id, VertexCount, NumLanes);

    Displayf("\tOncoming Path = %d, Intersections = %d %d", OncomingPath->Id, Sink->Id, Source->Id);

    Displayf("\tBlocked = %d, PedBlocked = %d, HasBridge = %d", IsBlocked, IsPedBlocked, HasBridge);
    Displayf("\tDivided = %d, Flat       = %d, Alley     = %d", IsDivided, IsFlat, IsAlley);

    for (i32 i = 0; i < NumLanes; ++i)
    {
        Displayf("\tLane: %d", i);

        i32 vertex_per_lane = VertexCount;

        Vector3& start = LaneVertices[i * vertex_per_lane + 1];
        Vector3& end = LaneVertices[i * vertex_per_lane + VertexCount - 2];

        Displayf("\t\tStart = %.2f, %.2f, %.2f", start.x, start.y, start.z);
        Displayf("\t\tEnd   = %.2f, %.2f, %.2f", end.x, end.y, end.z);
    }
}

Vector3* aiPath::VertXDir(i32 index)
{
    if (index >= 0 && index < VertexCount)
        return &VertXDirs[index];

    Warningf("Returning a NULL VertXDir vector.");
    Warningf("Bad Idx: %d, Path Id: %d", index, Id);
    return nullptr;
}

Vector3* aiPath::VertZDir(i32 index)
{
    if (index >= 0 && index < VertexCount)
        return &VertZDirs[index];

    Warningf("Returning a NULL VertZDir vector.");
    Warningf("Bad Idx: %d, Path Id: %d", index, Id);
    return nullptr;
}

void aiPath::Blocked(b32 blocked)
{
    IsBlocked = blocked || IsAlwaysBlocked;
}

b32 aiPath::RoadCapacity(aiVehicleSpline* spline, i32 lane)
{
    aiVehicleSpline* lane_vehicle = Vehicles[lane];

    if (!lane_vehicle)
        return true;

    f32 road_dist = lane_vehicle->RailSet.RoadDist;
    f32 back_bumper_dist = lane_vehicle->RailSet.BackBumperDist;

    for (aiVehicleSpline* ahead = spline->Next[spline->RailSet.TargetLane]; ahead != nullptr;
        ahead = ahead->Next[ahead->RailSet.TargetLane])
    {
        if (ahead->RailSet.CurLink->Id != spline->RailSet.CurLink->Id ||
            spline->RailSet.RoadDist >= ahead->RailSet.RoadDist)
        {
            break;
        }

        if (ahead->RailSet.NextLink->Id == spline->RailSet.NextLink->Id)
            back_bumper_dist = ahead->TotLength() + back_bumper_dist;
    }

    f32 total_length = spline->TotLength() + back_bumper_dist;

    return road_dist > total_length;
}



f32 aiPath::SubSectionLength(i32 start_index, i32 end_index)
{
    return SubSectionOffsets[end_index] - SubSectionOffsets[start_index];
}


void aiPath::SubSectionPt(Vector3& out, i32 lane, i32 subsection, f32 dist)
{
    i32 vertex_index = subsection + lane * VertexCount;
    Vector3& vertex = LaneVertices[vertex_index];
    Vector3& prev_vertex = LaneVertices[vertex_index - 1];

    f32 subsection_length = SubSectionLength(vertex_index - 1, vertex_index);
    f32 t = 1.0f - dist / subsection_length;

    out = prev_vertex + (vertex - prev_vertex) * t;
}






f32 aiPath::SubSectionDist(f32 road_dist, i32 lane)
{
    f32 lane_length = SubSectionLength(VertexCount * lane + 1, VertexCount * (lane + 1) - 1);

    if (road_dist > 0.0f)
    {
        if (road_dist >= lane_length)
            road_dist = lane_length;
    }
    else
    {
        road_dist = 0.0f;
    }

    if (VertexCount <= 1)
        return -1.0f;

    i32 lane_start = VertexCount * lane;
    i32 subsection = 1;
    f32 start_offset = SubSectionOffsets[lane_start + 1];
    for (i32 i = lane_start + 1; road_dist > SubSectionOffsets[i] - start_offset + 0.00001f; ++i)
    {
        ++subsection;
        if (subsection >= VertexCount)
            return -1.0f;
    }

    return road_dist - (SubSectionOffsets[lane_start + subsection - 1] - start_offset);
}