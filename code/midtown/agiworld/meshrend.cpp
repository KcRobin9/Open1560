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

define_dummy_symbol(agiworld_meshrend);

#include "meshrend.h"

#include "agi/pipeline.h"
#include "agi/rsys.h"
#include "agi/viewport.h"
#include "agisw/swrend.h"
#include "agiworld/packnorm.h"
#include "agiworld/quality.h"
#include "agiworld/texsort.h"
#include "data7/b2f.h"
#include "data7/utimer.h"
#include "dyna7/gfx.h"
#include "memory/alloca.h"
#include "pcwindis/setupdata.h"
#include "vector7/matrix34.h"
#include "vector7/matrix44.h"

// #ifdef ARTS_ENABLE_KNI
// #    define CLIP_ALL_TO_SCREEN
// #endif

f32 agiMeshSet::DepthOffset = 0.5f;
f32 agiMeshSet::DepthScale = 0.5f;

i32 agiMeshSet::EyePlaneCount = 0;
i32 agiMeshSet::EyePlanesHit = 0;
Vector3 agiMeshSet::EyePos {};

b32 agiMeshSet::FlipX = false;
f32 agiMeshSet::FogValue = 0.0f;
b32 agiMeshSet::MirrorMode = false;
b32 agiMeshSet::AllowEyeBackfacing = false;

Vector3 agiMeshSet::LocPos {};
Matrix34 agiMeshSet::M {};

f32 agiMeshSet::HalfHeight = 0.0f;
f32 agiMeshSet::HalfWidth = 0.0f;
f32 agiMeshSet::MaxX = 0.0f;
f32 agiMeshSet::MaxY = 0.0f;
f32 agiMeshSet::MinX = 0.0f;
f32 agiMeshSet::MinY = 0.0f;
f32 agiMeshSet::OffsX = 0.0f;
f32 agiMeshSet::OffsY = 0.0f;
f32 agiMeshSet::ProjZW = 0.0f;
f32 agiMeshSet::ProjZZ = 0.0f;

u32 agiMeshSet::MtxSerial = 0;
u32 agiMeshSet::ViewSerial = 0;

alignas(64) u8 agiMeshSet::codes[16384];
alignas(64) i16 agiMeshSet::firstFacet[256];
alignas(64) u8 agiMeshSet::fogout[16384];
alignas(64) i16 agiMeshSet::indexCounts[256];
alignas(64) i16 agiMeshSet::nextFacet[16384];
alignas(64) Vector4 agiMeshSet::out[16384];
alignas(64) i16 agiMeshSet::vertCounts[256];

struct CV
{
    f32 x, y, z, w; // screen position
    f32 map[3];     // source vertex interpolation
    u8 fog;         // vertex fog
    u8 idx[3];      // source vertex indices

    CV(const Vector4& pos, f32 map_0, f32 map_1, f32 map_2)
        : x(pos.x)
        , y(pos.y)
        , z(pos.z)
        , w(pos.w)
        , map {map_0, map_1, map_2}
    {}

    CV(const Vector2& pos, f32 z, f32 w, f32 map_0, f32 map_1, f32 map_2, i32 idx_0, i32 idx_1, i32 idx_2)
        : x(pos.x)
        , y(pos.y)
        , z(z)
        , w(w)
        , map {map_0, map_1, map_2}
        , idx {static_cast<u8>(idx_0), static_cast<u8>(idx_1), static_cast<u8>(idx_2)}
    {}
};

check_size(CV, 0x20);

struct CT
{
    u32 Index;
    u32 Count;
    u32 Tri[3];
    CT* Next;
};

check_size(CT, 0x18);

static u32 ClippedVertCount = 0;
static u32 ClippedTriCount = 0;
ARTS_IMPORT extern CV ClippedVerts[2048];
ARTS_IMPORT extern CT ClippedTris[512];
ARTS_IMPORT extern CT* ClippedTextures[256];
static bool OnlyZClip = false;
ARTS_IMPORT extern Matrix44 ShadowMatrix;

u32 ClipMask = MESH_CLIP_ANY;

void SetClipMode(b32 mask_only_z)
{
    ClipMask = mask_only_z ? (MESH_CLIP_NZ | MESH_CLIP_PZ) : MESH_CLIP_ANY;
    OnlyZClip = true;
}

// ?ClipNX@@YIXAAUCV@@0@Z
ARTS_IMPORT /*static*/ void ARTS_FASTCALL ClipNX(CV& arg1, CV& arg2);

// ?ClipNX@@YAHPAUCV@@0H@Z
ARTS_IMPORT /*static*/ i32 ClipNX(CV* arg1, CV* arg2, i32 arg3);

// ?ClipNY@@YIXAAUCV@@0@Z
ARTS_IMPORT /*static*/ void ARTS_FASTCALL ClipNY(CV& arg1, CV& arg2);

// ?ClipNY@@YAHPAUCV@@0H@Z
ARTS_IMPORT /*static*/ i32 ClipNY(CV* arg1, CV* arg2, i32 arg3);

// ?ClipNZ@@YIXAAUCV@@0@Z
ARTS_IMPORT /*static*/ void ARTS_FASTCALL ClipNZ(CV& arg1, CV& arg2);

// ?ClipNZ@@YAHPAUCV@@0H@Z
ARTS_IMPORT /*static*/ i32 ClipNZ(CV* arg1, CV* arg2, i32 arg3);

// ?ClipPX@@YIXAAUCV@@0@Z
ARTS_IMPORT /*static*/ void ARTS_FASTCALL ClipPX(CV& arg1, CV& arg2);

// ?ClipPX@@YAHPAUCV@@0H@Z
ARTS_IMPORT /*static*/ i32 ClipPX(CV* arg1, CV* arg2, i32 arg3);

// ?ClipPY@@YIXAAUCV@@0@Z
ARTS_IMPORT /*static*/ void ARTS_FASTCALL ClipPY(CV& arg1, CV& arg2);

// ?ClipPY@@YAHPAUCV@@0H@Z
ARTS_IMPORT /*static*/ i32 ClipPY(CV* arg1, CV* arg2, i32 arg3);

// ?ClipPZ@@YIXAAUCV@@0@Z
ARTS_IMPORT /*static*/ void ARTS_FASTCALL ClipPZ(CV& arg1, CV& arg2);

// ?ClipPZ@@YAHPAUCV@@0H@Z
ARTS_IMPORT /*static*/ i32 ClipPZ(CV* arg1, CV* arg2, i32 arg3);

