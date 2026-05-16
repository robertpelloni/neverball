# Neverball Project Architecture, Patterns, and Decisions

## 1. Core Architecture and Technologies
* **Language:** C (ANSI/C90/C99 style).
* **Multimedia Library:** Currently undergoing a challenging migration from **SDL1.2 to SDL2**. Many subsystems (video, events, input) are being patched to reflect SDL2 API changes (e.g., `SDL_WINDOW_OPENGL`, `SDL_GL_SwapWindow`, `<SDL2/SDL.h>`).
* **Graphics:** OpenGL. GUI rendering uses `video_push_ortho` to establish an orthographic projection spanning the full window dimensions (`video.device_w`, `video.device_h`) regardless of active viewport size.
* **Level Format:** Quake 3 style brush geometry (`.map`). Levels are compiled via `mapc` into `.sol` files.
* **Network/State:** Game state is split between server (`game_server.c`) and client (`game_client.c`), communicating via commands (e.g., `CMD_GYRO_STATE`, `CMD_PUNCH`, `CMD_HELD_ITEM`).

## 2. Game Modes and Mechanics
* **Party Modes (`st_party.c` / `game_server.c`):**
  * **Monkey Golf (`MODE_GOLF`):** Stroke-based state machine. Includes custom sink logic (artificial gravity) when the ball rolls slowly over the hole (`zv[0]`).
  * **Monkey Shot (`MODE_SHOT`):** Rail shooter party game featuring cursor movement, ammo management, and reloading mechanics. Crosshair visible in `ball/hud.c`.
  * **Monkey Dogfight (`MODE_DOGFIGHT`):** Forces players into flight mode; adds hitscan machine gun weapon.
  * **Monkey Soccer (`MODE_SOCCER`):** Team-based spawning, extra ball entity (index 0), goal detection (`z > 20.0f`), AI seeking. Field: 40x80 units.
  * **Monkey Tennis (`MODE_TENNIS`):** Swing mechanics visualized via `CMD_PUNCH`, net collision, scoring. Court: 20x40 units.
  * **Monkey Boat (`MODE_BOAT`):** Specific physics (`game_boat_step`) where alternating inputs increase speed and simultaneous inputs brake.
  * **Monkey Mole (`MODE_MOLE`):** Whack-a-Mole using a 4x4 grid. Rolling over balls awards points and bounces the player.
  * **Monkey Hammer Throw (`MODE_HAMMER`):** Tilt input to build spin charge, throwing ball in a forward arc.
  * **Monkey Baseball (`MODE_BASEBALL`):** Pitcher/Batter roles and Home Run logic.
* **Variable Gravity:** Supported via `pl->gravity_dir` in `game_server.c`. Gravity zones triggered by overloading `b_jump` entities based on jump vector magnitude (`q <= 20.0f`).
* **Bonus Stages:** Treat `GAME_TIME` (timeout) as a success (`GAME_GOAL`) to allow "Collect all you can" gameplay.

## 3. Items and Powerups
* **Core Definitions:** Defined in `share/solid_base.h` (`ITEM_MISSILE`, `ITEM_BANANA`, `ITEM_GYRO`, `ITEM_SHOCK`, `ITEM_SUPER_SPEED`).
* **Visuals:** Mapped in `share/geom.c` (`GEOM_GYRO`, `GEOM_SHOCK`, etc.).
* **Physics/Mechanics:** Tracked in `game_server.c` (`shock_active`, `speed_active`) for elasticity dampening and speed boosting. Gyrocopter (`ITEM_GYRO`) slows descent and features a HUD indicator. Held items synchronized to client via `CMD_HELD_ITEM` and displayed in `hud.c`.

## 4. UI, Menus, and HUD
* **HUD (`ball/hud.c`):** Implements Toast notifications (`hud_show_toast`) at `GUI_TOP`, radar/minimap (`hud_radar_draw`) projecting 3D bounds to 2D, and tracks player stats.
* **Menus:** Pause menu (`st_pause.c`) displays active character stats. Character selection (`st_char.c`) restricts characters based on achievement flags (e.g., `ACH_UNLOCK_GOLD`).
* **Text Formatting:** Multi-line text passed to `gui_multi` uses standard `\n` characters parsed by `strcspn` in `share/gui.c`.

## 5. Progression, Stats, and Persistence
* **Profiles (`ball/profile.c`):** Stores currency, unlocked worlds (`world_%d`), general statistics (`stat_*`), and achievements in the user config directory.
* **Achievements (`ball/achieve.c`):** Checks stats against thresholds (`ach_defs`) and unlocks persistent flags.
* **Shop (`ball/st_shop.c`):** Allows purchasing unlockables (e.g., Gold Monkey, Master Mode) using persistent collected coins.
* **Story/Hub (`ball/st_story.c` / `ball/progress.c`):** "Hub Return" mechanism (`story_set_hub`) and multi-page slide cutscenes (`story_load_script`) parsing pipe-delimited files.

## 6. Replays and Ghost Data
* **Recording:** Handled in `ball/demo.c` (`demo_ghost_record`), streaming to `.gho` files.
* **Playback:** `ball/st_demo.c` implements the Replay Browser. `ball/st_play.c` synchronizes ghost playback via `game_client_ghost_sync(ghost_fp)`. Ghost rendering (`game_client_draw_ghost`) utilizes `ball_draw_geom` to render a translucent 3D mesh.

## 7. Project Conventions and Environment Factors
* **Build System Traps:** The environment strictly intercepts large diffs (e.g. from compilation output), requiring python bypass scripts (like `do_tail.py` or `silent.py`) to debug linker/compiler errors safely.
* **Documentation Mandates:** Rigid requirements to maintain detailed files such as `VISION.md`, `MEMORY.md`, `ROADMAP.md`, `TODO.md`, `CHANGELOG.md`, `AGENTS.md` (and related LLM instructions). All submodules must be documented and updated.
* **Version Bumping:** The version number (currently `1.6.10-dev`) must be updated in the `VERSION` file, referenced globally, and incremented on every commit/build with explicit commit messages.
* **Comment Standards:** Code must be commented in extreme depth regarding purpose, side effects, and why decisions were made, while self-explanatory code is left bare.