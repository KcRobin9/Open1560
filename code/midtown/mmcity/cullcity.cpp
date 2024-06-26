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

define_dummy_symbol(mmcity_cullcity);

#include "cullcity.h"

#include "agi/rsys.h"
#include "agisw/swrend.h"
#include "agiworld/meshset.h"
#include "agiworld/quality.h"

#ifdef ARTS_DEV_BUILD
void mmRunwayLight::AddWidgets(Bank* /*arg1*/)
{}
#endif

void ARTS_FASTCALL mmRunwayLight::FromMatrix(const Matrix34& /*arg1*/)
{}

// ?fix_fill1@@YAXXZ
ARTS_IMPORT /*static*/ void fix_fill1();

// ?fix_fill2@@YAXXZ
ARTS_IMPORT /*static*/ void fix_fill2();

// ?fix_sun@@YAXXZ
ARTS_IMPORT /*static*/ void fix_sun();

// ?parseARGB@@YAXAAI@Z
ARTS_IMPORT /*static*/ void parseARGB(u32& arg1);

// ?parseFloat@@YAXAAM@Z
ARTS_IMPORT /*static*/ void parseFloat(f32& arg1);

// ?parseRGB@@YAXAAI@Z
ARTS_IMPORT /*static*/ void parseRGB(u32& arg1);

// ?parseVector3@@YAXAAVVector3@@@Z
ARTS_IMPORT /*static*/ void parseVector3(Vector3& arg1);

void mmCullCity::Cull()
{
    if (FogEnd == 0.0f || agiCurState.GetDrawMode() == agiDrawDepth)
    {
        agiCurState.SetFogMode(agiFogMode::None);
        agiMeshSet::SetFog(0.0, 0);
    }
    else
    {
        agiCurState.SetFogMode(UsePixelFog ? agiFogMode::Pixel : agiFogMode::Vertex);
        agiCurState.SetFogColor(SkyColor | swIsInterlaced);

        if (!agiCurState.GetSoftwareRendering())
            FogEnd = std::min(FogEnd, agiRQ.FarClip);

        if (UsePixelFog)
        {
            agiMeshSet::SetFog(0.0f, 0);
            agiCurState.SetFogStart(1.0f);
            agiCurState.SetFogEnd(FogEnd);
        }
        else if (UseFogEnd2)
        {
            agiMeshSet::SetFog(FogEnd2, UseFogEnd2);
        }
        else
        {
            agiMeshSet::SetFog(FogEnd, 0);
        }
    }
}