// ?FullClip@@YAHPAUCV@@0H@Z
ARTS_EXPORT /*static*/ i32 FullClip(CV* ARTS_RESTRICT output, CV* ARTS_RESTRICT input, i32 count)
{
    if (count = ClipNZ(output, input, count); count == 0)
        return 0;

    if (count = ClipNX(input, output, count); count == 0)
        return 0;

    if (count = ClipPX(output, input, count); count == 0)
        return 0;

    if (count = ClipNY(input, output, count); count == 0)
        return 0;

    if (count = ClipPY(output, input, count); count == 0)
        return 0;

    if (count = ClipPZ(input, output, count); count == 0)
        return 0;

    for (i32 i = 0; i < count; ++i)
        output[i] = input[i];

    return count;
}

// ?ZClipOnly@@YAHPAUCV@@0H@Z
ARTS_EXPORT /*static*/ i32 ZClipOnly(CV* ARTS_RESTRICT output, CV* ARTS_RESTRICT input, i32 count)
{
    if (count = ClipNZ(output, input, count); count == 0)
        return 0;

    if (count = ClipPZ(input, output, count); count == 0)
        return 0;

    // NOTE: Original code did not copy the verts
    // Without the copy, there are artifacts, since the last clip wrote to the input
    for (i32 i = 0; i < count; ++i)
        output[i] = input[i];

    return count;
}

static inline u8 CalculateFog(f32 w, f32 fog)
{
    return ~FloatToByte(std::min<f32>(w * fog, 255.0f));
}

#define ARTS_TRANSFORM_FOG fogout[i] = CalculateFog(output[i].w, FogValue)

#define ARTS_TRANSFORM_CODE                                                                    \
    f32 w_abs = std::abs(output[i].w);                                                         \
    u8 clip_code = ((((w_abs - std::abs(output[i].x)) < 0.0f) << ((output[i].x < 0.0f) + 0)) | \
        (((w_abs - std::abs(output[i].y)) < 0.0f) << ((output[i].y < 0.0f) + 2)) |             \
        (((w_abs - std::abs(output[i].z)) < 0.0f) << ((output[i].z < 0.0f) + 4)));             \
    clip_any |= clip_code;                                                                     \
    clip_all &= clip_code;                                                                     \
    out_codes[i] = clip_code;

#ifndef ARTS_ENABLE_KNI
void agiMeshSet::ToScreen(u8* ARTS_RESTRICT in_codes, Vector4* ARTS_RESTRICT verts, i32 count)
{
    ARTS_UTIMED(agiInvertTimer);

    for (i32 i = 0; i < count; ++i)
    {
        if (!(in_codes[i] & MESH_CLIP_SCREEN))
            continue;

        Vector4& vert = verts[i];

        f32 const inv_w = 1.0f / vert.w;

        vert.x = (vert.x * inv_w * HalfWidth) + OffsX;
        vert.y = (vert.y * inv_w * HalfHeight) + OffsY;
        vert.z = (vert.z * inv_w * DepthScale) + DepthOffset;
        vert.w = inv_w;

        ClampToScreen(vert);
    }
}

#    define ARTS_TRANSFORM_DOT                                                                  \
        output[i].x = M.m0.x * input[i].x + M.m1.x * input[i].y + M.m2.x * input[i].z + M.m3.x; \
        output[i].y = M.m0.y * input[i].x + M.m1.y * input[i].y + M.m2.y * input[i].z + M.m3.y; \
        output[i].w = M.m0.z * input[i].x + M.m1.z * input[i].y + M.m2.z * input[i].z + M.m3.z; \
        output[i].z = output[i].w * ProjZZ + ProjZW

void agiMeshSet::Transform(Vector4* ARTS_RESTRICT output, Vector3* ARTS_RESTRICT input, i32 count)
{
    STATS.VertsXfrm += count;

    if (FogValue == 0.0f)
    {
        for (i32 i = 0; i < count; ++i)
        {
            ARTS_TRANSFORM_DOT;
        }
    }
    else
    {
        for (i32 i = 0; i < count; ++i)
        {
            ARTS_TRANSFORM_DOT;
            ARTS_TRANSFORM_FOG;
        }
    }
}

u32 agiMeshSet::TransformOutcode(
    u8* ARTS_RESTRICT out_codes, Vector4* ARTS_RESTRICT output, Vector3* ARTS_RESTRICT input, i32 count)
{
    STATS.VertsOut += count;
    STATS.VertsXfrm += count;

    u8 clip_any = 0;
    u8 clip_all = 0xFF;

    if (FogValue == 0.0f)
    {
        for (i32 i = 0; i < count; ++i)
        {
            ARTS_TRANSFORM_DOT;
            ARTS_TRANSFORM_CODE;
        }
    }
    else
    {
        for (i32 i = 0; i < count; ++i)
        {
            ARTS_TRANSFORM_DOT;
            ARTS_TRANSFORM_FOG;
            ARTS_TRANSFORM_CODE;
        }
    }

    return clip_any | (clip_all << 8);
}

void agiBlendColors(u32* ARTS_RESTRICT shaded, u32* ARTS_RESTRICT colors, i32 count, u32 color)
{
    if (count)
    {
        const u32 mul_b = (color & 0xFF) * 0x8081;
        const u32 mul_g = ((color >> 8) & 0xFF) * 0x8081;
        const u32 mul_r = ((color >> 16) & 0xFF) * 0x8081;
        const u32 mul_a = (color >> 24) * 0x8081;

        for (i32 i = 0; i < count; ++i)
        {
            const u32 input = colors[i];

            const u8 b = static_cast<u8>((mul_b * (input & 0xFF)) >> 23);
            const u8 g = static_cast<u8>((mul_g * ((input >> 8) & 0xFF)) >> 23);
            const u8 r = static_cast<u8>((mul_r * ((input >> 16) & 0xFF)) >> 23);
            const u8 a = static_cast<u8>((mul_a * (input >> 24)) >> 23);

            shaded[i] = (a << 24) | (r << 16) | (g << 8) | b;
        }
    }
}
#endif

#define ARTS_SHADOW_TRANSFORM_DOT                                                                                    \
    output[i].x = ShadowMatrix.m0.x * input[i].x + ShadowMatrix.m1.x * input[i].y + ShadowMatrix.m2.x * input[i].z + \
        ShadowMatrix.m3.x;                                                                                           \
    output[i].y = ShadowMatrix.m0.y * input[i].x + ShadowMatrix.m1.y * input[i].y + ShadowMatrix.m2.y * input[i].z + \
        ShadowMatrix.m3.y;                                                                                           \
    output[i].z = ShadowMatrix.m0.z * input[i].x + ShadowMatrix.m1.z * input[i].y + ShadowMatrix.m2.z * input[i].z + \
        ShadowMatrix.m3.z;                                                                                           \
    output[i].w = ShadowMatrix.m0.w * input[i].x + ShadowMatrix.m1.w * input[i].y + ShadowMatrix.m2.w * input[i].z + \
        ShadowMatrix.m3.w;

