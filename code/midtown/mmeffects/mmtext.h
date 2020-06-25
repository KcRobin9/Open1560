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
    mmeffects:mmtext

    0x4FEE30 | void __cdecl ddGDIFlip(void) | ?ddGDIFlip@@YAXXZ
    0x4FEE40 | public: __thiscall mmText::mmText(void) | ??0mmText@@QAE@XZ
    0x4FEE50 | public: __thiscall mmText::~mmText(void) | ??1mmText@@QAE@XZ
    0x4FEE60 | public: static void * __cdecl mmText::CreateLocFont(struct LocString *,int) | ?CreateLocFont@mmText@@SAPAXPAULocString@@H@Z
    0x4FEF30 | public: static void * __cdecl mmText::CreateFont(char *,int) | ?CreateFont@mmText@@SAPAXPADH@Z
    0x4FEF60 | public: static void __cdecl mmText::DeleteFont(void *) | ?DeleteFont@mmText@@SAXPAX@Z
    0x4FEF70 | public: static void * __cdecl mmText::GetDC(class agiSurfaceDesc *) | ?GetDC@mmText@@SAPAXPAVagiSurfaceDesc@@@Z
    0x4FF010 | public: static void __cdecl mmText::ReleaseDC(void) | ?ReleaseDC@mmText@@SAXXZ
    0x4FF0C0 | public: static class agiBitmap * __cdecl mmText::CreateFitBitmap(char *,void *,int,int) | ?CreateFitBitmap@mmText@@SAPAVagiBitmap@@PADPAXHH@Z
    0x4FF2D0 | public: void __thiscall mmText::Draw(class agiSurfaceDesc *,float,float,char *,void *) | ?Draw@mmText@@QAEXPAVagiSurfaceDesc@@MMPADPAX@Z
    0x4FF360 | public: void __thiscall mmText::Draw2(class agiSurfaceDesc *,float,float,char *,void *,unsigned int) | ?Draw2@mmText@@QAEXPAVagiSurfaceDesc@@MMPADPAXI@Z
    0x4FF450 | public: __thiscall mmTextNode::mmTextNode(void) | ??0mmTextNode@@QAE@XZ
    0x4FF4D0 | public: virtual __thiscall mmTextNode::~mmTextNode(void) | ??1mmTextNode@@UAE@XZ
    0x4FF540 | public: void __thiscall mmTextNode::Init(float,float,float,float,int,int) | ?Init@mmTextNode@@QAEXMMMMHH@Z
    0x4FF690 | public: void __thiscall mmTextNode::SetBGColor(class Vector4 &) | ?SetBGColor@mmTextNode@@QAEXAAVVector4@@@Z
    0x4FF6F0 | public: void __thiscall mmTextNode::SetFGColor(class Vector4 &) | ?SetFGColor@mmTextNode@@QAEXAAVVector4@@@Z
    0x4FF750 | public: unsigned int __thiscall mmTextNode::GetFGColor(void) | ?GetFGColor@mmTextNode@@QAEIXZ
    0x4FF760 | public: void __thiscall mmTextNode::SetEffects(int,int) | ?SetEffects@mmTextNode@@QAEXHH@Z
    0x4FF790 | public: void __thiscall mmTextNode::GetTextDimensions(void *,struct LocString *,float &,float &) | ?GetTextDimensions@mmTextNode@@QAEXPAXPAULocString@@AAM2@Z
    0x4FF8D0 | public: void __thiscall mmTextNode::SetTextPosition(int,float,float) | ?SetTextPosition@mmTextNode@@QAEXHMM@Z
    0x4FF930 | public: int __thiscall mmTextNode::AddText(void *,struct LocString *,int,float,float) | ?AddText@mmTextNode@@QAEHPAXPAULocString@@HMM@Z
    0x4FFA40 | public: void __thiscall mmTextNode::SetString(int,struct LocString *) | ?SetString@mmTextNode@@QAEXHPAULocString@@@Z
    0x4FFAA0 | public: void __thiscall mmTextNode::SetPosition(int,float,float) | ?SetPosition@mmTextNode@@QAEXHMM@Z
    0x4FFB00 | public: void __cdecl mmTextNode::Printf(char const *,...) | ?Printf@mmTextNode@@QAAXPBDZZ
    0x4FFB30 | public: virtual void __thiscall mmTextNode::Update(void) | ?Update@mmTextNode@@UAEXXZ
    0x4FFB60 | public: void __thiscall mmTextNode::RenderText(class agiSurfaceDesc *,struct mmTextData *,int,unsigned int) | ?RenderText@mmTextNode@@QAEXPAVagiSurfaceDesc@@PAUmmTextData@@HI@Z
    0x4FFD90 | public: virtual void __thiscall mmTextNode::Cull(void) | ?Cull@mmTextNode@@UAEXXZ
    0x4FFE70 | public: virtual void * __thiscall mmTextNode::`scalar deleting destructor'(unsigned int) | ??_GmmTextNode@@UAEPAXI@Z
    0x620658 | const mmTextNode::`vftable' | ??_7mmTextNode@@6B@
*/

#include "arts7/node.h"

struct mmText
{
public:
    // 0x4FEE40 | ??0mmText@@QAE@XZ
    ARTS_IMPORT mmText();

    // 0x4FEE50 | ??1mmText@@QAE@XZ
    ARTS_IMPORT ~mmText();

    // 0x4FF2D0 | ?Draw@mmText@@QAEXPAVagiSurfaceDesc@@MMPADPAX@Z | unused
    ARTS_IMPORT void Draw(class agiSurfaceDesc* arg1, f32 arg2, f32 arg3, char* arg4, void* arg5);

