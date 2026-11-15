# Architecture & System Memory

## 1. Project Overview & Engine Topology
- Neverball is an open-source 3D physics platformer targeting feature parity with *Super Monkey Ball* and *Marble Blast*.
- Subprojects:
  - `ball/`: Neverball client, server, game states, 14 party modes, level editor.
  - `putt/`: Neverputt turn-based multiplayer minigolf companion.
  - `share/`: Low-level engine utilities, math, audio, SDL2 video, GUI toolkit, collision physics simulation.
  - `data/`: Levels, textures, fonts, UI materials, sound effects, music tracks.

## 2. Server & Client Architecture
- Neverball utilizes a proxy command queue (`share/cmd.c`, `ball/game_proxy.c`) decoupling the client rendering loop from the server physics simulation.
- Physics runs in a fixed-step loop (`solid_sim_sol.c`, `DT = 0.01f`).
- Arcade physics mode (`CONFIG_PHYSICS`) provides snappy acceleration, increased friction, and instantaneous lock-on follow camera.

## 3. UI Framework & Consolidation Principles
- All GUI elements are built with the custom hierarchical GUI toolkit in `share/gui.c`.
- Title menu organizes high-value modes into primary game choices (Arcade Play, Story Mode, Party Games, Level Editor) followed by extras and configuration.
- Tooltips provide contextual guidance across menu options.

## 4. Headless Execution & Null-Safety Guardrails
- Headless testing under `xvfb-run` runs without physical sound or standard display hardware.
- Defensive guards in `game_common.c` (`game_view_fly`) and `game_draw.c` (`game_draw_fore`) are necessary to protect against null pointers when textures/varying objects fail to initialize during headless operation.
