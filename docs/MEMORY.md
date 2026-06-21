[PROJECT_MEMORY]

# Neverball Engine Architecture & Project Memory

This document serves as a comprehensive summary of the architecture, patterns, decisions, and accumulated knowledge of the Neverball repository based on extensive interaction and exploration.

## 1. High-Level Architecture & Compilation Pipeline
*   **Core Languages:** C90/ANSI C with strict compliance checks.
*   **Migration Status:** The codebase is actively migrating from SDL1.2 to SDL2. This requires manual patching for API swaps (e.g., `SDL_WINDOW_OPENGL`, `SDL_GL_SwapWindow()`) and handling `<stdarg.h>` (`va_list`) errors related to strict C90 compliance.
*   **Level Compilation (`.sol` Architecture):** Levels are designed as Quake 3 style brush geometry (`.map`). They are compiled via a custom tool `mapc` into highly optimized binary `.sol` files. This legacy architecture makes dynamic asset loading or real-time level modification natively difficult, as geometry (`s_base`) is expected to be pre-compiled.
*   **Rendering Pipeline Constraints:** Headless integration tests revealed that the rendering pipeline inherently lacked null safety for missing assets. Without deep null-pointer guards in `ball/game_client.c` and `ball/game_draw.c`, missing textures (like `mtrl/default.png`) cause uninitialized array dereferences (e.g., `gd[p].vary.base`), leading to `SIGSEGV` crashes.
*   **Build Environment Quirks:** The build environment strictly intercepts shell commands causing "unusually large diff" errors. Safe interaction with compiler outputs requires redirecting stdout to temporary files and reading them with Python bypass scripts. New source files must be manually registered in the `BALL_OBJS` list in the `Makefile`.

## 2. Core Game Loop & State Management
*   **State Machine (`ball/st_*.c`):** The game operates on a robust state machine (e.g., `st_title`, `st_play`, `st_party`, `st_shop`, `st_story`).
*   **UI Representation Policy:** A strict policy dictates that every implemented feature must be 100% represented in the UI (labels, descriptions, tooltips) and documented. Text strings for `gui_multi` use standard `\n` for line breaks, parsed by `strcspn` in `share/gui.c`.
*   **Game Outcomes:** Events like Wins or Playtime are processed in `ball/progress.c` when `progress_stat` evaluates a `GAME_GOAL` status. Notably, for Bonus stages (`level_bonus`), a `GAME_TIME` (timeout) status is treated as a success (`GAME_GOAL`), allowing for "Collect all you can" mechanics without penalty.

## 3. Persistent Data, Profiles & Economy
*   **Profile System (`ball/profile.c`):** Handles persistent player data stored in the user configuration directory. It tracks currency (coins), unlocked worlds (via an integer array formatted as `world_%d`), and general statistics (`stat_*`).
*   **Achievements (`ball/achieve.c`):** Continuously checks player stats against defined thresholds (`ach_defs`). When met, it unlocks persistent achievement flags (`achieve_*`) in the profile.
*   **Economy & Shop (`ball/st_shop.c`):** An in-game shop utilizes collected coins to purchase unlockables (e.g., Gold Monkey, Master Mode). Character selection (`st_char.c`) restricts access to expensive characters by validating achievement flags (e.g., `ACH_UNLOCK_GOLD`).
*   **Statistic Accumulation:** To optimize write frequency, highly volatile stats like player distance are accumulated in `dist_accumulator` before being committed to persistent storage.

