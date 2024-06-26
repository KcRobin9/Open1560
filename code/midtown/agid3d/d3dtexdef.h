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
    agid3d:d3dtexdef

    0x5319A0 | public: __thiscall agiD3DTexDef::agiD3DTexDef(class agiPipeline *) | ??0agiD3DTexDef@@QAE@PAVagiPipeline@@@Z
    0x5319D0 | public: virtual __thiscall agiD3DTexDef::~agiD3DTexDef(void) | ??1agiD3DTexDef@@UAE@XZ
    0x5319E0 | public: virtual int __thiscall agiD3DTexDef::BeginGfx(void) | ?BeginGfx@agiD3DTexDef@@UAEHXZ
    0x531D40 | public: virtual void __thiscall agiD3DTexDef::Request(void) | ?Request@agiD3DTexDef@@UAEXXZ
    0x531DA0 | public: struct IDirect3DTexture2 * __thiscall agiD3DTexDef::GetHandle(int) | ?GetHandle@agiD3DTexDef@@QAEPAUIDirect3DTexture2@@H@Z
    0x531E20 | public: virtual int __thiscall agiD3DTexDef::Lock(struct agiTexLock &) | ?Lock@agiD3DTexDef@@UAEHAAUagiTexLock@@@Z
    0x531EC0 | public: virtual void __thiscall agiD3DTexDef::Unlock(struct agiTexLock &) | ?Unlock@agiD3DTexDef@@UAEXAAUagiTexLock@@@Z
    0x531EF0 | public: virtual void __thiscall agiD3DTexDef::EndGfx(void) | ?EndGfx@agiD3DTexDef@@UAEXXZ
    0x531F60 | public: virtual int __thiscall agiD3DTexDef::IsAvailable(void) | ?IsAvailable@agiD3DTexDef@@UAEHXZ
    0x531F70 | public: virtual void __thiscall agiD3DTexDef::Set(class Vector2 &,class Vector2 &) | ?Set@agiD3DTexDef@@UAEXAAVVector2@@0@Z
    0x531F90 | public: virtual void __thiscall agiD3DTexDef::Restore(void) | ?Restore@agiD3DTexDef@@UAEXXZ
    0x531FD0 | public: virtual int __thiscall agiD3DTexLut::BeginGfx(void) | ?BeginGfx@agiD3DTexLut@@UAEHXZ
    0x532020 | public: virtual void __thiscall agiD3DTexLut::EndGfx(void) | ?EndGfx@agiD3DTexLut@@UAEXXZ
    0x532060 | public: virtual void * __thiscall agiD3DTexDef::`scalar deleting destructor'(unsigned int) | ??_GagiD3DTexDef@@UAEPAXI@Z
    0x532060 | public: virtual void * __thiscall agiD3DTexDef::`vector deleting destructor'(unsigned int) | ??_EagiD3DTexDef@@UAEPAXI@Z
    0x621090 | const agiD3DTexDef::`vftable' | ??_7agiD3DTexDef@@6B@
    0x795C38 | int DontCacheTextures | ?DontCacheTextures@@3HA
*/

#include "agi/texdef.h"

#include "d3dpipe.h"

class agiD3DTexDef final : public agiTexDef
{
public:
    // ??0agiD3DTexDef@@QAE@PAVagiPipeline@@@Z
    ARTS_EXPORT agiD3DTexDef(agiPipeline* pipe)
        : agiTexDef(pipe)
    {}

    // ??_EagiD3DTexDef@@UAEPAXI@Z
    // ??_GagiD3DTexDef@@UAEPAXI@Z
    // ??1agiD3DTexDef@@UAE@XZ
    ARTS_EXPORT ~agiD3DTexDef() override = default;

    // ?BeginGfx@agiD3DTexDef@@UAEHXZ
    ARTS_EXPORT i32 BeginGfx() override;

    // ?EndGfx@agiD3DTexDef@@UAEXXZ
    ARTS_EXPORT void EndGfx() override;

    // ?GetHandle@agiD3DTexDef@@QAEPAUIDirect3DTexture2@@H@Z
    ARTS_EXPORT IDirect3DTexture2* GetHandle(i32 stage);

    // ?IsAvailable@agiD3DTexDef@@UAEHXZ
    ARTS_EXPORT b32 IsAvailable() override;

    // ?Lock@agiD3DTexDef@@UAEHAAUagiTexLock@@@Z
    ARTS_EXPORT b32 Lock(agiTexLock& lock) override;

    // ?Request@agiD3DTexDef@@UAEXXZ
    ARTS_EXPORT void Request() override;

    // ?Restore@agiD3DTexDef@@UAEXXZ
    ARTS_EXPORT void Restore() override;

    // ?Set@agiD3DTexDef@@UAEXAAVVector2@@0@Z
    ARTS_EXPORT void Set(Vector2& arg1, Vector2& arg2) override;

    // ?Unlock@agiD3DTexDef@@UAEXAAUagiTexLock@@@Z
    ARTS_EXPORT void Unlock(agiTexLock& lock) override;

    // ?Prober@agiD3DTexDef@@SAXPAX@Z | agid3d:d3dpipe
    ARTS_EXPORT static void Prober(void* arg1);

    agiD3DPipeline* Pipe() const
    {
        return static_cast<agiD3DPipeline*>(agiRefreshable::Pipe());
    }

    i32 GetStage() const
    {
        return stage_;
    }

private:
    i32 probed_ {};
    IDirect3DTexture2* mem_tex_ {};
    IDirectDrawSurface4* mem_tex_surf_ {};
    i32 stage_ {};
    IUnknown* pal_ {};
};

check_size(agiD3DTexDef, 0x88);

class agiD3DTexLut final : public agiTexLut
{
public:
    agiD3DTexLut(agiPipeline* pipe)
        : agiTexLut(pipe)
    {}

    // ??_GagiD3DTexLut@@UAEPAXI@Z
    // ??_EagiD3DTexLut@@UAEPAXI@Z
    // ??1agiD3DTexLut@@UAE@XZ | inline
    ARTS_IMPORT ~agiD3DTexLut() override = default;

    // ?BeginGfx@agiD3DTexLut@@UAEHXZ
    ARTS_IMPORT i32 BeginGfx() override;

    // ?EndGfx@agiD3DTexLut@@UAEXXZ
    ARTS_IMPORT void EndGfx() override;

private:
    IDirectDrawPalette* pal_ {};
};

check_size(agiD3DTexLut, 0x420);

// ?DontCacheTextures@@3HA
ARTS_IMPORT extern i32 DontCacheTextures;
