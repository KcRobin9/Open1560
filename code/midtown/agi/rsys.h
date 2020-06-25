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
    agi:rsys

    0x557BE0 | public: __thiscall agiRasterizer::agiRasterizer(class agiPipeline *) | ??0agiRasterizer@@QAE@PAVagiPipeline@@@Z
    0x557CE0 | public: void __thiscall agiRendStateStruct::Reset(void) | ?Reset@agiRendStateStruct@@QAEXXZ
    0x557D10 | public: virtual __thiscall agiRasterizer::~agiRasterizer(void) | ??1agiRasterizer@@UAE@XZ
    0x557D20 | public: virtual void __thiscall agiRasterizer::BeginGroup(void) | ?BeginGroup@agiRasterizer@@UAEXXZ
    0x557D30 | public: virtual void __thiscall agiRasterizer::EndGroup(void) | ?EndGroup@agiRasterizer@@UAEXXZ
    0x557D40 | public: virtual void __thiscall agiRasterizer::Quad(int,int,int,int) | ?Quad@agiRasterizer@@UAEXHHHH@Z
    0x557D70 | public: virtual void __thiscall agiRasterizer::Poly(int *,int) | ?Poly@agiRasterizer@@UAEXPAHH@Z
    0x557DB0 | public: virtual void __thiscall agiRasterizer::Mesh2(struct agiScreenVtx2 *,int,unsigned short *,int) | ?Mesh2@agiRasterizer@@UAEXPAUagiScreenVtx2@@HPAGH@Z
    0x557DD0 | public: virtual void __thiscall agiRasterizer::LineList(enum agiVtxType,union agiVtx *,int) | ?LineList@agiRasterizer@@UAEXW4agiVtxType@@PATagiVtx@@H@Z
    0x557E40 | public: virtual void * __thiscall agiRasterizer::`vector deleting destructor'(unsigned int) | ??_EagiRasterizer@@UAEPAXI@Z
    0x557E40 | public: virtual void * __thiscall agiRasterizer::`scalar deleting destructor'(unsigned int) | ??_GagiRasterizer@@UAEPAXI@Z
    0x621660 | const agiRasterizer::`vftable' | ??_7agiRasterizer@@6B@
    0x656BD0 | struct agiRenderOpts ROPTS | ?ROPTS@@3UagiRenderOpts@@A
    0x8FF050 | struct agiRendStateStruct agiLastState | ?agiLastState@@3UagiRendStateStruct@@A
    0x8FF090 | class agiRendState agiCurState | ?agiCurState@@3VagiRendState@@A
    0x8FF0D0 | class agiRasterizer * RAST | ?RAST@@3PAVagiRasterizer@@A
*/

#include "refresh.h"

class agiRasterizer : public agiRefreshable
{
    // const agiRasterizer::`vftable' @ 0x621660

public:
    // 0x557BE0 | ??0agiRasterizer@@QAE@PAVagiPipeline@@@Z
    ARTS_IMPORT agiRasterizer(class agiPipeline* arg1);

    // 0x557E40 | ??_GagiRasterizer@@UAEPAXI@Z
    // 0x557D10 | ??1agiRasterizer@@UAE@XZ
    ARTS_IMPORT ~agiRasterizer() override;

    // 0x557D20 | ?BeginGroup@agiRasterizer@@UAEXXZ
    ARTS_IMPORT virtual void BeginGroup();

    // 0x557D30 | ?EndGroup@agiRasterizer@@UAEXXZ
    ARTS_IMPORT virtual void EndGroup();

    virtual void Verts(enum agiVtxType arg1, union agiVtx* arg2, i32 arg3) = 0;

    virtual void Points(enum agiVtxType arg1, union agiVtx* arg2, i32 arg3) = 0;

    virtual void SetVertCount(i32 arg1) = 0;

    virtual void Triangle(i32 arg1, i32 arg2, i32 arg3) = 0;

    // 0x557D40 | ?Quad@agiRasterizer@@UAEXHHHH@Z
    ARTS_IMPORT virtual void Quad(i32 arg1, i32 arg2, i32 arg3, i32 arg4);

    // 0x557D70 | ?Poly@agiRasterizer@@UAEXPAHH@Z
    ARTS_IMPORT virtual void Poly(i32* arg1, i32 arg2);

    virtual void Line(i32 arg1, i32 arg2) = 0;

    virtual void Card(i32 arg1, i32 arg2) = 0;

    virtual void Mesh(enum agiVtxType arg1, union agiVtx* arg2, i32 arg3, u16* arg4, i32 arg5) = 0;

    // 0x557DB0 | ?Mesh2@agiRasterizer@@UAEXPAUagiScreenVtx2@@HPAGH@Z
    ARTS_IMPORT virtual void Mesh2(struct agiScreenVtx2* arg1, i32 arg2, u16* arg3, i32 arg4);

    // 0x557DD0 | ?LineList@agiRasterizer@@UAEXW4agiVtxType@@PATagiVtx@@H@Z
    ARTS_IMPORT virtual void LineList(enum agiVtxType arg1, union agiVtx* arg2, i32 arg3);
};

check_size(agiRasterizer, 0x0);

struct agiRendStateStruct
{
public:
    // 0x557CE0 | ?Reset@agiRendStateStruct@@QAEXXZ
    ARTS_IMPORT void Reset();
};

check_size(agiRendStateStruct, 0x0);

// 0x8FF0D0 | ?RAST@@3PAVagiRasterizer@@A
ARTS_IMPORT extern class agiRasterizer* RAST;

// 0x656BD0 | ?ROPTS@@3UagiRenderOpts@@A
ARTS_IMPORT extern struct agiRenderOpts ROPTS;

// 0x8FF090 | ?agiCurState@@3VagiRendState@@A
ARTS_IMPORT extern class agiRendState agiCurState;

// 0x8FF050 | ?agiLastState@@3UagiRendStateStruct@@A
ARTS_IMPORT extern struct agiRendStateStruct agiLastState;
