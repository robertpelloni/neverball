# Changelog

## [1.6.13-dev] - Dynamic Level Editing

### Added
- **Dynamic Item Placement:** In `MODE_EDITOR`, users can now press the Dash (B) button to spawn an item (defaulting to Banana) 10 units in front of the camera.
- **Dynamic Networking:** Added `CMD_PLACE_ITEM` to `cmd.c` to synchronize dynamically generated level objects over the network to `game_client.c`, resolving the inability of `s_vary` to spawn items outside of `.sol` file compilation.

## [1.6.12-dev] - Editor Scaffolding

### Added
- **Level Editor Prototype:** Added `st_edit.c` to serve as a foundational state for a future in-game level editor. Currently acts as a free-flight camera viewer loading a default environment.
- **Editor Mode:** Implemented `MODE_EDITOR` in `progress.h` and `game_server.c`, utilizing the flight mechanics (zero gravity) and standard movement inputs to allow users to free-roam and inspect geometry.
- **Main Menu UI:** Exposed the prototype via a "Level Editor" button in the main menu (`st_title.c`).

## [1.6.11-dev] - Phase Parity Met & Research

### Added
- **Core Loop Polish:** Modified `progress.c` to dynamically grant extra lives mid-level as soon as the player crosses the 100 coin threshold, matching the classic SMB formula.
- **Future Tech Scaffolding:** Added `docs/NETWORK_RESEARCH.md` and `docs/LEVEL_EDITOR_RESEARCH.md` exploring feasibility and architectures for Phase 4 implementation.
- **Roadmap Shift:** Officially marked Phase 1, Phase 2, and Phase 3 as functionally complete in software. The roadmap and TODO have pivoted to Content Generation and Phase 4 (Online/Editors).

## [1.6.10-dev] - Monkey Golf Enhancements

### Added
- **Monkey Golf Hole Logic:** Overhauled `game_golf_step` to accurately detect when the ball slows down over the hole (`zv[0]`), apply an artificial gravity to pull it in, and trigger the goal sequence and toast notification.

## [1.6.9-dev] - Character Unlocks & Progression

### Added
- **Character Unlocks:** The character selection screen (`st_char.c`) now queries the user profile to determine if a character has been purchased. Characters with a cost > 0 are locked by default until bought.
- **Gold Monkey Unlock:** Explicit logic was added to check the `ACH_UNLOCK_GOLD` profile stat to unlock the Gold Monkey. Generic names use a hashing mechanism for unique storage IDs.

## [1.6.8-dev] - In-Game Shop & Economy

### Added
- **Monkey Shop:** Added an in-game shop UI (`st_shop.c`) accessible from the main menu, allowing players to spend accumulated coins on unlockable items.
- **Unlockables System:** Extended `profile.c` and `achieve.h` to permanently store and retrieve unlocked items (e.g., Gold Monkey, Master Mode, Concept Art).
- **Economy:** Coins collected during standard play and party games are now actively used as currency for the Shop.

## [1.6.7-dev] - Monkey Dogfight

