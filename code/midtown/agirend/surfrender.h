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

/*
    agirend:surfrender

    0x552D60 | public: __thiscall agiSurfRenderer::agiSurfRenderer(class agiRasterizer *) | ??0agiSurfRenderer@@QAE@PAVagiRasterizer@@@Z
    0x552DB0 | protected: virtual __thiscall agiSurfRenderer::~agiSurfRenderer(void) | ??1agiSurfRenderer@@MAE@XZ
    0x552DC0 | public: virtual void __thiscall agiSurfRenderer::BeginGroup(void) | ?BeginGroup@agiSurfRenderer@@UAEXXZ
    0x552E00 | private: static int __cdecl agiSurfRenderer::AddSurface(struct agiPatch *,int,int,int,int) | ?AddSurface@agiSurfRenderer@@CAHPAUagiPatch@@HHHH@Z
    0x5544F0 | public: virtual void __thiscall agiSurfRenderer::Draw(class DLP *,int) | ?Draw@agiSurfRenderer@@UAEXPAVDLP@@H@Z
    0x554890 | public: virtual void __thiscall agiSurfRenderer::DrawCard(class Vector3 &) | ?DrawCard@agiSurfRenderer@@UAEXAAVVector3@@@Z
    0x554D70 | public: virtual void __thiscall agiSurfRenderer::BeginDraw(int) | ?BeginDraw@agiSurfRenderer@@UAEXH@Z
    0x554DA0 | public: virtual void __thiscall agiSurfRenderer::Verts(union agiVtx *,int) | ?Verts@agiSurfRenderer@@UAEXPATagiVtx@@H@Z
    0x554E40 | public: virtual void __thiscall agiSurfRenderer::Points(union agiVtx *,int) | ?Points@agiSurfRenderer@@UAEXPATagiVtx@@H@Z
    0x554EB0 | public: virtual void __thiscall agiSurfRenderer::DrawTri(int,int,int) | ?DrawTri@agiSurfRenderer@@UAEXHHH@Z
    0x554EE0 | public: virtual void __thiscall agiSurfRenderer::DrawLine(int,int) | ?DrawLine@agiSurfRenderer@@UAEXHH@Z
    0x555180 | public: virtual void __thiscall agiSurfRenderer::DrawLabel(union agiVtx &,char *) | ?DrawLabel@agiSurfRenderer@@UAEXAATagiVtx@@PAD@Z
    0x5551F0 | public: virtual void __thiscall agiSurfRenderer::EndDraw(void) | ?EndDraw@agiSurfRenderer@@UAEXXZ
    0x555200 | protected: virtual void * __thiscall agiSurfRenderer::`scalar deleting destructor'(unsigned int) | ??_GagiSurfRenderer@@MAEPAXI@Z
    0x555200 | protected: virtual void * __thiscall agiSurfRenderer::`vector deleting destructor'(unsigned int) | ??_EagiSurfRenderer@@MAEPAXI@Z
    0x555230 | protected: virtual void * __thiscall agiRenderer::`vector deleting destructor'(unsigned int) | ??_EagiRenderer@@MAEPAXI@Z
    0x555230 | protected: virtual void * __thiscall agiRenderer::`scalar deleting destructor'(unsigned int) | ??_GagiRenderer@@MAEPAXI@Z
    0x6214A0 | const agiSurfRenderer::`vftable' | ??_7agiSurfRenderer@@6B@
    0x6214D0 | const agiRenderer::`vftable' | ??_7agiRenderer@@6B@
    0x656590 | int EarlyBackface | ?EarlyBackface@@3HA
    0x8BABF8 | protected: static int agiSurfRenderer::SurfaceCount | ?SurfaceCount@agiSurfRenderer@@1HA
    0x8BABFC | protected: static int agiSurfRenderer::VertexCount | ?VertexCount@agiSurfRenderer@@1HA
    0x8BAC00 | int NoClip | ?NoClip@@3HA
    0x8BAC08 | protected: static struct agiSurface * agiSurfRenderer::SurfaceHeap | ?SurfaceHeap@agiSurfRenderer@@1PAUagiSurface@@A
    0x8FAC14 | int ZTrick | ?ZTrick@@3HA
*/

#include "agi/render.h"

class agiLighter;
struct agiPatch;
class agiRasterizer;
struct agiSurface;

class agiSurfRenderer : public agiRenderer
{
public:
    // ??0agiSurfRenderer@@QAE@PAVagiRasterizer@@@Z
    ARTS_IMPORT agiSurfRenderer(agiRasterizer* arg1);

    // ?BeginDraw@agiSurfRenderer@@UAEXH@Z
    ARTS_EXPORT void BeginDraw(i32 flags) override;

    // ?BeginGroup@agiSurfRenderer@@UAEXXZ
    ARTS_EXPORT void BeginGroup() override;

    // ?Draw@agiSurfRenderer@@UAEXPAVDLP@@H@Z
    ARTS_IMPORT void Draw(DLP* arg1, i32 arg2) override;

    // ?DrawCard@agiSurfRenderer@@UAEXAAVVector3@@@Z
    ARTS_IMPORT void DrawCard(Vector3& arg1) override;

    // ?DrawLabel@agiSurfRenderer@@UAEXAATagiVtx@@PAD@Z
    ARTS_IMPORT void DrawLabel(agiVtx& arg1, char* arg2) override;

    // ?DrawLine@agiSurfRenderer@@UAEXHH@Z
    ARTS_IMPORT void DrawLine(i32 arg1, i32 arg2) override;

    // ?DrawTri@agiSurfRenderer@@UAEXHHH@Z
    ARTS_IMPORT void DrawTri(i32 arg1, i32 arg2, i32 arg3) override;

    // ?EndDraw@agiSurfRenderer@@UAEXXZ
    ARTS_IMPORT void EndDraw() override;

    // ?Points@agiSurfRenderer@@UAEXPATagiVtx@@H@Z
    ARTS_IMPORT void Points(agiVtx* arg1, i32 arg2) override;

    // ?Verts@agiSurfRenderer@@UAEXPATagiVtx@@H@Z
    ARTS_IMPORT void Verts(agiVtx* arg1, i32 arg2) override;

protected:
    // ??_EagiSurfRenderer@@MAEPAXI@Z
    // ??_GagiSurfRenderer@@MAEPAXI@Z
    // ??1agiSurfRenderer@@MAE@XZ
    ARTS_IMPORT ~agiSurfRenderer() override = default;

    // ?SurfaceCount@agiSurfRenderer@@1HA
    ARTS_IMPORT static i32 SurfaceCount;

    // ?SurfaceHeap@agiSurfRenderer@@1PAUagiSurface@@A
    ARTS_IMPORT static agiSurface SurfaceHeap[16384];

    // ?VertexCount@agiSurfRenderer@@1HA
    ARTS_IMPORT static i32 VertexCount;

private:
    // ?AddSurface@agiSurfRenderer@@CAHPAUagiPatch@@HHHH@Z
    ARTS_IMPORT static i32 AddSurface(agiPatch* arg1, i32 arg2, i32 arg3, i32 arg4, i32 arg5);

    agiRasterizer* rasterizer_ {};
    agiLighter* lighter_ {};
};

check_size(agiSurfRenderer, 0x14);

// ?EarlyBackface@@3HA
ARTS_IMPORT extern i32 EarlyBackface;

// ?NoClip@@3HA
ARTS_IMPORT extern i32 NoClip;

// ?ZTrick@@3HA
ARTS_IMPORT extern i32 ZTrick;

void UpdateZTrick();
