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

#include "mmwidget/menu.h"
#include "mmcityinfo/racedata.h"
#include "mmcityinfo/state.h"
#include "mmeffects/mmtext.h"

class RaceMenuBase : public UIMenu
{
public:
    // ??0RaceMenuBase@@QAE@H@Z
    ARTS_IMPORT RaceMenuBase(i32 arg1);

    // ??1RaceMenuBase@@UAE@XZ
    ARTS_IMPORT ~RaceMenuBase() override;

    // ?UsesLobby@RaceMenuBase@@UAEXH@Z | inline
    ARTS_EXPORT virtual void UsesLobby(i32 arg1);

    // ?AICallback@RaceMenuBase@@QAEXXZ
    ARTS_IMPORT void AICallback();

    // ?ChangeLocalVals@RaceMenuBase@@QAEXXZ
    ARTS_IMPORT void ChangeLocalVals();

    // ?CityChange@RaceMenuBase@@QAEXXZ
    ARTS_IMPORT void CityChange();

    // ?FocusDescription@RaceMenuBase@@QAEXH@Z
    ARTS_IMPORT void FocusDescription(i32 arg1);

    // ?GameCallback@RaceMenuBase@@QAEXXZ
    ARTS_IMPORT void GameCallback();

    // ?Init@RaceMenuBase@@QAEXH@Z
    ARTS_EXPORT void Init(i32 arg1);

    // ?IsEnvEnabled@RaceMenuBase@@QAEHXZ
    ARTS_IMPORT i32 IsEnvEnabled();

    // ?LapsCallback@RaceMenuBase@@QAEXXZ
    ARTS_IMPORT void LapsCallback();

    // ?SetRW@RaceMenuBase@@QAEXXZ
    ARTS_IMPORT void SetRW();

    // ?SetStateRace@RaceMenuBase@@QAEXXZ
    ARTS_IMPORT void SetStateRace();

    // ?SyncRaceState@RaceMenuBase@@QAEXXZ
    ARTS_IMPORT void SyncRaceState();

    // ?WPCallback@RaceMenuBase@@QAEXXZ
    ARTS_IMPORT void WPCallback();

    // ?WidgetOnOff@RaceMenuBase@@QAEXHPAVuiWidget@@@Z
    ARTS_IMPORT void WidgetOnOff(i32 arg1, uiWidget* arg2);

    // ========================================================================
    // MEMBER VARIABLES
    // ========================================================================

    // Race configuration
    b32 IsMultiplayer; // 0x90
    i32 GameMode;      // 0x94 - Current game mode (Blitz/Circuit/Checkpoint/Roam/CnR)
    i32 RaceState;     // 0x98 # was: field_98
    i32 field_9C;      // 0x9C - Unknown/unused
    i32 MaxOpponents;  // 0xA0 - Number of AI opponents (single player)
    i32 MaxPlayers;    // 0xA4 # was: dwordA4 - Max players (multiplayer)

    mmTimeOfDay TimeOfDay; // 0xA8
    mmWeather Weather;     // 0xAC

    i32 NumLaps;         // 0xB0 - Number of laps/checkpoints
    i32 MenuInitialized; // 0xB4 # was: field_B4 - Menu state flag
    u32 UnlockedMask;    // 0xB8 - Bitmask of unlocked races

    // Multiplayer data buffer (includes password at offset 12)
    u8 MultiplayerData[0x34]; // 0xBC - 0xEF (52 bytes) # was: gap_BC

    Callback OnChangeCallback; // 0xF0 (20 bytes) # was: Callback

    // Widget pointers - Race selection
    uiWidget* RaceDropdown; // 0x104 - Race name dropdown selector

    // Widget pointers - Race config rollers
    uiWidget* LapsRoller;        // 0x108 # was: dword108 - Laps text roller
    uiWidget* CheckpointsRoller; // 0x10C # was: dword10C - Checkpoints text roller
    uiWidget* OpponentsRoller;   // 0x110 # was: dword110 - Opponents text roller (SP only)

    // Mode button array
    UIBMButton** ModeButtons; // 0x114 # was: field_114 - Array of 5 game mode buttons
    i32 field_118;            // 0x118 - Unknown/unused

    // Environment widgets
    uiWidget* EnvButton;          // 0x11C # was: field_11C - Environment settings button
    UIBMButton* CityChangeButton; // 0x120 # was: field_120 - City/environment change button
    i32 field_124;                // 0x124 - Unknown/unused
    i32 field_128;                // 0x128 - Unknown/unused

    // Race name icon
    uiWidget* RaceNameIcon; // 0x12C # was: field_12C - Icon for race dropdown

    // Density sliders (single player)
    UISlider* TrafficDensitySlider; // 0x130 # was: dword130
    UISlider* CopDensitySlider;     // 0x134 # was: dword134
    UISlider* PedDensitySlider;     // 0x138 # was: field_138

    uiWidget* dword13C; // 0x13C - Unknown/unused
    uiWidget* dword140; // 0x140 - Unknown/unused

    // Race data containers
    mmRaceData RaceData;    // 0x144 (0x4B4 bytes)
    mmRaceData BlitzData;   // 0x5F8 (0x4B4 bytes)
    mmRaceData CircuitData; // 0xAAC (0x4B4 bytes)

    // Race description text node
    mmTextNode* RaceDescriptionText; // 0xF60 # was: field_F60

    // Bracket icons for laps roller
    UIIcon* LapsLeftBracket;  // 0xF64 # was: field_F64
    UIIcon* LapsRightBracket; // 0xF68 # was: field_F68

    // Bracket icons for checkpoints roller
    UIIcon* CheckpointsLeftBracket;  // 0xF6C # was: field_F6C
    UIIcon* CheckpointsRightBracket; // 0xF70 # was: field_F70

    // Bracket icons for opponents roller
    UIIcon* OpponentsLeftBracket;  // 0xF74 # was: field_F74
    UIIcon* OpponentsRightBracket; // 0xF78 # was: field_F78

    // Bracket icons for race dropdown
    UIIcon* RaceDropdownLeftBracket;  // 0xF7C # was: field_F7C
    UIIcon* RaceDropdownRightBracket; // 0xF80 # was: field_F80

    // Icon widgets for rollers
    UIIcon* OpponentsIcon;   // 0xF84 # was: field_F84
    UIIcon* LapsIcon;        // 0xF88 # was: field_F88
    UIIcon* CheckpointsIcon; // 0xF8C # was: field_F8C
};

check_size(RaceMenuBase, 0xF90);

// ?GoldDamageScale@@3MA
ARTS_IMPORT extern f32 GoldDamageScale;

// ?GoldHPScale@@3MA
ARTS_IMPORT extern f32 GoldHPScale;