void agiMeshSet::ShadowTransform(Vector4* ARTS_RESTRICT output, Vector3* ARTS_RESTRICT input, i32 count)
{
    STATS.VertsXfrm += count;

    if (FogValue == 0.0f)
    {
        for (i32 i = 0; i < count; ++i)
        {
            ARTS_SHADOW_TRANSFORM_DOT;
        }
    }
    else
    {
        for (i32 i = 0; i < count; ++i)
        {
            ARTS_SHADOW_TRANSFORM_DOT;
            ARTS_TRANSFORM_FOG;
        }
    }
}

u32 agiMeshSet::ShadowTransformOutcode(
    u8* ARTS_RESTRICT out_codes, Vector4* ARTS_RESTRICT output, Vector3* ARTS_RESTRICT input, i32 count)
{
    STATS.VertsOut += count;
    STATS.VertsXfrm += count;

    u8 clip_any = 0;
    u8 clip_all = 0xFF;

    if (FogValue == 0.0f)
    {
        for (i32 i = 0; i < count; ++i)
        {
            ARTS_SHADOW_TRANSFORM_DOT;
            ARTS_TRANSFORM_CODE;
        }
    }
    else
    {
        for (i32 i = 0; i < count; ++i)
        {
            ARTS_SHADOW_TRANSFORM_DOT;
            ARTS_TRANSFORM_FOG;
            ARTS_TRANSFORM_CODE;
        }
    }

    return clip_any | (clip_all << 8);
}

void agiMeshSet::SetFog(f32 fog, i32 /*arg2*/)
{
    FogValue = fog ? (255.0f / fog) : 0.0f;
}

// A simple wrapper to avoid default constructing elements
template <typename T, std::size_t N>
struct PodArray
{
    static_assert(std::is_trivially_copy_assignable_v<T>);

    alignas(T) unsigned char Data[N][sizeof(T)];

    ARTS_FORCEINLINE operator T*()
    {
        return reinterpret_cast<T*>(Data);
    }

    ARTS_FORCEINLINE operator const T*() const
    {
        return reinterpret_cast<const T*>(Data);
    }
};

// TODO: Process all clipped CV's at once, similar to ToScreen
void agiMeshSet::ClipTri(i32 i1, i32 i2, i32 i3, i32 texture)
{
    ARTS_UTIMED(agiClipTimer);

    if ((ClippedVertCount + 16) > ARTS_SIZE(ClippedVerts) || ClippedTriCount == ARTS_SIZE(ClippedTris))
        Quitf("ClipTri: clip buffer overflow");

    PodArray<CV, 16> cv_in;
    cv_in[0] = {out[VertexIndices[i1]], 1.0f, 0.0f, 0.0f};
    cv_in[1] = {out[VertexIndices[i2]], 0.0f, 1.0f, 0.0f};
    cv_in[2] = {out[VertexIndices[i3]], 0.0f, 0.0f, 1.0f};

    u32 count = OnlyZClip ? ZClipOnly(&ClippedVerts[ClippedVertCount], cv_in, 3)
                          : FullClip(&ClippedVerts[ClippedVertCount], cv_in, 3);

    if (count)
    {
        for (u32 i = 0; i < count; ++i)
        {
            CV& vert = ClippedVerts[ClippedVertCount + i];

            f32 inv_w = 1.0f / vert.w;
            f32 x = vert.x * inv_w * HalfWidth + OffsX;
            f32 y = vert.y * inv_w * HalfHeight + OffsY;
            f32 z = vert.z * inv_w * DepthScale + DepthOffset;

            vert.fog = CalculateFog(vert.w, FogValue);
            vert.x = x;
            vert.y = y;
            vert.z = z;
            vert.w = inv_w;

            ClampToScreen(vert);
        }

        ClippedTris[ClippedTriCount] = {ClippedVertCount, count,
            {static_cast<u32>(i1), static_cast<u32>(i2), static_cast<u32>(i3)}, ClippedTextures[texture]};
        ClippedTextures[texture] = &ClippedTris[ClippedTriCount];
        ++ClippedTriCount;
        ClippedVertCount += count;
    }
}

void agiMeshSet::InitViewport(agiViewParameters& params)
{
    f32 pipe_width = static_cast<f32>(Pipe()->GetWidth());
    f32 pipe_height = static_cast<f32>(Pipe()->GetHeight());

    f32 x = std::round(pipe_width * params.X);
    f32 y = std::round(pipe_height * params.Y);
    f32 w = std::round(pipe_width * (params.X + params.Width)) - x;
    f32 h = std::round(pipe_height * (params.Y + params.Height)) - y;
    y = pipe_height - (y + h);

    HalfWidth = w * 0.5f;
    HalfHeight = -h * 0.5f;

    OffsX = x + HalfWidth;
    OffsY = y - HalfHeight;

    if (GetRendererInfo().SpecialFlags & 0x8)
        HalfHeight *= 1.01f;

    MinX = OffsX - HalfWidth;
    MaxX = OffsX + HalfWidth;
    MinY = OffsY + HalfHeight;
    MaxY = OffsY - HalfHeight;

    OnlyZClip = false;
    ClipMask = MESH_CLIP_ANY;

    if (GetRendererInfo().SpecialFlags & 0x20)
    {
        // TODO: Use viewport scissor region
        // TODO: Allow custom [Min/Max]Z
        if (MinX <= 0.0f && MaxX >= pipe_width && MinY <= 0.0f && MaxY >= pipe_height)
        {
            MinX = -INFINITY;
            MaxX = +INFINITY;
            MinY = -INFINITY;
            MaxY = +INFINITY;

            OnlyZClip = true;
            ClipMask = MESH_CLIP_NZ | MESH_CLIP_PZ;
        }
    }
    else
    {
        MinX = std::max(MinX, x);
        MaxX = std::min(MaxX, x + w);
        MinY = std::max(MinY, y);
        MaxY = std::min(MaxY, y + h);

        MinX = std::max(MinX, 0.0f);
        MaxX = std::min(MaxX, pipe_width);
        MinY = std::max(MinY, 0.0f);
        MaxY = std::min(MaxY, pipe_height);
    }

    if (FlipX)
        HalfWidth = -HalfWidth;
}

