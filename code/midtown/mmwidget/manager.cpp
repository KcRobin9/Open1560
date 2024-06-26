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

define_dummy_symbol(mmwidget_manager);

#include "manager.h"

#include "agi/pipeline.h"
#include "arts7/camera.h"
#include "arts7/lamp.h"
#include "arts7/linear.h"
#include "arts7/view.h"
#include "eventq7/eventq.h"
#include "mmaudio/sound.h"
#include "mmeffects/card2d.h"

#include "menu.h"
#include "pointer.h"
#include "widget.h"

void MenuManager::AddPointer()
{}

void MenuManager::AdjustPopupCard(UIMenu* menu)
{
    f32 x, y, w, h;
    menu->GetDimensions(x, y, w, h);
    CheckBG(menu);
    popup_->SetDimensions(x, y, w, h);
}

#ifdef ARTS_DEV_BUILD
void MenuManager::AddWidgets(Bank* /*arg1*/)
{}
#endif

// ?HelpWatcher@@YGKPAX@Z
ARTS_IMPORT /*static*/ ulong ARTS_STDCALL HelpWatcher(void* arg1);

i32 MenuManager::GetPreviousMenu()
{
    if (UIMenu* menu = GetCurrentMenu())
        return menu->GetPreviousMenuID();

    return -1;
}

void MenuManager::GetScale(f32& x, f32& y, f32& width, f32& height)
{
    if (Is3D())
    {
        x = start_x_;
        y = start_y_;

        width = scale_x_;
        height = scale_y_;
    }
    else
    {
        x = 0.0f;
        y = 0.0f;

        width = 1.0f;
        height = 1.0f;
    }

    // TODO: Move scaling to MenuManager::Init?
    x = UI_StartX + (x * UI_ScaleX);
    y = UI_StartY + (y * UI_ScaleY);

    width = width * UI_ScaleX;
    height = height * UI_ScaleY;
}

void MenuManager::SetFocus(UIMenu* menu)
{
    active_menu_ = menu;
}

i32 MenuManager::Switch(i32 id)
{
    // The logic for updating popups is wierd/slightly broken.
    // If PUControls is opened and closed using F1, it locks up the menu system because the MenuManager is no longer being updated
    if (IsPopupOpen() || active_menu_id_ < 0)
    {
        SwitchNow(id);
    }
    else
    {
        next_active_menu_id_ = id;
    }

    return id;
}

void MenuManager::Update()
{
    ForceCurrentFocus();

    last_drawn_ = nullptr;

    asNode::Update();

    if (last_drawn_)
        last_drawn_->Update();

    pointer_->Update();
}

void MenuManager::SwitchNow(i32 id)
{
    if (id == active_menu_id_)
        return;

    bool had_previous = false;

    if (num_menus_ > 0 && active_menu_id_ >= 0)
    {
        had_previous = true;
        Disable(active_menu_id_);
    }

    Enable(id);
    active_menu_id_ = id;

    if (had_previous && !IsPopupOpen())
        PlayMenuSwitchSound();

    if (had_previous || IsPopupOpen())
    {
        if (active_menu_)
        {
            active_menu_->ClearAction();
            active_menu_->ClearWidgets();
        }

        SetFocus(GetCurrentMenu());

        active_menu_->SetSelected();
    }
}

void MenuManager::ForceCurrentFocus()
{
    if (next_active_menu_id_ != -1)
    {
        SwitchNow(next_active_menu_id_);
        next_active_menu_id_ = -1;
    }
}

UIMenu* MenuManager::GetCurrentMenu()
{
    i32 index = FindMenu(active_menu_id_);

    return (index >= 0) ? menus_[index] : nullptr;
}

void MenuManager::Enable(i32 id)
{
    i32 index = FindMenu(id);

    if (index < 0)
        return;

    active_menu_id_ = id;

    if (!Is3D() || IsPopupOpen())
    {
        menus_[index]->Enable();

        if (Is3D())
            AdjustPopupCard(menus_[index]);

        CheckBG(menus_[index]);
    }
}

i32 MenuManager::CurrentMenuSelected()
{
    if (dialog_menu_)
        return dialog_menu_->GetMenuID();

    return active_menu_id_;
}

void MenuManager::Disable(i32 id)
{
    if (id == -1)
        id = active_menu_id_;

    if (i32 index = FindMenu(id); index >= 0)
        menus_[index]->Disable();
}

void MenuManager::Kill()
{
    event_q_ = nullptr;
    menu_cs_ = nullptr;
    lamps_ = nullptr;
    lcss_ = nullptr;

    move_selector_sound_ = nullptr;
    selection_made_sound_ = nullptr;
    switch_sound_ = nullptr;
    menu_camera_ = nullptr;
}

i32 MenuManager::MenuState(i32 menu)
{
    if (dialog_menu_)
        return dialog_menu_->GetState();

    if (menu == -1)
        menu = active_menu_id_;

    if (i32 index = FindMenu(menu); index >= 0)
        return menus_[index]->GetState();

    return 0;
}

hook_func(INIT_main, [] {
    for (usize addr : {
             0x4086B4,
             0x4086D7,
             0x40870A,
             0x408722,
         })
    {
        create_hook("mmInterface::ShowMain", "Menu Switching", addr, &MenuManager::Switch, hook_type::call);
    }
});
