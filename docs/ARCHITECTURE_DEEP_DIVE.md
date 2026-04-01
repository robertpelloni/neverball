# Architecture Deep Dive: Party Games, Stats, and Items

This document provides a comprehensive analysis of the recent architectural additions to the Neverball codebase, focusing on the implementation of Party Modes, Persistent Statistics, Achievements, and new Item Mechanics. It serves as a guide for future AI agents to understand the system design and extend functionality.

## 1. Persistent Statistics & Achievements System

### Overview
A robust system for tracking player metrics across sessions and unlocking achievements.

### Components
*   **`ball/profile.c`**: The core persistence layer.
    *   **Data Structure**: Uses a simple key-value pair text file (`USER_PROFILE_FILE`).
    *   **Storage**:
        *   `currency`: Total bananas collected.
        *   `world_%d`: Unlocked level counts per world.
        *   `stat_%d`: Generic integer statistics (e.g., `STAT_JUMPS`, `STAT_DISTANCE`).
        *   `achieve_%d`: Boolean flags for unlocked achievements.
    *   **API**:
        *   `profile_add_stat(id, value)`: Increments a stat and triggers `achieve_check()`.
        *   `profile_set_achieved(id)`: Sets an achievement flag and marks data as dirty.
*   **`ball/achieve.c`**: The achievement logic layer.
    *   **Definitions**: `ach_defs` array maps IDs (`ACH_FIRST_JUMP`) to names and descriptions.
    *   **Logic**: `achieve_check()` evaluates current stats against thresholds.
    *   **Trigger**: Called automatically whenever `profile_add_stat` modifies a relevant metric.
*   **`ball/hud.c`**: The notification layer.
    *   **Toast System**: `hud_show_toast(text)` displays a temporary popup at the top of the screen.
    *   **Integration**: `achieve_check()` calls `hud_show_toast()` upon unlocking an achievement.

### Data Flow
1.  **Event**: Player performs an action (e.g., jumps) in `ball/game_server.c`.
2.  **Tracking**: `game_server.c` calls `profile_add_stat(STAT_JUMPS, 1)`.
3.  **Persistence**: `profile.c` updates the in-memory stat value.
4.  **Check**: `profile.c` calls `achieve_check()`.
5.  **Unlock**: `achieve.c` verifies `STAT_JUMPS >= 100`.
6.  **Notification**: `achieve.c` calls `profile_set_achieved()` and `hud_show_toast()`.
7.  **Render**: `ball/hud.c` renders the toast overlay.
8.  **Save**: `ball/main.c` calls `profile_save()` on exit, writing to disk.

## 2. Party Mode Architecture

### Overview
The `st_party` state acts as a launcher for various game modes, configuring the `game_server` with specific rulesets.

### Components
*   **`ball/st_party.c`**: The Party Mode Menu.
    *   **State Machine**: Handles UI navigation (Mode, Players, CPU, Physics).
    *   **Configuration**: Sets global configs (`CONFIG_MULTIBALL`, `CONFIG_PHYSICS`) and game server parameters (`game_set_cpu_count`).
    *   **Launch**: Calls `progress_init(mode)` and `set_goto(0)` to load the "Easy" set as a placeholder environment.
*   **`ball/game_server.c`**: The Game Logic Core.
    *   **Mode Handling**: `game_mode` global determines the active ruleset (`MODE_TARGET`, `MODE_BATTLE`, `MODE_GOLF`, etc.).
    *   **Player Initialization**: `game_player_init` sets up player-specific state (e.g., `golf_strokes`, `bowling_pins`, `ammo`).
    *   **Step Function**: `game_step` dispatches to mode-specific logic:
        *   `game_golf_step`: Handles stroke power/aim.
        *   `game_bowling_step`: Manages pin physics and scoring.
        *   `game_boat_step`: Implements paddling physics.
        *   `game_shot_step`: Implements rail shooter mechanics.
*   **`ball/game_draw.c`**: Visual Representation.
    *   **Mode-Specific Rendering**: Checks `curr_mode()` to draw specific elements (e.g., bowling pins, boat models, crosshairs). *Note: Currently relies on ball primitives and HUD.*

### Key Mechanics
*   **Monkey Golf**: Uses a 3-stage state machine (Aim -> Power -> Roll). Physics are modified for high friction ("grass").
*   **Monkey Bowling**: Initializes 10 "pin" balls in a triangular formation. Collision logic detects pins moved from start position.
*   **Monkey Boat**: Maps left/right inputs to "paddles" that apply impulse and torque. Simultaneous input acts as a brake.
*   **Monkey Shot**: A rail shooter where the camera moves automatically. Player controls a cursor (`cursor_x`, `cursor_y`) to raycast against targets.

## 3. Character System

### Overview
Allows players to select different balls with unique physics attributes.

### Components
*   **`ball/stats.c`**: Stat Loading.
    *   **File Format**: Reads `stats.txt` from `data/ball/<name>/`.
    *   **Attributes**: `speed` (max velocity), `acceleration` (response time), `jump` (impulse force), `control` (traction/turning).
*   **`ball/st_char.c`**: Selection UI.
    *   **Preview**: Renders the selected ball model.
    *   **Unlocking**: Checks `profile_get_currency()` against character cost.
*   **Integration**:
    *   `game_server.c` loads stats into `player_stats[p]` during initialization.
    *   Physics logic uses these multipliers to adjust forces (e.g., `jump_force = 12.0f * stats.jump`).

## 4. Items & Mechanics

### Overview
New items (Powerups) and movement mechanics integrated into the existing physics engine.

### Components
*   **`share/solid_base.h`**: Item Definitions.
    *   Adds `ITEM_GYRO`, `ITEM_SHOCK`, `ITEM_SUPER_SPEED`, `ITEM_MISSILE`, `ITEM_BANANA`.
*   **`ball/game_server.c`**: Item Logic.
    *   **Collision**: `game_update_state` checks for item overlaps.
    *   **Effect Application**:
        *   **Gyro**: Sets `gyro_active` flag, enabling slow-fall physics in `game_step`.
        *   **Shock Absorber**: Sets `shock_active`, disabling bounce (restitution).
        *   **Super Speed**: Temporarily boosts `tilt.rx/rz` limits.
*   **`ball/hud.c`**: Feedback.
    *   **Timers**: Displays active powerup duration.
    *   **Inventory**: Shows held items for Battle/Race modes.

### New Mechanics
*   **Spin Dash**:
    *   **Input**: `game_server.c` tracks `dash` input state.
    *   **Charge**: Accumulates `spin_charge` over time.
    *   **Release**: Applies an impulse vector in the camera's forward direction.
*   **Active Jump**:
    *   **Logic**: Allows jumping only when grounded (`can_jump` flag set by velocity check).
    *   **Physics**: Applies instantaneous vertical velocity.

## 5. Future Extensibility

To add a new mode (e.g., **Monkey Tennis**):
1.  **Define**: Add `MODE_TENNIS` to `game_common.h`.
2.  **UI**: Add entry to `get_mode_label` and `get_mode_desc` in `st_party.c`.
3.  **State**: Add `game_tennis_step` in `game_server.c`.
4.  **Init**: Initialize rackets/ball in `game_player_init`.
5.  **Render**: Add racket rendering to `game_draw.c` or client-side logic.