### Added
- **Monkey Dogfight Mode:** Added `MODE_DOGFIGHT` where players are permanently in flight mode and can shoot opponents with a hitscan machine gun.
- **Weapon System:** Players have an ammo counter that reloads automatically over time. Dealing 100 damage (tracked via the victim's "coins") destroys the target, respawns them, and rewards the shooter with points.

## [1.6.6-dev] - Whack-a-Mole & Hammer Throw

### Added
- **Whack-a-Mole Mode:** Added `MODE_MOLE` where players race to roll over 16 randomly popping moles in a grid for points. Bumping a mole awards points and a slight upward bounce.
- **Hammer Throw Mode:** Added `MODE_HAMMER` where players rapidly tilt left/right to build rotational speed (indicated on the dash charge meter), then press Action to launch the ball as far as possible.
- **Story Scripting:** `st_story.c` now supports `story_load_script` which reads pipe-delimited text files to play multi-page slide cutscenes instead of static single images.

## [1.6.5-dev] - Replay Metadata & Ghost Visuals

### Added
- **Replay Browser:** Now displays the replay's internal score, time, and recorded level name underneath each thumbnail in the list view (`st_demo.c`).
- **Documentation:** Major updates to meta-documentation, creating `VISION.md`, `DEPLOY.md`, `IDEAS.md`, and unifying AI specific directives into individual files referencing `AGENTS_UNIVERSAL.md`.

## [1.6.4-dev] - Party Mode Polish & Build Fixes

### Added
- **Fall Out Logic:** Added out-of-bounds detection for shared balls in Monkey Soccer and Monkey Tennis. The ball is now automatically reset with a "Ball Reset!" toast notification.
- **HUD Cleanup:** Ensured all party mode specific HUD elements (crosshair, toast, gyro, dash, jump) are properly hidden when the HUD is reset between states.

### Fixed
- Fixed array out-of-bounds compilation warnings in `game_server.c`.
- Fixed implicit declaration warnings for `v_dist` and `curr_mode()`.
- Fixed unused variable warnings for `spin_axis` and `spin_mag` in `game_server.c`.
- Cleaned up build errors by adding missing includes (`progress.h`, `st_play.h`).

## [1.6.3-dev] - Soccer, Tennis, Ghosts & Stats

### Added
- **Monkey Tennis:** New 1v1/2v2 party game with racket mechanics and scoring.
- **Monkey Soccer:** New 2v2 party game with a giant ball and goal detection.
- **Ghost Race:** "Race against Ghost" feature in the Replay Browser.
- **Persistent Stats:** System for tracking jumps, distance, and coins across sessions.
- **Achievements:** Achievement unlocking system with HUD Toast notifications.
- **Party Modes:** Added Golf, Boat, and Shot modes.
- **Character Stats:** Loading character attributes (speed, jump, etc.) from files.
- **Documentation:** New `ARCHITECTURE_DEEP_DIVE.md` and updated `party_games.txt`.

## [1.6.2-dev] - Party Mode UI & Documentation

### Added
- **UI:** Added detailed descriptions and control hints to the Party Games menu (`st_party.c`).
- **Documentation:** Created comprehensive instructions for all Party Games in `doc/party_games.txt`.
- **Manual:** Updated `doc/manual.txt` to reference the new Party Games documentation.
- **Monkey Bowling:** Formally exposed the Bowling mode in the Party Game selection cycle.

## [1.6.1-dev] - Campaign Engine & Party Update

### Added
- **Campaign Hub Engine:** Implemented `GAME_WARP` logic allowing level transitions via in-game switches, enabling non-linear Hub World navigation.
- **Story Mode:** Added `st_story` state for narrative cutscenes (static image + text).
- **Party Mode Menu:** Dedicated menu for selecting party games (Target, Fight, Race, Billiards, Bowling).
- **Split-Screen Support:** Refactored engine (`game_server`, `game_client`, `game_draw`) to support up to 4 local players with independent viewports.
- **Monkey Billiards:** New game mode with 16-ball physics, cue stick rendering, and pocket logic.
- **Monkey Bowling:** New game mode with pin physics and frame scoring.
- **Monkey Fight:** New game mode with punch mechanics (`CMD_PUNCH`) and knockback.
- **Monkey Target:** New game mode with flight physics (lift/drag), landing zones, and instrument HUD.

### Changed
- Refactored `game_server.c` to handle arrays of player states (`server_player`).
- Updated `game_draw.c` to support "Ghost Rendering" and multiple viewports.
- Input system now routes events via `device_id` to specific players.

### Fixed
- Fixed build error by adding `st_story.o` and `st_party.o` to Makefile.

## [1.6.14-dev] - 2026-06-25
### Added
- Phase 1 Arcade Physics: Increased global friction modifier to 4.0x when `MODE_ARCADE` is active.
- Phase 1 Arcade Physics: Increased input acceleration multiplier to `0.6f` to achieve snappy movement.
- Phase 1 Camera: Adjusted arcade camera "snap-to-back" interpolation speed to tightly track the ball's velocity vector.
