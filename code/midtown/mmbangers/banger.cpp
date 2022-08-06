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

define_dummy_symbol(mmbangers_banger);

#include "banger.h"

#include "data.h"
#include "mmcity/cullcity.h"
#include "vector7/matrix34.h"

mmBangerInstance::mmBangerInstance()
{
    Flags |= INST_FLAG_COLLIDER | INST_FLAG_40;
}

mmBangerData* mmBangerInstance::GetData()
{
    return BangerDataMgr()->GetBangerData(BangerIndex);
}

mmUnhitBangerInstance::mmUnhitBangerInstance()
{
    Flags |= INST_FLAG_200;
}

b32 mmUnhitBangerInstance::Init(aconst char* name, Vector3& pos1, Vector3& pos2, i32 init_flags, aconst char* part)
{
    BangerIndex = static_cast<i16>(BangerDataMgr()->AddBangerDataEntry(name, part));
    mmBangerData* bng_data = GetData();

    Vector3 cg {};
    Vector3 glow_offset {};

    if (bng_data)
    {
        cg = bng_data->CG;
        glow_offset = bng_data->CG + bng_data->GlowOffset;
    }

    bool is_valid = false;

    InitMeshes(name, MESH_SET_UV | MESH_SET_NORMAL | MESH_SET_CPV, part, &cg);
    InitBreakables(name, part, bng_data);

    if (MeshIndex)
    {
        if (bng_data)
            bng_data->MeshIndex = MeshIndex;

        Matrix34 matrix;

        matrix.m0 = ~Vector3(pos2.x - pos1.x, 0.0f, pos2.z - pos1.z);
        matrix.m1 = Vector3(0.0f, 1.0f, 0.0f);
        matrix.m2 = Vector3(-matrix.m0.z, 0.0f, matrix.m0.x);
        matrix.m3 = pos1;

        matrix.m3 = cg ^ matrix;

        FromMatrix(matrix);

        is_valid = true;
    }

    if ((init_flags & INST_INIT_FLAG_GLOW) && bng_data && (bng_data->BillFlags & BANGER_BILL_FLAG_20))
    {
        AddMeshes(name, MESH_SET_UV | MESH_SET_CPV | MESH_SET_NO_BOUND, xconst("GLOW"), &glow_offset);

        Flags |= INST_FLAG_GLOW;
    }

    if (init_flags & INST_INIT_FLAG_TERRAIN)
    {
        Flags |= INST_FLAG_TERRAIN;
    }

    if (is_valid)
    {
        // TODO: Implement MultiRoomInstance

        CullCity()->ReparentObject(this);

        if (bng_data && !(bng_data->BillFlags & BANGER_BILL_FLAG_10))
            Flags |= INST_FLAG_2;
    }

    return is_valid;
}

#ifdef ARTS_DEV_BUILD
void mmBangerInstance::AddWidgets(Bank* /*arg1*/)
{}
#endif
