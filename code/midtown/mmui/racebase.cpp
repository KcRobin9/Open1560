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

#include "racebase.h"

#include "agi/pipeline.h"
#include "data7/str.h"
#include "data7/utimer.h"
#include "localize/localize.h"
#include "mmcityinfo/citylist.h"
#include "mmcityinfo/racedata.h"
#include "mmcityinfo/state.h"
#include "mmeffects/mmtext.h"
#include "mmwidget/bm_button.h"
#include "mmwidget/bm_label.h"
#include "mmwidget/icon.h"
#include "mmwidget/manager.h"
#include "mmwidget/slider.h"
#include "mmwidget/textfield.h"
#include "mmwidget/textroller2.h"

define_dummy_symbol(mmui_racebase);

void RaceMenuBase::UsesLobby(i32 /*arg1*/)
{}

void RaceMenuBase::Init(i32 is_multiplayer)
{
    // ========================================================================
    // INITIALIZATION
    // ========================================================================

    IsMultiplayer = (is_multiplayer != 0);

    // Initialize race configuration to defaults
    RaceState = 0;
    TimeOfDay = mmTimeOfDay::Morning;
    Weather = mmWeather::Sun;
    NumLaps = 3;
    MaxOpponents = 0;
    OpponentsRoller = nullptr;

    // Allocate button array (Blitz, Circuit, Checkpoint, Roam, Cops&Robbers)
    constexpr i32 max_mode_buttons = 5;
    ModeButtons = new UIBMButton*[max_mode_buttons];

    // ========================================================================
    // LAYOUT CONFIGURATION
    // ========================================================================

    const f32 left_x = UIMenu::UI_LEFT_MARGIN;
    const f32 widget_w = UIMenu::WIDGET_WIDTH;
    const f32 widget_h = UIMenu::WIDGET_HEIGHT;
    const f32 right_x = left_x + widget_w + 0.1f;

    // ========================================================================
    // GAME MODE SELECTION BUTTONS
    // ========================================================================

    // Create game mode callback
    Callback game_callback([this]() { GameCallback(); });

    f32 button_y = 0.05f;

    // Blitz mode
    {
        Callback focus_callback([this]() { FocusDescription(0); });

        ModeButtons[0] =
            AddBMButton(-1, "race_blitz"_xconst, left_x, button_y, 6, game_callback, &GameMode, 0, 0, focus_callback);
    }

    // Calculate vertical spacing between buttons
    const f32 button_spacing = static_cast<f32>(ModeButtons[0]->Height) / static_cast<f32>(Pipe()->GetHeight()) + 0.05f;
    button_y += button_spacing;

    // Circuit mode
    {
        Callback focus_callback([this]() { FocusDescription(1); });

        ModeButtons[1] =
            AddBMButton(-1, "race_circ"_xconst, left_x, button_y, 6, game_callback, &GameMode, 1, 0, focus_callback);
    }
    button_y += button_spacing;

    // Checkpoint mode
    {
        Callback focus_callback([this]() { FocusDescription(2); });

        ModeButtons[2] =
            AddBMButton(-1, "race_waypt"_xconst, left_x, button_y, 6, game_callback, &GameMode, 2, 0, focus_callback);
    }
    button_y += button_spacing;

    SetFocusWidget(-1);

    // Cruise/Roam mode
    {
        Callback focus_callback([this]() { FocusDescription(3); });

        ModeButtons[3] =
            AddBMButton(-1, "race_roam"_xconst, left_x, button_y, 6, game_callback, &GameMode, 3, 0, focus_callback);
    }
    button_y += button_spacing;

    // Cops & Robbers mode (multiplayer only)
    if (IsMultiplayer)
    {
        Callback focus_callback([this]() { FocusDescription(4); });

        ModeButtons[4] =
            AddBMButton(-1, "race_cops"_xconst, left_x, button_y, 6, game_callback, &GameMode, 4, 0, focus_callback);
    }

    // ========================================================================
    // RACE CONFIGURATION WIDGETS (Left Column)
    // ========================================================================

    f32 config_y = button_y + 0.1f;

    // Laps text roller
    {
        Callback laps_callback([this]() { LapsCallback(); });

        LapsRoller = AddTextRoller2(2, LOC_TEXT("LAPS"), &NumLaps, left_x, config_y, widget_w, widget_h,
            MenuMgr()->string8C, 0, 10, -1, MenuMgr()->font_height_1, laps_callback);

        // Add icon
        LapsIcon = new UIIcon();
        LapsRoller->AddChild(LapsIcon);

        aconst char* icon_name = IsMultiplayer ? "host_laps"_xconst : "race_laps"_xconst;
        const f32 icon_x = IsMultiplayer ? (LapsRoller->X - 0.02f) : (LapsRoller->X - 0.20625f);
        const f32 icon_y = IsMultiplayer ? (LapsRoller->Y - 0.03125f) : (LapsRoller->Y + 0.010416667f);

        LapsIcon->Init(const_cast<char*>(icon_name), icon_x, icon_y);

        MenuMgr()->AddBrackets(LapsLeftBracket, LapsRightBracket, LapsRoller, 0.02f, 0.0f);
    }

    // Checkpoints text roller
    {
        Callback wp_callback([this]() { WPCallback(); });

        CheckpointsRoller = AddTextRoller2(3, LOC_TEXT("CHECKPOINTS"), &NumLaps, left_x, config_y, widget_w, widget_h,
            MenuMgr()->string8C, 0, 10, -1, MenuMgr()->font_height_1, wp_callback);

        // Add icon
        CheckpointsIcon = new UIIcon();
        CheckpointsRoller->AddChild(CheckpointsIcon);

        aconst char* icon_name = IsMultiplayer ? "host_checkpoints"_xconst : "race_checkpoints"_xconst;
        const f32 icon_x = IsMultiplayer ? (CheckpointsRoller->X - 0.02f) : (CheckpointsRoller->X - 0.20625f);
        const f32 icon_y = IsMultiplayer ? (CheckpointsRoller->Y - 0.03125f) : (CheckpointsRoller->Y + 0.010416667f);

        CheckpointsIcon->Init(const_cast<char*>(icon_name), icon_x, icon_y);

        MenuMgr()->AddBrackets(CheckpointsLeftBracket, CheckpointsRightBracket, CheckpointsRoller, 0.02f, 0.0f);
    }

    // Opponents text roller (single player only)
    if (!IsMultiplayer)
    {
        Callback ai_callback([this]() { AICallback(); });

        const f32 opp_x = left_x + widget_w;
        const f32 opp_y = LapsRoller->Y + config_y;
        const f32 opp_w = widget_w * 0.33333334f;

        OpponentsRoller = AddTextRoller2(4, LOC_TEXT("OPPONENTS"), &MaxOpponents, opp_x, opp_y, opp_w, widget_h,
            MenuMgr()->string94, 0, 10, -1, MenuMgr()->font_height_1, ai_callback);

        // Add icon
        OpponentsIcon = new UIIcon();
        OpponentsRoller->AddChild(OpponentsIcon);

        const f32 icon_x = OpponentsRoller->X - 0.20625f;
        const f32 icon_y = OpponentsRoller->Y + 0.010416667f;
        OpponentsIcon->Init(const_cast<char*>("race_oppo"_xconst), icon_x, icon_y);

        MenuMgr()->AddBrackets(OpponentsLeftBracket, OpponentsRightBracket, OpponentsRoller, 0.02f, 0.0f);
    }

    // ========================================================================
    // RACE SELECTION (Right Column)
    // ========================================================================

    // Race name dropdown
    {
        Callback race_callback([this]() { SetStateRace(); });

        aconst char* frame_icon = IsMultiplayer ? "host_drop_frame"_xconst : "race_drop_frame"_xconst;

        RaceDropdown = reinterpret_cast<uiWidget*>(
            AddTextDropdown(0, LOC_TEXT("RACE NAME"), &MMSTATE.EventId, right_x, 0.1f, widget_w, widget_h,
                MenuMgr()->checkpoint_names, 0, -1, 24, race_callback, const_cast<char*>(frame_icon)));

        // Add icon
        UIIcon* race_icon = new UIIcon();
        RaceNameIcon = race_icon;
        RaceDropdown->AddChild(race_icon);

        const f32 icon_x = RaceDropdown->X - 0.021875f;
        const f32 icon_y = RaceDropdown->Y - 0.043749999f;
        race_icon->Init(const_cast<char*>("race_name"_xconst), icon_x, icon_y);

        MenuMgr()->AddBrackets(RaceDropdownLeftBracket, RaceDropdownRightBracket, RaceDropdown, 0.02f, 0.0f);
    }

    // Race description text display
    {
        RaceDescriptionText = new mmTextNode();

        const f32 text_h = widget_h * 4.0f;
        RaceDescriptionText->Init(right_x, 0.2f, widget_w, text_h, 1, 1);

        // void* font = MenuMgr()->GetFont(16);
        // const char* desc_text_ptr = reinterpret_cast<const char*>(dword_705910);
        // RaceDescriptionText->AddText(font, LOC_TEXT(desc_text_ptr), 48, 0.0f, 0.0f);
        // RaceDescriptionText->AddText(font, &"hi"_xconst, 48, 0.0f, 0.0f);
    }

    // ========================================================================
    // ENVIRONMENT SELECTION
    // ========================================================================

    // Environment button with tooltip
    {
        UIBMButton* env_button = AddBMButton(7, "race_env"_xconst, 0.8f, 0.4f, 4, nullptr, nullptr, -1, 0, nullptr);
        EnvButton = env_button;
        env_button->AddToolTip(this, LOC_TEXT("This is a test"));
    }

    // Time of day icon selector
    {
        string tod_icons;
        tod_icons.Init(150);
        arts_strcpy(tod_icons.get(), 150, "race_imorn|race_inoon|race_isuns|race_inite"_xconst);

        AddBMLabel(-1, const_cast<char*>("TOD Icons"_xconst), &tod_icons, 0.5f, 0.35f,
            reinterpret_cast<i32*>(&MMSTATE.TimeOfDay));
    }

    // Weather icon selector
    {
        string weather_icons;
        weather_icons.Init(150);
        arts_strcpy(weather_icons.get(), 150, "race_isun|race_ifog|race_irain|race_isnow"_xconst);

        AddBMLabel(-1, const_cast<char*>("Weather Icons"_xconst), &weather_icons, 0.5f, 0.5f,
            reinterpret_cast<i32*>(&MMSTATE.Weather));
    }

    // ========================================================================
    // DENSITY SLIDERS
    // ========================================================================

    f32 slider_y = 0.6f;

    // Traffic and cop density (single player only)
    if (!IsMultiplayer)
    {
        // Traffic density
        TrafficDensitySlider = AddSlider(9, LOC_TEXT("TRAFFIC DENSITY"), &MMSTATE.AmbientDensity, 0.4f, slider_y,
            widget_w, widget_h, 0.0f, 1.0f, 11, 0, 20, 0, nullptr);
        TrafficDensitySlider->SetReadWrite(0);

        slider_y += 0.1f;

        // Cop density
        CopDensitySlider = AddSlider(10, LOC_TEXT("COP DENSITY"), &MMSTATE.CopDensity, 0.4f, slider_y, widget_w,
            widget_h, 0.0f, 1.0f, 11, 0, 20, 0, nullptr);
        CopDensitySlider->SetReadWrite(0);

        slider_y += 0.1f;
    }

    // Pedestrian density (always visible)
    PedDensitySlider = AddSlider(11, LOC_TEXT("PEDESTRIAN DENSITY"), &MMSTATE.PedDensity, 0.4f, slider_y, widget_w,
        widget_h, 0.0f, 1.0f, 11, 0, 20, 0, nullptr);
    PedDensitySlider->SetReadWrite(0);

    slider_y += 0.1f;

    // ========================================================================
    // MULTIPLAYER-SPECIFIC CONTROLS
    // ========================================================================

    if (IsMultiplayer)
    {
        // Password field (stored at offset 12 in MultiplayerData buffer)
        char* password_buffer = reinterpret_cast<char*>(&MultiplayerData[12]);
        AddTextField(
            -1, LOC_TEXT("Password"), password_buffer, 0.4f, slider_y, widget_w, widget_h, 8, 1, 183, 24, 0, nullptr);

        slider_y += 0.2f;

        // Max players
        AddTextRoller2(-1, LOC_TEXT("Max Players"), &MaxPlayers, 0.4f, slider_y, widget_w, widget_h,
            MenuMgr()->string94, 0, 10, -1, MenuMgr()->font_height_1, nullptr);
    }

    // ========================================================================
    // GAME MODE DESCRIPTION ICONS
    // ========================================================================

    {
        string mode_desc_icons;
        mode_desc_icons.Init(100);
        arts_strcpy(mode_desc_icons.get(), 100, "race_tblitz|race_tcirc|race_twaypt|race_troam|race_tcops"_xconst);

        AddBMLabel(-1, const_cast<char*>("race desc icons"_xconst), &mode_desc_icons, 0.4844f, 0.25f, &GameMode);
    }

    // ========================================================================
    // CITY/ENVIRONMENT CHANGE BUTTON
    // ========================================================================

    {
        const f32 env_button_y = slider_y + button_spacing;

        CityChangeButton =
            AddBMButton(8, "race_cenv"_xconst, left_x, env_button_y, 4, nullptr, nullptr, -1, 0, nullptr);

        // Only allow city changes if multiple cities are available
        if (CityListPtr->NumCities < 2)
            CityChangeButton->Kill();
    }

    // ========================================================================
    // FINALIZATION
    // ========================================================================

    // Initialize menu state
    MenuInitialized = 0;
    SetBstate(0);

    // Disable unavailable race modes
    if (!BlitzData.GetNumRaces())
        ModeButtons[0]->Kill();

    if (!CircuitData.GetNumRaces())
        ModeButtons[1]->Kill();

    // Synchronize local values with global state
    ChangeLocalVals();
    CityChange();
}