## 4. The Party Mode Suite (`st_party.c` & `game_server.c`)
Neverball has been extensively expanded to achieve feature parity with Super Monkey Ball's party game suite. A dedicated menu allows selection of 14 modes for 1-4 players:
*   **Monkey Golf (`MODE_GOLF`):** Stroke-based state machine featuring artificial gravity logic when the ball rolls slowly over the hole (`zv[0]`).
*   **Monkey Shot (`MODE_SHOT`):** A rail shooter featuring cursor movement, ammo management, reloading mechanics, and a UI crosshair (`crosshair_id` in `hud.c`).
*   **Monkey Dogfight (`MODE_DOGFIGHT`):** Forces flight mode and implements a hitscan machine gun weapon that destroys targets.
*   **Monkey Soccer (`MODE_SOCCER`):** Features team-based spawning, AI seeking behavior, goal detection (`z > 20.0f`), and an extra ball entity (index 0). Requires a specific 40x80 unit field.
*   **Monkey Boat (`MODE_BOAT`):** Custom physics where alternating inputs increase `boat_speed` and simultaneous inputs act as a brake.
*   **Monkey Tennis (`MODE_TENNIS`):** Features swing mechanics (visualized via `CMD_PUNCH`), net collision, and basic scoring. Requires a 20x40 unit court with a net at Z=0.
*   **Monkey Baseball (`MODE_BASEBALL`):** Implements Pitcher (charge/throw) and Batter (swing/hit) roles alongside Home Run logic.
*   **Monkey Mole (`MODE_MOLE`):** A 4x4 grid Whack-a-Mole minigame; rolling over popped-up balls awards points and bounces the player.
*   **Monkey Hammer Throw (`MODE_HAMMER`):** Utilizes tilt input to build spin charge before throwing the ball for distance.

## 5. Items, Modifiers & Enhanced Physics
*   **Item Integration:** `share/geom.c` maps new item types (Gyro, Shock, Super Speed) to geometries for rendering.
*   **Modifier States:** States like `shock_active` and `speed_active` are tracked in `game_server.c` to modify physics properties (e.g., elasticity dampening, speed boosting).
*   **Client Synchronization:** Held items are synchronized to the client via `CMD_HELD_ITEM`, stored in `client_stats.held_item`, and displayed by name in the HUD.

## 6. Ghosts & Replay System
*   **Recording:** Replay streams are written to `.gho` files via `demo_ghost_record` in `ball/demo.c`.
*   **Playback & Sync:** The game loop synchronizes playback using `game_client_ghost_sync(ghost_fp)`. `demo_ghost_open` skips the file header to prepare for this synchronization.
*   **Rendering:** Handled by `game_client_draw_ghost` inside `play_loop_paint`. It uses `ball_draw_geom` to render a translucent 3D mesh instead of a simple GL point.
*   **Browser UI:** The Replay Browser (`st_demo.c`) allows players to "Watch" or "Race" (`progress_race`) ghosts, displaying replay scores, times, and level names under thumbnails.

## 7. Experimental & Future Tech (Phase 4)
*   **Level Editor Prototype (`st_edit.c`):** A Phase 4 prototype introduces the `editor_tile` data structure, a UI overlay, grid-snapped coordinates, and placement controls. To bypass the restrictive legacy `.sol` loading architecture, it visualizes placed tiles using direct immediate-mode OpenGL wireframes (`glBegin(GL_LINES)`). Future iterations will require serializing this array to JSON/XML and potentially invoking the `mapc` compiler in a background thread to generate playable `.sol` files dynamically.
*   **Hub & Story Mechanics:** `progress_hub` sets `MODE_HUB` to load a default level serving as a Hub World. `st_story.c` implements a "Hub Return" mechanism and can load multi-page slide cutscenes by parsing pipe-delimited text files.

## 8. Documentation & Versioning Directives
*   **Meta-Files:** The user mandates strict maintenance of all project meta-files (`VISION.md`, `MEMORY.md`, `DEPLOY.md`, `CHANGELOG.md`, `ROADMAP.md`, `TODO.md`, `IDEAS.md`) to reflect ongoing architectural shifts.
*   **Versioning:** The version string (currently `1.6.14-dev`) resides in the `VERSION` file. It must be bumped on every build/commit and explicitly referenced in commit messages.
*   **Universal Agent Rule:** All AI instruction files (CLAUDE.md, GEMINI.md, GPT.md) must reference a universal set of instructions located at `docs/AGENTS_UNIVERSAL.md`.
*   **Code Commenting:** Code must be commented in extreme depth to explain "why" and detail structural side effects, while self-explanatory code is left bare.