    // 0x4FF360 | ?Draw2@mmText@@QAEXPAVagiSurfaceDesc@@MMPADPAXI@Z
    ARTS_IMPORT void Draw2(class agiSurfaceDesc* arg1, f32 arg2, f32 arg3, char* arg4, void* arg5, u32 arg6);

    // 0x4FF0C0 | ?CreateFitBitmap@mmText@@SAPAVagiBitmap@@PADPAXHH@Z
    ARTS_IMPORT static class agiBitmap* CreateFitBitmap(char* arg1, void* arg2, i32 arg3, i32 arg4);

    // 0x4FEF30 | ?CreateFont@mmText@@SAPAXPADH@Z
    ARTS_IMPORT static void* CreateFont(char* arg1, i32 arg2);

    // 0x4FEE60 | ?CreateLocFont@mmText@@SAPAXPAULocString@@H@Z
    ARTS_IMPORT static void* CreateLocFont(struct LocString* arg1, i32 arg2);

    // 0x4FEF60 | ?DeleteFont@mmText@@SAXPAX@Z
    ARTS_IMPORT static void DeleteFont(void* arg1);

    // 0x4FEF70 | ?GetDC@mmText@@SAPAXPAVagiSurfaceDesc@@@Z
    ARTS_IMPORT static void* GetDC(class agiSurfaceDesc* arg1);

    // 0x4FF010 | ?ReleaseDC@mmText@@SAXXZ
    ARTS_IMPORT static void ReleaseDC();

private:
    u8 byte0 {1};
    u8 byte1 {0};
};

check_size(mmText, 0x2);

class mmTextNode : public asNode
{
    // const mmTextNode::`vftable' @ 0x620658

public:
    // 0x4FF450 | ??0mmTextNode@@QAE@XZ
    ARTS_IMPORT mmTextNode();

    // 0x426300 | ??_EmmTextNode@@UAEPAXI@Z
    // 0x4FFE70 | ??_GmmTextNode@@UAEPAXI@Z | unused
    // 0x4FF4D0 | ??1mmTextNode@@UAE@XZ
    ARTS_IMPORT ~mmTextNode() override;

    // 0x4FF930 | ?AddText@mmTextNode@@QAEHPAXPAULocString@@HMM@Z
    ARTS_IMPORT i32 AddText(void* arg1, struct LocString* arg2, i32 arg3, f32 arg4, f32 arg5);

    // 0x4FFD90 | ?Cull@mmTextNode@@UAEXXZ
    ARTS_IMPORT void Cull() override;

    // 0x4FF750 | ?GetFGColor@mmTextNode@@QAEIXZ
    ARTS_IMPORT u32 GetFGColor();

    // 0x4FF790 | ?GetTextDimensions@mmTextNode@@QAEXPAXPAULocString@@AAM2@Z
    ARTS_IMPORT void GetTextDimensions(void* arg1, struct LocString* arg2, f32& arg3, f32& arg4);

    // 0x4FF540 | ?Init@mmTextNode@@QAEXMMMMHH@Z
    ARTS_IMPORT void Init(f32 arg1, f32 arg2, f32 arg3, f32 arg4, i32 arg5, i32 arg6);

    // 0x4FFB00 | ?Printf@mmTextNode@@QAAXPBDZZ | unused
    ARTS_IMPORT void Printf(char const* arg1, ...);

    // 0x4FFB60 | ?RenderText@mmTextNode@@QAEXPAVagiSurfaceDesc@@PAUmmTextData@@HI@Z
    ARTS_IMPORT void RenderText(class agiSurfaceDesc* arg1, struct mmTextData* arg2, i32 arg3, u32 arg4);

    // 0x4FF690 | ?SetBGColor@mmTextNode@@QAEXAAVVector4@@@Z | unused
    ARTS_IMPORT void SetBGColor(class Vector4& arg1);

    // 0x4FF760 | ?SetEffects@mmTextNode@@QAEXHH@Z
    ARTS_IMPORT void SetEffects(i32 arg1, i32 arg2);

    // 0x4FF6F0 | ?SetFGColor@mmTextNode@@QAEXAAVVector4@@@Z
    ARTS_IMPORT void SetFGColor(class Vector4& arg1);

    // 0x4FFAA0 | ?SetPosition@mmTextNode@@QAEXHMM@Z
    ARTS_IMPORT void SetPosition(i32 arg1, f32 arg2, f32 arg3);

    // 0x4FFA40 | ?SetString@mmTextNode@@QAEXHPAULocString@@@Z
    ARTS_IMPORT void SetString(i32 arg1, struct LocString* arg2);

    // 0x4FF8D0 | ?SetTextPosition@mmTextNode@@QAEXHMM@Z
    ARTS_IMPORT void SetTextPosition(i32 arg1, f32 arg2, f32 arg3);

    // 0x4FFB30 | ?Update@mmTextNode@@UAEXXZ
    ARTS_IMPORT void Update() override;

private:
    f32 x_ {0.0f};
    f32 y_ {0.0f};
    i32 line_count_ {0};
    i32 max_lines_ {0};
    u32 enabled_lines_ {0xFFFFFFFF};
    mmText int30_ {};
    mmTextData* lines_ {nullptr};
    agiBitmap* text_bitmap_ {nullptr};
    u32 touched_ {1};
    u32 hidden_ {0};
    u32 dword44_ {0};
    u32 fg_color_ {0xFFFFFF};
    u32 bg_color_ {0};
};

check_size(mmTextNode, 0x54);

// 0x4FEE30 | ?ddGDIFlip@@YAXXZ | unused
ARTS_IMPORT void ddGDIFlip();
