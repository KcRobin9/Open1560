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
    agirend:projvtx

    0x5524F0 | public: static void __cdecl agiProjVtx::InitMtx(class Matrix34 &,float,float,float,float) | ?InitMtx@agiProjVtx@@SAXAAVMatrix34@@MMMM@Z
    0x552630 | public: static void __cdecl agiProjVtx::SetOutcode(int) | ?SetOutcode@agiProjVtx@@SAXH@Z
    0x552710 | public: static int __cdecl agiProjVtx::Project(int,struct agiLitVtx *) | ?Project@agiProjVtx@@SAHHPAUagiLitVtx@@@Z
    0x552A20 | public: static void __cdecl agiProjVtx::ToScreen(int) | ?ToScreen@agiProjVtx@@SAXH@Z
    0x552BA0 | public: static void __cdecl agiProjVtx::Init(int,class agiLighter *,unsigned int) | ?Init@agiProjVtx@@SAXHPAVagiLighter@@I@Z
    0x656588 | float DepthOffset | ?DepthOffset@@3MA
    0x65658C | float DepthScale | ?DepthScale@@3MA
    0x7E02C8 | public: static float agiProjVtx::ProjZW | ?ProjZW@agiProjVtx@@2MA
    0x7E02CC | public: static int agiProjVtx::NoModelView | ?NoModelView@agiProjVtx@@2HA
    0x7E02D0 | public: static float agiProjVtx::OffsX | ?OffsX@agiProjVtx@@2MA
    0x7E02D8 | public: static union agiVtx * agiProjVtx::Vtx | ?Vtx@agiProjVtx@@2PATagiVtx@@A
    0x8B31D8 | public: static class Matrix34 agiProjVtx::M | ?M@agiProjVtx@@2VMatrix34@@A
    0x8B3208 | public: static float agiProjVtx::ProjYZ | ?ProjYZ@agiProjVtx@@2MA
    0x8B320C | int clip_pz | ?clip_pz@@3HA
    0x8B3210 | public: static float agiProjVtx::HalfHeight | ?HalfHeight@agiProjVtx@@2MA
    0x8B3214 | public: static float agiProjVtx::ProjX | ?ProjX@agiProjVtx@@2MA
    0x8B3220 | public: static struct agiAdjunctData * agiProjVtx::Adj | ?Adj@agiProjVtx@@2PAUagiAdjunctData@@A
    0x8B9B98 | public: static class agiLighter * agiProjVtx::LIGHTER | ?LIGHTER@agiProjVtx@@2PAVagiLighter@@A
    0x8B9BA0 | public: static class Matrix34 agiProjVtx::M2 | ?M2@agiProjVtx@@2VMatrix34@@A
    0x8B9BD0 | public: static float agiProjVtx::ProjXZ | ?ProjXZ@agiProjVtx@@2MA
    0x8B9BD4 | public: static float agiProjVtx::ScreenHeight | ?ScreenHeight@agiProjVtx@@2MA
    0x8B9BD8 | public: static int agiProjVtx::Orthographic | ?Orthographic@agiProjVtx@@2HA
    0x8B9BDC | public: static float agiProjVtx::ProjY | ?ProjY@agiProjVtx@@2MA
    0x8B9BE0 | public: static int agiProjVtx::NeedsClip | ?NeedsClip@agiProjVtx@@2HA
    0x8B9BE4 | public: static float agiProjVtx::ProjZZ | ?ProjZZ@agiProjVtx@@2MA
    0x8B9BE8 | public: static float agiProjVtx::OffsY | ?OffsY@agiProjVtx@@2MA
    0x8BABF0 | public: static float agiProjVtx::ScreenWidth | ?ScreenWidth@agiProjVtx@@2MA
    0x8BABF4 | public: static float agiProjVtx::HalfWidth | ?HalfWidth@agiProjVtx@@2MA
*/

struct agiAdjunctData;
class agiLighter;
struct agiLitVtx;
union agiVtx;

struct agiProjVtx
{
public:
    // ?Init@agiProjVtx@@SAXHPAVagiLighter@@I@Z
    ARTS_IMPORT static void Init(i32 arg1, agiLighter* arg2, u32 arg3);

    // ?InitMtx@agiProjVtx@@SAXAAVMatrix34@@MMMM@Z
    ARTS_IMPORT static void InitMtx(Matrix34& arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5);

    // ?Project@agiProjVtx@@SAHHPAUagiLitVtx@@@Z
    ARTS_IMPORT static i32 Project(i32 arg1, agiLitVtx* arg2);

    // ?SetOutcode@agiProjVtx@@SAXH@Z
    ARTS_IMPORT static void SetOutcode(i32 arg1);

    // ?ToScreen@agiProjVtx@@SAXH@Z
    ARTS_IMPORT static void ToScreen(i32 arg1);

    // ?Adj@agiProjVtx@@2PAUagiAdjunctData@@A
    ARTS_IMPORT static agiAdjunctData Adj[6968];

    // ?HalfHeight@agiProjVtx@@2MA
    ARTS_IMPORT static f32 HalfHeight;

    // ?HalfWidth@agiProjVtx@@2MA
    ARTS_IMPORT static f32 HalfWidth;

    // ?LIGHTER@agiProjVtx@@2PAVagiLighter@@A
    ARTS_IMPORT static agiLighter* LIGHTER;

    // ?M@agiProjVtx@@2VMatrix34@@A
    ARTS_IMPORT static Matrix34 M;

    // ?M2@agiProjVtx@@2VMatrix34@@A
    ARTS_IMPORT static Matrix34 M2;

    // ?NeedsClip@agiProjVtx@@2HA
    ARTS_IMPORT static i32 NeedsClip;

    // ?NoModelView@agiProjVtx@@2HA
    ARTS_IMPORT static i32 NoModelView;

    // ?OffsX@agiProjVtx@@2MA
    ARTS_IMPORT static f32 OffsX;

    // ?OffsY@agiProjVtx@@2MA
    ARTS_IMPORT static f32 OffsY;

    // ?Orthographic@agiProjVtx@@2HA
    ARTS_IMPORT static i32 Orthographic;

    // ?ProjX@agiProjVtx@@2MA
    ARTS_IMPORT static f32 ProjX;

    // ?ProjXZ@agiProjVtx@@2MA
    ARTS_IMPORT static f32 ProjXZ;

    // ?ProjY@agiProjVtx@@2MA
    ARTS_IMPORT static f32 ProjY;

    // ?ProjYZ@agiProjVtx@@2MA
    ARTS_IMPORT static f32 ProjYZ;

    // ?ProjZW@agiProjVtx@@2MA
    ARTS_IMPORT static f32 ProjZW;

    // ?ProjZZ@agiProjVtx@@2MA
    ARTS_IMPORT static f32 ProjZZ;

    // ?ScreenHeight@agiProjVtx@@2MA
    ARTS_IMPORT static f32 ScreenHeight;

    // ?ScreenWidth@agiProjVtx@@2MA
    ARTS_IMPORT static f32 ScreenWidth;

    // ?Vtx@agiProjVtx@@2PATagiVtx@@A
    ARTS_IMPORT static agiVtx Vtx[6968];
};

// ?DepthOffset@@3MA
ARTS_IMPORT extern f32 DepthOffset;

// ?DepthScale@@3MA
ARTS_IMPORT extern f32 DepthScale;

// ?clip_pz@@3HA
ARTS_IMPORT extern i32 clip_pz;
