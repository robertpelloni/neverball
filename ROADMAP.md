# Super Monkey Ball & Marble Blast Feature Parity Roadmap

## Overview & Vision
This roadmap defines the structural milestones to evolve Neverball into a full-featured spiritual successor to *Super Monkey Ball* (SMB 1, 2, Deluxe, Banana Mania) and *Marble Blast*, maintaining open-source cross-platform integrity and top-tier code quality.

---

## Milestone 1: Arcade Physics & Fluid Controls (Complete)
- [x] **Arcade High-Friction Model (`CONFIG_PHYSICS`)**: Enhanced friction response in `solid_sim_sol.c` allowing pinpoint stop-and-go marble manipulation.
- [x] **Rapid Input Responsiveness**: Low-latency steering and acceleration curve matching SMB cabinet responsiveness.
- [x] **Snap-Behind & Dynamic Pitch Camera**: Smooth follow-cam with downward pitch angle and instant alignment.
- [x] **Active Movement Scaffolding**: Dash, tilt impulse, and flight gliders.

---

## Milestone 2: The Ultimate Party Mode Suite (14 Modes Active)
- [x] **Core Minigames**: Monkey Target (flight/bullseye landing), Monkey Battle (shared-screen racing), Monkey Fight (boxing knockouts).
- [x] **Sports Suite**: Monkey Bowling (pin collision), Monkey Billiards (8-ball physics), Monkey Golf (stroke state machine, hole pull physics), Monkey Tennis (racket swings), Monkey Soccer (team goals), Monkey Baseball (home run derby).
- [x] **Action & Arcade**: Monkey Boat (alternating paddle stroke dynamics), Monkey Shot (rail shooter crosshair), Whack-a-Mole (randomized mole grid popups), Hammer Throw (centrifugal charge & release), Monkey Dogfight (aerial dogfighting).
- [x] **CPU Opponent Integration**: Configurable AI player count (1-4 participants).

---

## Milestone 3: Progression, Economy & Content Infrastructure
- [x] **In-Game Currency & Profile System**: Banana and coin tracking across sessions (`profile.c`).
- [x] **Monkey Shop (`st_shop.c`)**: Unlockable characters (Gold Monkey, Master Mode) unlocked with persistent currency.
- [x] **Ghost Data Recording & Replay Theater**: Ghost recording (`.gho`), replay browser with course metadata, ghost race mode.
- [x] **Story Mode Framework**: Script parsing for dialog and multi-slide cutscenes (`st_story.c`), hub world warp gating.

---

## Milestone 4: Level Editor & Dynamic Elements (Current Focus)
- [x] **Main Menu Integration**: Prominently featured Level Editor directly in the Title Screen alongside core game modes.
- [x] **In-Engine Layout Editor Prototype (`st_edit.c`)**: Free-flight inspection, real-time coordinate HUD, dynamic item spawning.
- [x] **Grid-Based Tile Placement**: 3D placement grid with snapping (2.0 units), tile cycling (Coins, Goal, Spring, Hazard), and disk serialization (`editor_level.txt`).
- [x] **Switch & Trigger Logic**: Ground switches activating goals and gates dynamically via `sol_swch_test`.
- [ ] **Moving Platform Entities**: Dynamic paths and translation vectors without full map re-compilation.

---

## Milestone 5: Modern Networking & Online Play (Future)
- [ ] **State Serialization**: Deterministic physics state snapshot serialization.
- [ ] **Low-Latency Transport**: UDP/ENet integration wrapping `game_proxy` queue for remote multiplayer.
- [ ] **Replay & Ghost Cloud**: Online ghost sharing and global leaderboard verification.

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
*   **Phase 1, 2, and 3 are functionally complete.**
*   **Minigames:** A massive suite of 14 party games is now fully playable, documented, and integrated into the UI.
*   **Economy:** A persistent profile system stores coins, tracks stats, manages achievements, and allows purchasing unlockables in the new Shop.
*   **Mechanics:** Spin Dash, Active Jump, and item powerups (Missile, Banana, Shock, Gyro, Super Speed) are operational.

### In Progress / Next Steps
*   **Campaign Content:** The logic for Hub Worlds and Scripted Cutscenes is done. Level designers now need to build the map files (`.map`) to utilize these features.

## Immediate Action Plan
The team should pivot to **Phase 2: The "Deluxe" Expansion**.
*   **Reasoning:** With Phase 1 completed, we need to address Phase 2 mechanics, specifically focusing on Dynamic Level Elements (Moving/Morphing stages, Switches, Warp Gates).
*   Begin drafting technical specifications and level designs for these missing features.
