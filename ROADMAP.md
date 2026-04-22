# Super Monkey Ball Feature Parity Roadmap

## Executive Summary
This document outlines a definitive roadmap to bring the *Neverball* project to feature parity with the *Super Monkey Ball (SMB)* series. The goal is to evolve the Neverball engine to support the beloved gameplay modes, physics nuances, and extensive party game library of the SMB franchise, while preserving Neverball's open-source legacy.

The roadmap is divided into four phases, prioritizing the "Golden Era" (SMB 1 & 2) features first, followed by the vast content of the "Party Game" era, and finally the "Modern" mechanics (Jump, Stats, Online).

## Feature Gap Analysis

| Feature Category | Super Monkey Ball Series (Target) | Current Neverball (Status) |
| :--- | :--- | :--- |
| **Physics** | "Snappy", high-friction, instant acceleration. | "Heavy", momentum-based, inertial. |
| **Camera** | Auto-follow, fixed angle options, "snap" behind. | Loose follow, manual rotation focus. |
| **Main Game** | Timer-based, Bananas = Lives, Bonus Stages. | Timer-based, Coins = Score/Life, no Bonus Stages. |
| **Story Mode** | Cutscenes, World Map, Hub Worlds (SMB2). | Linear level progression only. |
| **Characters** | Distinct Stats (Speed, Weight, Accel, Jump). | Uniform physics (cosmetic differences only). |
| **Mechanics** | Tilt (Classic), Jump (Blitz/Mania), Spin Dash (Rumble). | Tilt only. |
| **Party Games** | 50+ Minigames (Race, Fight, Target, Bowling, Golf, etc.). | Basic Race Mode (In Progress). |
| **Multiplayer** | 4-Player Split-Screen, Online (Modern). | 4-Player Split-Screen (Basic). |
| **Unlockables** | Shop, Characters, Costumes, Modes, Art. | Level unlocking only. |

## Phased Roadmap

### Phase 1: The "Classic" Foundation (SMB 1 Parity)
*Goal: Replicate the feel and core loop of the original arcade/GameCube classic.*

1.  **Physics & Control Overhaul**
    *   Implement an "Arcade Physics" toggle (higher friction, faster acceleration).
    *   Refine Camera logic: Implement "Snap-to-Back" and tighter auto-follow.
    *   **Deliverable:** `MODE_ARCADE` in `game_server.c`.

2.  **Core Game Loop Enhancements (Implemented)**
    *   **Lives System:** Refactored Coin logic in `progress.c` to dynamically grant Extra Lives immediately (100 Bananas = 1 Life).
    *   **Bonus Stages:** Handled `GAME_TIME` as success if `level_bonus()` is true.
    *   **UI/UX:** Implemented visual "Ready? Set? GO!" and "Fall Out" states with sounds.

3.  **The "Holy Trinity" Party Games**
    *   **Monkey Race:**
        *   Power-ups (Speed, Missiles, Peels).
        *   Lap counter and Waypoint system.
    *   **Monkey Target:**
        *   Flight Physics (Gliding state, wind resistance).
        *   Wheel of Danger / Landing Zones.
    *   **Monkey Fight:**
        *   Punch mechanic (Spring attached to ball).
        *   Knockback physics calculation.
        *   Power-ups (Big Punch, Speed, etc.).

### Phase 2: The "Deluxe" Expansion (SMB 2 Features)
*Goal: Add depth, story, and the extended suite of beloved minigames.*

1.  **Campaign Engine**
    *   Support for "Worlds" (groups of levels).
    *   Cutscene playback integration (Video or In-Engine).
    *   World Map / Hub UI.

2.  **Advanced Party Games (Physics Heavy)**
    *   **Monkey Bowling:**
        *   Pin physics simulation.
        *   Spin control UI.
    *   **Monkey Golf / Mini-Golf:**
        *   Stroke power meter.
        *   Club selection.
    *   **Monkey Billiards:**
        *   Cue ball physics, spin, banking.
        *   Rulesets (9-ball, 8-ball).

3.  **Dynamic Level Elements**
    *   Moving/Morphing Stages (e.g., Arthropod level).
    *   Switches that alter geometry in real-time.
    *   Warp Gates.

### Phase 3: The "Modern" Mechanics (Banana Blitz/Mania)
*Goal: Modernize the engine with features from the Wii/Switch eras.*

1.  **Character Class System (Implemented)**
    *   `struct character_stats` implemented (Speed, Acceleration, Jump, Control).
    *   Character Selection Screen with interactive stat visualization and unlock requirements.

2.  **New Mechanics (SMB & MB)**
    *   **Jump:** Active ability to hop (toggleable per mode/level). (Partially Implemented)
    *   **Spin Dash:** Chargeable speed boost (Sonic-style). (In Progress)
    *   **Powerups:** Super Speed, Super Jump, Gyrocopter (Marble Blast).
    *   **Gravity:** Gravity Modifiers / Inverters.

3.  **Unlock & Economy System (Implemented)**
    *   "Bananas/Coins" as persistent currency (`profile.c`).
    *   In-game Shop UI (`st_shop.c`) to unlock Characters, Costumes, and Modes.

4.  **Minigame Explosion (Implemented)**
    *   *Tier 1:* Monkey Boat, Monkey Shot, Monkey Soccer.
    *   *Tier 2:* Monkey Tennis, Monkey Baseball.
    *   *Tier 3:* Whack-a-Mole, Monkey Dogfight, Hammer Throw.

### Phase 4: Future Tech (Rumble & Online)
*Goal: Bring Neverball into the next generation.*

1.  **Online Multiplayer**
    *   Networked Physics (Prediction/Rollback or Lockstep).
    *   Lobby System.
2.  **Level Editor Integration**
    *   In-game level builder (User Generated Content).
3.  **Ghost Data Sharing**
    *   Global Leaderboards with replay downloads.

## Progress Update (Current)

### Completed Features
*   **Physics Parity:** "Arcade Physics" toggle implemented (CONFIG_PHYSICS).
*   **Party Games:**
    *   **Monkey Target:** Flight physics, landing zones, and scoring implemented.
    *   **Monkey Fight:** Punch mechanics and knockback implemented.
    *   **Monkey Billiards:** 16-ball physics, pockets, and cue stick implemented.
    *   **Monkey Bowling:** 10-pin setup, frame logic, and basic throwing UI implemented.
    *   **Monkey Race:** Basic race mode with waypoints supported.
*   **Documentation:** Comprehensive Party Mode instructions added (`doc/party_games.txt`) and UI descriptions integrated into the menu.

### In Progress / Next Steps
*   **Character Mechanics:** Spin Dash is being implemented. Jump is in place.
*   **UI Polish:** Enhance in-game HUDs for Party Games and new mechanics (Dash Charge Meter).
*   **Campaign:** Hub World logic (Warps) is in place; need to build actual Hub levels and integrate cutscenes.
*   **Marble Blast Features:** Assess feasibility of Gravity Modifiers.

## Immediate Action Plan
The team should focus on **Phase 3, Item 2: New Mechanics (Spin Dash)**.
*   **Reasoning:** Completing the "Modern" movement suite (Jump + Dash) creates a solid foundation for designing levels that utilize these abilities, bridging the gap between SMB and Marble Blast gameplay styles.
*   **Prerequisite:** `game_server.c` logic for impulse application.