b32 agiMeshSet::Draw(u32 flags)
{
    // FIXME: Avoid this check
    // Lots of unchecked calls in aiVehicleInstance::Draw
    if (agiMeshSet* volatile this_ptr = this; !this_ptr)
        return false;

    bool drawn = false;

    if (LockIfResident())
    {
        if (Geometry(flags, Vertices, Planes) <= 0xFF)
        {
            FirstPass(Colors, TexCoords, 0xFFFFFFFF);
            drawn = true;
        }

        Unlock();
    }
    else
    {
        PageIn();
    }

    return drawn;
}

b32 agiMeshSet::DrawColor(u32 color, u32 flags)
{
    bool drawn = false;

    if (LockIfResident())
    {
        if (Geometry(flags, Vertices, Planes) <= 0xFF)
        {
            u32* colors = Colors;

            if (colors && color != 0xFFFFFFFF)
            {
                u32* shaded = ARTS_ALLOCA(u32, AdjunctCount);

                {
                    ARTS_UTIMED(agiLightTimer);
                    agiBlendColors(shaded, colors, AdjunctCount, color);
                }

                colors = shaded;
                color = 0xFFFFFFFF;
            }

            FirstPass(colors, TexCoords, color);

            drawn = true;
        }

        Unlock();
    }
    else
    {
        PageIn();
    }

    return drawn;
}

b32 agiMeshSet::DrawLit(agiMeshLighter lighter, u32 flags, u32* colors)
{
    if (!lighter)
        return Draw(flags);

    bool drawn = false;

    if (LockIfResident())
    {
        if (Geometry(flags, Vertices, Planes) <= 0xFF)
        {
            u32* shaded = ARTS_ALLOCA(u32, AdjunctCount);

            {
                ARTS_UTIMED(agiLightTimer);
                lighter(codes, shaded, colors ? colors : Colors, this);
            }

            FirstPass(shaded, TexCoords, 0);
            drawn = true;
        }

        Unlock();
    }
    else
    {
        PageIn();
    }

    return drawn;
}

void agiMeshSet::DrawLitEnv(agiMeshLighter lighter, agiTexDef* env_map, Matrix34& transform, u32 flags)
{
    if (LockIfResident())
    {
        if (agiCurState.GetMaxTextures() > 1 && agiRQ.EnvMap)
        {
            if (Geometry(flags, Vertices, Planes) <= 0xFF)
            {
                u32* colors = Colors;

                if (lighter)
                {
                    u32* shaded = ARTS_ALLOCA(u32, AdjunctCount);

                    {
                        ARTS_UTIMED(agiLightTimer);
                        lighter(codes, shaded, colors, this);
                    }

                    colors = shaded;
                }

                MultiTexEnvMap(colors, 0xFFFFFFFF, env_map, transform);
            }
        }
        else if (DrawLit(lighter, flags, nullptr) && env_map)
        {
            EnvMap(transform, env_map, 0xFFFFFFFF);
        }

        Unlock();
    }
    else
    {
        PageIn();
    }
}

void agiMeshSet::DrawNormals([[maybe_unused]] Vector3& color)
{
    // FIXME: Avoid this check
    if (agiMeshSet* volatile this_ptr = this; !this_ptr)
        return;

    if (LockIfResident())
    {
#ifdef ARTS_DEV_BUILD
        if (Normals)
        {
            DrawBegin(xconst(ViewParams().World));
            ::DrawColor(color);

            for (u32 i = 0; i < AdjunctCount; ++i)
            {
                Vector3 start = Vertices[VertexIndices[i]];
                Vector3 end = start + UnpackNormal[Normals[i]];
                DrawLine(start, end);
            }

            DrawEnd();
        }
#endif

        Unlock();
    }
    else
    {
        PageIn();
    }
}

void agiMeshSet::DrawShadow(u32 flags, const Vector4& plane, const Vector3& light_dir)
{
    if (const Vector3& camera = ViewParams().Camera.m3;
        camera.x * plane.x + camera.y * plane.y + camera.z * plane.z + plane.w < 0.0f)
        return;

    if (LockIfResident())
    {
        if (ShadowGeometry(flags, Vertices, plane, light_dir) <= 0xFF)
        {
            FirstPass(nullptr, nullptr, ShadowColor);
        }

        Unlock();
    }
    else
    {
        PageIn();
    }
}

static void (agiMeshSet::*const FirstPassFunctions[2][2][2][2])(u32* colors, Vector2* tex_coords, u32 color) {
    {
        {
            {&agiMeshSet::FirstPass_HW_UV_CPV_noDYNTEX, &agiMeshSet::FirstPass_HW_UV_CPV_DYNTEX},
            {&agiMeshSet::FirstPass_HW_UV_noCPV_noDYNTEX, &agiMeshSet::FirstPass_HW_UV_noCPV_DYNTEX},
        },
        {
            {&agiMeshSet::FirstPass_HW_noUV_CPV_noDYNTEX, &agiMeshSet::FirstPass_HW_noUV_CPV_DYNTEX},
            {&agiMeshSet::FirstPass_HW_noUV_noCPV_noDYNTEX, &agiMeshSet::FirstPass_HW_noUV_noCPV_DYNTEX},
        },
    },
    {
        {
            {&agiMeshSet::FirstPass_SW_UV_CPV_noDYNTEX, &agiMeshSet::FirstPass_SW_UV_CPV_DYNTEX},
            {&agiMeshSet::FirstPass_SW_UV_noCPV_noDYNTEX, &agiMeshSet::FirstPass_SW_UV_noCPV_DYNTEX},
        },
        {
            {&agiMeshSet::FirstPass_SW_noUV_CPV_noDYNTEX, &agiMeshSet::FirstPass_SW_noUV_CPV_DYNTEX},
            {&agiMeshSet::FirstPass_SW_noUV_noCPV_noDYNTEX, &agiMeshSet::FirstPass_SW_noUV_noCPV_DYNTEX},
        },
    },
};

ARTS_IMPORT extern i32 DynTexFlag; // mmCarModel::Draw, mmVehicleForm::Cull

void agiMeshSet::FirstPass(u32* colors, Vector2* tex_coords, u32 color)
{
    // FIXME: SW_noUV_* does not unbind the current texture
    if (tex_coords == nullptr)
    {
        agiCurState.SetTexture(nullptr);
    }

    if (agiCurState.GetDrawMode() == agiDrawDepth)
    {
        colors = nullptr;
        color = 0xFF202020;
    }

    ARTS_UTIMED(agiFirstPass);

    (this->*FirstPassFunctions[agiCurState.GetSoftwareRendering()][tex_coords == nullptr][colors == nullptr][(DynTexFlag & MESH_DRAW_DYNTEX) != 0])(
        colors, tex_coords, color);
}

