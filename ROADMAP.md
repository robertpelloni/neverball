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
