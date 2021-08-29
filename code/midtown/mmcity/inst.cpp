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

define_dummy_symbol(mmcity_inst);

#include "inst.h"

#include "agi/viewport.h"
#include "agiworld/meshset.h"
#include "arts7/sim.h"
#include "heap.h"
#include "mmcity/cullcity.h"
#include "mmcity/renderweb.h"

f32 mmInstance::LodTable[3 /*Inst Type*/][4 /*Terrain Quality*/][3 /*Lod Dist*/] {
    {
        // FACADES
        {200.0f, 150.0f, 100.0f},
        {350.0f, 250.0f, 150.0f},
        {700.0f, 500.0f, 300.0f},
        {700.0f, 500.0f, 300.0f},
    },
    {
        // BANGERS
        {100.0f, 50.0f, 20.0f},
        {150.0f, 85.0f, 35.0f},
        {250.0f, 160.0f, 60.0f},
        {250.0f, 200.0f, 150.0f},
    },
    {
        // UPPERS
        {9999.0f, 0.0f, 0.0f},
        {9999.0f, 100.0f, 0.0f},
        {9999.0f, 500.0f, 300.0f},
        {9999.0f, 600.0f, 400.0f},
    },
};

mmPhysEntity* mmInstance::GetEntity()
{
    return nullptr;
}

mmPhysEntity* mmInstance::AttachEntity()
{
    return nullptr;
}

void mmInstance::Impact(mmInstance* /*arg1*/, Vector3* /*arg2*/)
{}

void mmInstance::Detach()
{}

void ARTS_FASTCALL mmInstance::DrawShadow()
{}

void ARTS_FASTCALL mmInstance::DrawGlow()
{}

void ARTS_FASTCALL mmInstance::Relight()
{}

void mmInstance::Reset()
{}

i32 mmInstance::Init(char* /*arg1*/, Vector3& /*arg2*/, Vector3& /*arg3*/, i32 /*arg4*/, char* /*arg5*/)
{
    return 0;
}

// ?MatrixFromPoints@@YAXAAVMatrix34@@AAVVector3@@1M@Z
ARTS_IMPORT /*static*/ void MatrixFromPoints(Matrix34& arg1, Vector3& arg2, Vector3& arg3, f32 arg4);

void mmMatrixInstance::Hit(mmInstance* /*arg1*/)
{}

#ifdef ARTS_DEV_BUILD
void mmMatrixInstance::AddWidgets(Bank* /*arg1*/)
{}
#endif

void ARTS_FASTCALL mmStaticInstance::Relight()
{}

agiMeshSet* mmInstance::GetResidentMeshSet(i32 lod, i32 index, i32 variant)
{
    agiMeshSet* mesh = nullptr;

    if (MeshSetTableEntry* entry = GetMeshBase(index))
    {
        if (mesh = entry->Meshes[lod]; mesh && !mesh->IsFullyResident(variant))
        {
            if (lod)
            {
                if (agiMeshSet* low = entry->Meshes[lod - 1]; low && low->IsFullyResident(variant))
                    return low;
            }
        }
    }

    return mesh;
}

void* mmInstance::operator new(std::size_t size)
{
    return mmInstanceHeap.Allocate(size);
}

void mmInstance::operator delete(void* ptr)
{
    mmInstanceHeap.Free(ptr);
}

void mmBuildingInstance::Draw(i32 lod)
{
    enum
    {
        MESH_FACADE = 0,
        MESH_GRND = 1,
    };

    if (ARTSPTR->IsDebugDrawEnabled())
        return;

    Matrix34 world;
    Viewport()->SetWorld(ToMatrix(world));

    if (asRenderWeb::PassMask & RENDER_PASS_TERRAIN)
    {
        if (agiMeshSet* mesh = GetMeshSet(INST_LOD_HIGH, MESH_GRND))
            mesh->DrawLitEnv(DynamicLighter, CullCity()->ShadowMap, CullCity()->EnvMatrix, MESH_DRAW_CLIP);
    }

    if (asRenderWeb::PassMask & RENDER_PASS_BUILDINGS)
    {
        if (agiMeshSet* mesh = GetResidentMeshSet(std::max(lod, INST_LOD_LOW), MESH_FACADE))
            mesh->DrawLit(StaticLighter, MESH_DRAW_CLIP, nullptr);
    }
}