ARTS_IMPORT extern i32 CurrentMeshSetVariant;

template <typename T>
static inline void fill_bytes(T* dst, usize len, u8 value)
{
    std::memset(dst, value, len * sizeof(T));
}

i32 agiMeshSet::Geometry(u32 flags, Vector3* verts, Vector4* planes)
{
    ArAssert(Resident == 2, "Mesh not loaded");

    ClippedVertCount = 0;
    ClippedTriCount = 0;
    fill_bytes(ClippedTextures, TextureCount + 1, 0);

    Init(planes && (SurfaceCount > 1));

    u32 clip_mask = (flags & MESH_DRAW_CLIP) ? MESH_CLIP_ANY : 0; // clip_any | (clip_all << 8)

    {
        ARTS_UTIMED(agiTransformTimer);

        // If available, transform the bounding box, checking how visible it is
        if (clip_mask && BoundingBox)
        {
            clip_mask = TransformOutcode(codes, out, BoundingBox, 8);

            if (clip_mask > 0xFF) // All verts are clipped (not visible)
                return clip_mask;
        }

        if (clip_mask)
        {
            clip_mask = TransformOutcode(codes, out, verts, VertexCount);

            if (clip_mask > 0xFF) // All verts are clipped (not visible)
                return clip_mask;
        }
        else // Assume nothing needs clipping
        {
            Transform(out, verts, VertexCount);
        }
    }

    {
        ARTS_UTIMED(agiTraverseTimer);

        fill_bytes(firstFacet, TextureCount + 1, 0xFF);
        fill_bytes(vertCounts, TextureCount + 1, 0);
        fill_bytes(indexCounts, TextureCount + 1, 0);

        DynTexFlag = flags & MESH_DRAW_DYNTEX;
        CurrentMeshSetVariant = std::min<i32>(MESH_DRAW_GET_VARIANT(flags), VariationCount - 1);

        if (clip_mask)
        {
            for (u32 i = 0; i < SurfaceCount; ++i)
            {
                u16 surface[4];
                std::memcpy(surface, &SurfaceIndices[i * 4], sizeof(surface));

                u16 indices[4];
                indices[0] = VertexIndices[surface[0]];
                indices[1] = VertexIndices[surface[1]];
                indices[2] = VertexIndices[surface[2]];

                u8 clip_any = codes[indices[0]] | codes[indices[1]] | codes[indices[2]];
                u8 clip_all = codes[indices[0]] & codes[indices[1]] & codes[indices[2]];

                i16 num_verts;
                i16 num_index;

                if (surface[3])
                {
                    indices[3] = VertexIndices[surface[3]];
                    clip_any |= codes[indices[3]];
                    clip_all &= codes[indices[3]];
                    num_verts = 4;
                    num_index = 6;
                }
                else
                {
                    num_verts = 3;
                    num_index = 3;
                }

                if (!(clip_all & MESH_CLIP_ANY) && (!planes || !IsBackfacing(planes[i])))
                {
                    u8 texture = TextureIndices[i];

                    if (clip_any & ClipMask)
                    {
                        if (surface[3])
                        {
                            ClipTri(surface[1], surface[2], surface[3], texture);
                            ClipTri(surface[1], surface[3], surface[0], texture);
                        }
                        else
                        {
                            ClipTri(surface[0], surface[1], surface[2], texture);
                        }
                    }
                    else
                    {
                        vertCounts[texture] += num_verts;
                        indexCounts[texture] += num_index;
                        nextFacet[i] = firstFacet[texture];
                        firstFacet[texture] = static_cast<i16>(i);
                    }

                    codes[indices[0]] |= MESH_CLIP_SCREEN;
                    codes[indices[1]] |= MESH_CLIP_SCREEN;
                    codes[indices[2]] |= MESH_CLIP_SCREEN;

                    if (surface[3])
                        codes[indices[3]] |= MESH_CLIP_SCREEN;
                }
            }
        }
        else
        {
            // Either we didn't use TransformOutcode, or we don't care about the clipping it returned. Either way, initialize codes
            fill_bytes(codes, VertexCount,
#ifdef CLIP_ALL_TO_SCREEN
                MESH_CLIP_SCREEN
#else
                0
#endif
            );

            for (u32 i = 0; i < SurfaceCount; ++i)
            {
                if (!planes || !IsBackfacing(planes[i]))
                {
                    u8 texture = TextureIndices[i];
                    const u16* ARTS_RESTRICT surface = &SurfaceIndices[i * 4];

                    if (surface[3])
                    {
                        vertCounts[texture] += 4;
                        indexCounts[texture] += 6;

#ifndef CLIP_ALL_TO_SCREEN
                        codes[VertexIndices[surface[3]]] |= MESH_CLIP_SCREEN;
#endif
                    }
                    else
                    {
                        vertCounts[texture] += 3;
                        indexCounts[texture] += 3;
                    }

#ifndef CLIP_ALL_TO_SCREEN
                    codes[VertexIndices[surface[0]]] |= MESH_CLIP_SCREEN;
                    codes[VertexIndices[surface[1]]] |= MESH_CLIP_SCREEN;
                    codes[VertexIndices[surface[2]]] |= MESH_CLIP_SCREEN;
#endif

                    nextFacet[i] = firstFacet[texture];
                    firstFacet[texture] = static_cast<i16>(i);
                }
            }
        }
    }

    ToScreen(codes, out, VertexCount);

    return clip_mask;
}

