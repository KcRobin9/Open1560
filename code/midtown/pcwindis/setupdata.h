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
    pcwindis:setupdata

    0x5749B0 | int __cdecl dxiResClosestMatch(int,int,int) | ?dxiResClosestMatch@@YAHHHH@Z
    0x574A60 | int __cdecl dxiResGetRecommended(int,int) | ?dxiResGetRecommended@@YAHHH@Z
    0x574B00 | int __cdecl dxiReadConfigFile(void) | ?dxiReadConfigFile@@YAHXZ
    0x574DF0 | void __cdecl strtoguid(struct _GUID *,char *) | ?strtoguid@@YAXPAU_GUID@@PAD@Z
    0x574E90 | void __cdecl dxiWriteConfigFile(void) | ?dxiWriteConfigFile@@YAXXZ
    0x575090 | void __cdecl guidtostr(char *,struct _GUID *) | ?guidtostr@@YAXPADPAU_GUID@@@Z
    0x661380 | int dxiRendererChoice | ?dxiRendererChoice@@3HA
    0x909680 | struct dxiRendererInfo_t * dxiInfo | ?dxiInfo@@3PAUdxiRendererInfo_t@@A
    0x90A350 | int dxiRendererCount | ?dxiRendererCount@@3HA
*/

#include <guiddef.h>

struct dxiResolution
{
    u16 uWidth;
    u16 uHeight;
    u32 uTexMem;
};

check_size(dxiResolution, 8);

enum class dxiRendererType : i32
{
    Invalid = -1,
    DX6_Soft = 0, // DX6 Software
    DX6_GDI = 1,  // DX6 Hardware (Primary Surface)
    DX6 = 2,      // DX6 Hardware
    OpenGL = 3,   // OpenGL Hardware
    SDL2 = 4,     // SDL_Renderer Software
};

struct dxiRendererInfo_t
{
    b32 Valid;
    b32 Usable;
    dxiRendererType Type2; // Assigned to, but unused

    // 0xA6 for Software
    // 0x85 for Hardware
    u32 Flags;

    b32 SmoothAlpha;
    b32 AdditiveBlending;
    b32 VertexFog;
    b32 MultiTexture;
    b32 TexturePalette;
    b32 HaveMipmaps;

    // 0x1 | Use Software Renderer
    // 0x2 | TextureQuality = 0, FogDistance = 450
    // 0x4 | NoColorKey
    // 0x8 | agiMeshSet::HalfHeight *= 1.01
    // 0x10 | UsePixelFog, ShadowZBias = 0.001 (1588+)
    // 0x20 | Hardware Clipping (Open1560+)
    u32 SpecialFlags;
    char Name[64];

    union
    {
        struct
        {
            GUID Interface;
            GUID Driver;
        } DX6;

        struct
        {
            u32 Format;
            i32 RefreshRate;
            i32 Left;
            i32 Top;
            u32 Width;
            u32 Height;
            i32 Index;
            u32 Padding;
        } SDL;
    };

    dxiRendererType Type;

    dxiResolution Resolutions[32];
    i32 ResCount;
    i32 ResChoice;
};

check_size(dxiRendererInfo_t, 0x198);

// ?dxiReadConfigFile@@YAHXZ
ARTS_EXPORT b32 dxiReadConfigFile();

// ?dxiWriteConfigFile@@YAXXZ
ARTS_EXPORT void dxiWriteConfigFile();

// ?dxiResClosestMatch@@YAHHHH@Z
ARTS_EXPORT i32 dxiResClosestMatch(i32 renderer, i32 width, i32 height);

// ?dxiResGetRecommended@@YAHHH@Z
ARTS_EXPORT i32 dxiResGetRecommended(i32 renderer, i32 cpu_speed);

// ?dxiInfo@@3PAUdxiRendererInfo_t@@A
ARTS_EXPORT extern dxiRendererInfo_t dxiInfo[16];

// ?dxiRendererChoice@@3HA
ARTS_EXPORT extern i32 dxiRendererChoice;

// ?dxiRendererCount@@3HA
ARTS_EXPORT extern i32 dxiRendererCount;

inline dxiRendererInfo_t& GetRendererInfo()
{
    return dxiInfo[dxiRendererChoice];
}

inline bool IsSoftwareRenderer(dxiRendererType type)
{
    return (type == dxiRendererType::DX6_Soft) || (type == dxiRendererType::SDL2);
}

inline bool IsDX6Renderer(dxiRendererType type)
{
    return (type == dxiRendererType::DX6_Soft) || (type == dxiRendererType::DX6_GDI) || (type == dxiRendererType::DX6);
}

inline bool IsSDLRenderer(dxiRendererType type)
{
    return (type == dxiRendererType::OpenGL) || (type == dxiRendererType::SDL2);
}
