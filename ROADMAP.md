# Neverball to Super Monkey Ball Feature Parity Roadmap

This document outlines the definitive roadmap to bring Neverball to feature parity with the *Super Monkey Ball* series.

## Phase 1: Engine Foundation (Physics & Rendering)

**Objective:** Upgrade Neverball's core engine to support simultaneous multiplayer, ball-vs-ball interaction, and split-screen rendering.

### 1.1 Physics Engine Upgrade
*   **Ball-vs-Ball Collision:**
    *   Current Status: `sol_step` only checks collision against static geometry.
    *   Task: Implement dynamic circle-circle collision detection and response in `share/solid_sim_sol.c`.
    *   Requirement: Conservation of momentum calculations ($m_1v_1 + m_2v_2$).
*   **Parameterized Ball Stats:**
    *   Current Status: Physics constants are hardcoded or uniform.
    *   Task: Add `mass`, `acceleration`, `speed_limit`, `jump_power` to `struct v_ball` in `share/solid_vary.h`.
    *   Task: Update `sol_step` to utilize these per-ball parameters.
*   **Gliding & Pin Physics (Future Proofing):**
    *   Task: Add support for "Air State" (Gliding) physics (reduced gravity, air drag).
    *   Task: Investigate simple rigid body physics for "Pins" (Bowling).

### 1.2 Rendering Engine Upgrade
*   **Split-Screen Rendering:**
    *   Current Status: Single viewport assumed global.
    *   Task: Refactor `share/video.c` to support `video_push_persp_ex` taking arbitrary aspect ratios.
    *   Task: Refactor `ball/game_draw.c` to loop through active players/cameras and render to distinct viewport rectangles (`glViewport`).
*   **Camera System:**
    *   Task: Decouple `game_view` from a single global state. Create an array of views `game_view[MAX_PLAYERS]`.

### 1.3 Input System Upgrade
*   **Multi-Controller Support:**
    *   Current Status: `input_current` in `ball/game_server.c` is a single struct.
    *   Task: Arrayify `input_current` to `input_players[MAX_PLAYERS]`.
    *   Task: Map SDL Joystick indices to Player IDs in `ball/main.c`.

## Phase 2: Core Gameplay & Multiplayer

**Objective:** Enable local split-screen multiplayer for the main game (Race Mode).

### 2.1 Game Loop Refactor
*   **Multi-Ball Simulation:**
    *   Task: Ensure `ball/game_server.c` steps *all* active balls in the simulation loop, not just the primary one.
    *   Task: Handle win/loss states per player (e.g., Player 1 falls out, Player 2 keeps going).

### 2.2 Race Mode (Split-Screen)
*   **Feature:** 2-4 Player Split-screen race on standard Neverball levels.
*   **UI:** Add split-screen HUD (Timer, Bananas/Coins) for each viewport.

## Phase 3: Party Games Implementation

**Objective:** Implement the iconic Party Games.

### 3.1 Monkey Fight (Battle Mode)
*   **Description:** 4 Players knock each other off a platform.
*   **Requirements:**
    *   Ball-vs-Ball Physics (Phase 1.1).
    *   "Punch" mechanic: A temporary impulse/radius increase attached to the ball (boxing glove).
    *   Maps: Simple flat or multi-tiered arenas.

### 3.2 Monkey Target
*   **Description:** Roll down a ramp, jump, glide, and land on targets.
*   **Requirements:**
    *   Gliding Physics: "Open Ball" state toggles physics mode.
    *   Wind Mechanics: Global wind vector affecting flight.
    *   Score detection: Detect landing on specific "polygons" (Target rings).

### 3.3 Monkey Bowling
*   **Description:** Standard bowling.
*   **Requirements:**
    *   Pin Physics: Implementation of 10 simple rigid bodies (Pins) that can be knocked over.
    *   Lane Logic: Reset pins, track rounds.

### 3.4 Monkey Golf
*   **Status:** Already exists as **Neverputt**.
*   **Task:** Better integration (launch Neverputt from Neverball menu?).

## Phase 4: Content & Polish

### 4.1 UI Overhaul
*   **Party Game Menus:** Select game, select character (with stats).
*   **Results Screens:** Multiplayer scoreboards.

### 4.2 Assets
*   **Characters:** Distinct ball models.
*   **Levels:** Specific levels for Fight, Target, and Bowling.