i32 agiMeshSet::ShadowGeometry(u32 flags, Vector3* verts, const Vector4& plane, const Vector3& light_dir)
{
    ClippedVertCount = 0;
    ClippedTriCount = 0;
    ClippedTextures[0] = 0;
    ShadowInit(plane, light_dir);

    u32 clip_mask = (flags & MESH_DRAW_CLIP) ? MESH_CLIP_ANY : 0; // clip_any | (clip_all << 8)

    {
        ARTS_UTIMED(agiTransformTimer);

        // If available, transform the bounding box, checking how visible it is
        if (clip_mask && BoundingBox)
        {
            clip_mask = ShadowTransformOutcode(codes, out, BoundingBox, 8);

            if (clip_mask > 0xFF) // All verts are clipped (not visible)
                return clip_mask;
        }

        if (clip_mask)
        {
            clip_mask = ShadowTransformOutcode(codes, out, verts, VertexCount);

            if (clip_mask > 0xFF) // All verts are clipped (not visible)
                return clip_mask;
        }
        else // Assume nothing needs clipping
        {
            ShadowTransform(out, verts, VertexCount);
        }
    }

    {
        ARTS_UTIMED(agiTraverseTimer);

        DynTexFlag = 0;
        CurrentMeshSetVariant = 0;

        firstFacet[0] = -1;
        vertCounts[0] = 0;
        indexCounts[0] = 0;

        if (clip_mask)
        {
            for (u32 i = 0; i < SurfaceCount; ++i)
            {
                u16 surface[4];
                std::memcpy(surface, &SurfaceIndices[i * 4], sizeof(surface));

                u16 indices[4];
                indices[0] = VertexIndices[surface[0]];
                indices[1] = VertexIndices[surface[1]];
                indices[2] = VertexIndices[surface[2]];

                u8 clip_any = codes[indices[0]] | codes[indices[1]] | codes[indices[2]];
                u8 clip_all = codes[indices[0]] & codes[indices[1]] & codes[indices[2]];

                i16 num_verts;
                i16 num_index;

                if (surface[3])
                {
                    indices[3] = VertexIndices[surface[3]];
                    clip_any |= codes[indices[3]];
                    clip_all &= codes[indices[3]];
                    num_verts = 4;
                    num_index = 6;
                }
                else
                {
                    num_verts = 3;
                    num_index = 3;
                }

                if (!(clip_all & MESH_CLIP_ANY))
                {
                    if (clip_any & ClipMask)
                    {
                        if (surface[3])
                        {
                            ClipTri(surface[1], surface[2], surface[3], 0);
                            ClipTri(surface[1], surface[3], surface[0], 0);
                        }
                        else
                        {
                            ClipTri(surface[0], surface[1], surface[2], 0);
                        }
                    }
                    else
                    {
                        vertCounts[0] += num_verts;
                        indexCounts[0] += num_index;
                        nextFacet[i] = firstFacet[0];
                        firstFacet[0] = static_cast<i16>(i);
                    }

                    codes[indices[0]] |= MESH_CLIP_SCREEN;
                    codes[indices[1]] |= MESH_CLIP_SCREEN;
                    codes[indices[2]] |= MESH_CLIP_SCREEN;

                    if (surface[3])
                        codes[indices[3]] |= MESH_CLIP_SCREEN;
                }
            }
        }
        else
        {
            // Either we didn't use TransformOutcode, or we don't care about the clipping it returned. Either way, initialize codes
            fill_bytes(codes, VertexCount,
#ifdef CLIP_ALL_TO_SCREEN
                MESH_CLIP_SCREEN
#else
                0
#endif
            );

            for (u32 i = 0; i < SurfaceCount; ++i)
            {
                const u16* ARTS_RESTRICT surface = &SurfaceIndices[i * 4];

                if (surface[3])
                {
                    vertCounts[0] += 4;
                    indexCounts[0] += 6;

#ifndef CLIP_ALL_TO_SCREEN
                    codes[VertexIndices[surface[3]]] |= MESH_CLIP_SCREEN;
#endif
                }
                else
                {
                    vertCounts[0] += 3;
                    indexCounts[0] += 3;
                }

#ifndef CLIP_ALL_TO_SCREEN
                codes[VertexIndices[surface[0]]] |= MESH_CLIP_SCREEN;
                codes[VertexIndices[surface[1]]] |= MESH_CLIP_SCREEN;
                codes[VertexIndices[surface[2]]] |= MESH_CLIP_SCREEN;
#endif

                nextFacet[i] = firstFacet[0];
                firstFacet[0] = static_cast<i16>(i);
            }
        }
    }

    ToScreen(codes, out, VertexCount);

    return clip_mask;
}

ARTS_IMPORT extern agiMeshCardInfo CurrentMeshCard;

void agiMeshSet::DrawCard(Vector3& position, f32 scale, u32 rotation, u32 color, u32 frame)
{
    const agiViewParameters& view_params = ViewParams();
    const Matrix34& matrix = view_params.ModelView;

    f32 w = -(matrix.m0.z * position.x + matrix.m1.z * position.y + matrix.m2.z * position.z + matrix.m3.z);
    f32 z = w * ProjZZ + ProjZW;

    if (-w > z || z > w)
        return;

    f32 x = matrix.m0.x * position.x + matrix.m1.x * position.y + matrix.m2.x * position.z + matrix.m3.x;
    f32 y = matrix.m0.y * position.x + matrix.m1.y * position.y + matrix.m2.y * position.z + matrix.m3.y;

    u8 clip_any = 0;
    u8 clip_all = 0xFF;
    PodArray<Vector2, 4> positions;

    Vector2* rotations = &CurrentMeshCard.Rotations[4 * ((CurrentMeshCard.PointCount - 1) & rotation)];
    i32 vert_count = CurrentMeshCard.VertCount;

    for (i32 i = 0; i < vert_count; ++i)
    {
        f32 vert_x = (scale * rotations[i].x + x) * view_params.ProjX + z * view_params.ProjXZ;
        f32 vert_y = (scale * rotations[i].y + y) * view_params.ProjY + z * view_params.ProjYZ;

        i32 clip = 0;
        clip |= (-w > vert_x) ? MESH_CLIP_PX : (vert_x > w) ? MESH_CLIP_NX : 0;
        clip |= (-w > vert_y) ? MESH_CLIP_PY : (vert_y > w) ? MESH_CLIP_NY : 0;

        clip_any |= clip;
        clip_all &= clip;
        positions[i].x = vert_x;
        positions[i].y = vert_y;
    }

    if (clip_all)
        return;

    if (agiCurState.GetDrawMode() == agiDrawDepth)
        color = 0xFF202020;

    u32 fog = CalculateFog(w, FogValue) << 24;
    f32 inv_w = 1.0f / w;

    if (f32 size = scale * inv_w; size < MinCardSize || size > MaxCardSize)
        return;

    f32 vert_z = z * inv_w * DepthScale + DepthOffset;
    Vector2* tex_coords = &CurrentMeshCard.Frames[4 * frame];

    if (clip_any & ClipMask)
    {
        for (i32 i = 0; i < vert_count - 2; ++i)
        {
            PodArray<CV, 16> cv_in;
            PodArray<CV, 16> cv_out;
            PodArray<agiScreenVtx, 16> verts;

            cv_in[0] = CV(positions[0], z, w, 1.0f, 0.0f, 0.0f, 0, i + 1, i + 2);
            cv_in[1] = CV(positions[i + 1], z, w, 0.0f, 1.0f, 0.0f, 0, i + 1, i + 2);
            cv_in[2] = CV(positions[i + 2], z, w, 0.0f, 0.0f, 1.0f, 0, i + 1, i + 2);

            if (i32 clipped = FullClip(cv_out, cv_in, 3))
            {
                agiPolySet* poly = nullptr;

                if (!agiCurState.GetSoftwareRendering())
                    poly = agiTexSorter::BeginVerts(agiCurState.GetTexture(), clipped, 3 * (clipped - 2));

                for (i32 j = 0; j < clipped; ++j)
                {
                    agiScreenVtx& vert = !agiCurState.GetSoftwareRendering() ? poly->Vert() : verts[clipped - j - 1];

                    vert.x = cv_out[j].x * inv_w * HalfWidth + OffsX;
                    vert.y = cv_out[j].y * inv_w * HalfHeight + OffsY;
                    vert.z = vert_z;
                    vert.w = inv_w;

                    vert.tu = cv_out[j].map[0] * tex_coords[cv_out[j].idx[0]].x +
                        cv_out[j].map[1] * tex_coords[cv_out[j].idx[1]].x +
                        cv_out[j].map[2] * tex_coords[cv_out[j].idx[2]].x;

                    vert.tv = cv_out[j].map[0] * tex_coords[cv_out[j].idx[0]].y +
                        cv_out[j].map[1] * tex_coords[cv_out[j].idx[1]].y +
                        cv_out[j].map[2] * tex_coords[cv_out[j].idx[2]].y;

                    vert.color = color;
                    vert.specular = fog;

                    ClampToScreen(vert);
                }

                if (agiCurState.GetSoftwareRendering())
                {
                    swPoly(verts, clipped);
                }
                else
                {
                    for (i32 j = 2; j < clipped; ++j)
                        poly->Triangle(0, j, j - 1);

                    agiTexSorter::EndVerts();
                }
            }
        }
    }
    else
    {
        PodArray<agiScreenVtx, 4> verts;

        for (i32 i = 0; i < vert_count; ++i)
        {
            verts[i].x = positions[i].x * inv_w * HalfWidth + OffsX;
            verts[i].y = positions[i].y * inv_w * HalfHeight + OffsY;
            verts[i].z = vert_z;
            verts[i].w = inv_w;
            verts[i].tu = tex_coords[i].x;
            verts[i].tv = tex_coords[i].y;
            verts[i].color = color;
            verts[i].specular = fog;

            ClampToScreen(verts[i]);
        }

        if (agiCurState.GetSoftwareRendering())
        {
            if (vert_count == 3)
                swTri(&verts[2], &verts[1], &verts[0]);
            else
                swQuad(&verts[3], &verts[2], &verts[1], &verts[0]);
        }
        else
        {
            agiPolySet* poly = agiTexSorter::BeginVerts(agiCurState.GetTexture(), vert_count, 3 * (vert_count - 2));

            for (i32 i = 0; i < vert_count; ++i)
                poly->Vert() = verts[i];

            if (CurrentMeshCard.VertCount == 4)
                poly->Quad(3, 2, 1, 0);
            else
                poly->Triangle(2, 1, 0);

            agiTexSorter::EndVerts();
        }
    }
}

void agiMeshSet::DrawLines(Vector3* starts, Vector3* ends, u32* colors, i32 count)
{
    agiMeshSet::Init(false);

    agiScreenVtx* ARTS_RESTRICT verts = ARTS_ALLOCA(agiScreenVtx, count * 2);
    u32 stored = 0;

    for (; count; --count)
    {
        Vector3 start = *starts++;
        Vector3 end = *ends++;
        u32 color = *colors++;

        f32 start_w = M.m0.z * start.x + M.m1.z * start.y + M.m2.z * start.z + M.m3.z;
        f32 start_z = start_w * ProjZZ + ProjZW;

        if (-start_w > start_z || start_z > start_w)
            continue;

        f32 end_w = M.m0.z * end.x + M.m1.z * end.y + M.m2.z * end.z + M.m3.z;
        f32 end_z = end_w * ProjZZ + ProjZW;

        if (-end_w > end_z || end_z > end_w)
            continue;

        f32 start_x = M.m0.x * start.x + M.m1.x * start.y + M.m2.x * start.z + M.m3.x;
        f32 start_y = M.m0.y * start.x + M.m1.y * start.y + M.m2.y * start.z + M.m3.y;

        if (-start_w > start_x || start_x > start_w || -start_w > start_y || start_y > start_w)
            continue;

        f32 end_x = M.m0.x * end.x + M.m1.x * end.y + M.m2.x * end.z + M.m3.x;
        f32 end_y = M.m0.y * end.x + M.m1.y * end.y + M.m2.y * end.z + M.m3.y;

        if (-end_w > end_x || end_x > end_w || -end_w > end_y || end_y > end_w)
            continue;

        f32 start_rhw = 1.0f / start_w;
        f32 end_rhw = 1.0f / end_w;

        verts[stored].x = start_x * start_rhw * HalfWidth + OffsX;
        verts[stored].y = start_y * start_rhw * HalfHeight + OffsY;
        verts[stored].z = start_z * start_rhw * DepthScale + DepthOffset;
        verts[stored].w = start_rhw;
        verts[stored].color = color;
        verts[stored].specular = 0xFF000000;
        verts[stored].tv = 0.0f;
        verts[stored].tu = 0.0f;
        ClampToScreen(verts[stored]);
        ++stored;

        verts[stored].x = end_x * end_rhw * HalfWidth + OffsX;
        verts[stored].y = end_y * end_rhw * HalfHeight + OffsY;
        verts[stored].z = end_z * end_rhw * DepthScale + DepthOffset;
        verts[stored].w = end_rhw;
        verts[stored].color = color;
        verts[stored].specular = 0xFF000000;
        verts[stored].tv = 0.0f;
        verts[stored].tu = 0.0f;
        ClampToScreen(verts[stored]);
        ++stored;
    }

    if (stored)
    {
        ArAssert((stored & 1) == 0, "Invalid Vertex Count");

        RAST->LineList(agiVtxType::Screen, (agiVtx*) verts, stored);
    }
}

// clang-format off
static u16 QuadIndices[144] {
     0,  1,  2,  0,  2,  3,
     4,  5,  6,  4,  6,  7,
     8,  9, 10,  8, 10, 11,
    12, 13, 14, 12, 14, 15,
    16, 17, 18, 16, 18, 19,
    20, 21, 22, 20, 22, 23,
    24, 25, 26, 24, 26, 27,
    28, 29, 30, 28, 30, 31,
    32, 33, 34, 32, 34, 35,
    36, 37, 38, 36, 38, 39,
    40, 41, 42, 40, 42, 43,
    44, 45, 46, 44, 46, 47,
    48, 49, 50, 48, 50, 51,
    52, 53, 54, 52, 54, 55,
    56, 57, 58, 56, 58, 59,
    60, 61, 62, 60, 62, 63,
    64, 65, 66, 64, 66, 67,
    68, 69, 70, 68, 70, 71,
    72, 73, 74, 72, 74, 75,
    76, 77, 78, 76, 78, 79,
    80, 81, 82, 80, 82, 83,
    84, 85, 86, 84, 86, 87,
    88, 89, 90, 88, 90, 91,
    92, 93, 94, 92, 94, 95,
};
// clang-format on

void agiMeshSet::DrawWideLines(Vector3* starts, Vector3* ends, f32* widths, u32* colors, i32 count)
{
    const agiViewParameters& vp = ViewParams();

    agiMeshSet::Init(false);

    agiScreenVtx* ARTS_RESTRICT verts = ARTS_ALLOCA(agiScreenVtx, count * 4);
    u32 specular = CalculateFog(M.m3.z, FogValue) << 24;

    u32 stored = 0;
    u32 num_indices = 0;

    for (; count; --count)
    {
        Vector3 start = *starts++;
        Vector3 end = *ends++;
        f32 start_width = *widths++;
        f32 end_width = *widths++;
        u32 color = *colors++;

        f32 start_w = M.m0.z * start.x + M.m1.z * start.y + M.m2.z * start.z + M.m3.z;
        f32 start_z = start_w * ProjZZ + ProjZW;

        if (-start_w > start_z || start_z > start_w)
            continue;

        f32 end_w = M.m0.z * end.x + M.m1.z * end.y + M.m2.z * end.z + M.m3.z;
        f32 end_z = end_w * ProjZZ + ProjZW;

        if (-end_w > end_z || end_z > end_w)
            continue;

        f32 start_x = M.m0.x * start.x + M.m1.x * start.y + M.m2.x * start.z + M.m3.x;
        f32 start_y = M.m0.y * start.x + M.m1.y * start.y + M.m2.y * start.z + M.m3.y;

        if (-start_w > start_x || start_x > start_w || -start_w > start_y || start_y > start_w)
            continue;

        f32 end_x = M.m0.x * end.x + M.m1.x * end.y + M.m2.x * end.z + M.m3.x;
        f32 end_y = M.m0.y * end.x + M.m1.y * end.y + M.m2.y * end.z + M.m3.y;

        if (-end_w > end_x || end_x > end_w || -end_w > end_y || end_y > end_w)
            continue;

        f32 start_rhw = 1.0f / start_w;
        f32 end_rhw = 1.0f / end_w;

        start_x = start_x * start_rhw * HalfWidth + OffsX;
        start_y = start_y * start_rhw * HalfHeight + OffsY;

        end_x = end_x * end_rhw * HalfWidth + OffsX;
        end_y = end_y * end_rhw * HalfHeight + OffsY;

        if (std::abs(start_y - end_y) >= std::abs(start_x - end_x))
        {
            start_width = (start_width * vp.ProjX + start_z * vp.ProjXZ) * start_rhw * HalfWidth;
            end_width = (end_width * vp.ProjX + end_z * vp.ProjXZ) * end_rhw * HalfWidth;

            verts[stored + 0].x = start_x + start_width;
            verts[stored + 0].y = start_y;

            verts[stored + 1].x = start_x - start_width;
            verts[stored + 1].y = start_y;

            verts[stored + 2].x = end_x - end_width;
            verts[stored + 2].y = end_y;

            verts[stored + 3].x = end_x + end_width;
            verts[stored + 3].y = end_y;
        }
        else
        {
            start_width = (start_width * vp.ProjY + start_z * vp.ProjYZ) * start_rhw * HalfHeight;
            end_width = (end_width * vp.ProjY + end_z * vp.ProjYZ) * end_rhw * HalfHeight;

            verts[stored + 0].x = start_x;
            verts[stored + 0].y = start_y - start_width;

            verts[stored + 1].x = start_x;
            verts[stored + 1].y = start_y + start_width;

            verts[stored + 2].x = end_x;
            verts[stored + 2].y = end_y + end_width;

            verts[stored + 3].x = end_x;
            verts[stored + 3].y = end_y - end_width;
        }

        start_z = start_z * start_rhw * DepthScale + DepthOffset;
        end_z = end_z * end_rhw * DepthScale + DepthOffset;

        start_z = std::max(std::min(start_z, 1.0f), 0.0f);
        end_z = std::max(std::min(end_z, 1.0f), 0.0f);

        verts[stored + 0].z = start_z;
        verts[stored + 0].w = start_rhw;
        verts[stored + 0].color = color;
        verts[stored + 0].specular = specular;
        verts[stored + 0].tu = 0.0f;
        verts[stored + 0].tv = 0.0f;

        verts[stored + 1].z = start_z;
        verts[stored + 1].w = start_rhw;
        verts[stored + 1].color = color;
        verts[stored + 1].specular = specular;
        verts[stored + 1].tu = 0.0f;
        verts[stored + 1].tv = 0.0f;

        verts[stored + 2].z = end_z;
        verts[stored + 2].w = end_rhw;
        verts[stored + 2].color = color;
        verts[stored + 2].specular = specular;
        verts[stored + 2].tu = 0.0f;
        verts[stored + 2].tv = 0.0f;

        verts[stored + 3].z = end_z;
        verts[stored + 3].w = end_rhw;
        verts[stored + 3].color = color;
        verts[stored + 3].specular = specular;
        verts[stored + 3].tu = 0.0f;
        verts[stored + 3].tv = 0.0f;

        for (i32 i = 0; i < 4; ++i)
        {
            verts[stored + i].x = std::max(std::min(verts[stored + i].x, MaxX), MinX);
            verts[stored + i].y = std::max(std::min(verts[stored + i].y, MaxY), MinY);
        }

        stored += 4;
        num_indices += 6;
    }

    if (stored)
    {
        ArAssert(num_indices <= ARTS_SIZE32(QuadIndices), "Too Many Indices");

        agiCullMode old_cull = agiCurState.SetCullMode(agiCullMode::None);
        agiCurState.SetTexture(nullptr);

        RAST->Mesh(agiVtxType::Screen, (agiVtx*) verts, stored, QuadIndices, num_indices);

        agiCurState.SetCullMode(old_cull);
    }
}
