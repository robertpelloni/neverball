# Handoff - The "Golden Era" Parity Update

## Executive Summary
This has been an extraordinarily productive session resulting in the functional completion of Phases 1, 2, and 3 of the Super Monkey Ball Feature Parity Roadmap! The Neverball engine has been significantly expanded to support an entire suite of 14 party minigames, a persistent economy, an in-game shop, character unlockables, and scriptable multi-scene cutscenes.

We have officially transitioned into Phase 4 (Future Tech), laying the groundwork for an in-game level editor (`st_edit.c`) and beginning feasibility studies for Online Multiplayer.

## Key Accomplishments & Features Implemented
*   **Minigame Expansion (Completed Phase 3):**
    *   **Monkey Golf:** Overhauled `game_golf_step` with custom sink logic, checking ball velocity and distance to apply artificial gravity towards the hole's center.
    *   **Monkey Baseball:** Implemented `game_baseball_step` with Pitcher (curveball logic via tilt) and Batter (loft and power scaling via tilt timing) mechanics. Added hitscan logic and distance-based scoring.
    *   **Monkey Dogfight:** Added `MODE_DOGFIGHT` utilizing the flight mechanics. Players are forced into wing-deployment and use a hitscan machine gun to destroy opponents. Deals 10 damage per tick; 100 damage respawns the victim and awards the shooter 50 points.
    *   **Whack-a-Mole:** Added `MODE_MOLE`. Spawns a 4x4 grid of 16 balls (`vary->uv`) that randomly pop up on timers. Rolling over them awards points and bounces the player.
*   **Economy & Shop System:**
    *   **Core Loop Polish:** Modified `progress.c` to dynamically grant 1-Up Extra Lives mid-level as soon as the player crosses a 100 coin threshold.
    *   **Shop UI (`st_shop.c`):** Added a new state to the main menu allowing players to spend accumulated coins.
    *   **Persistent Unlocks:** Hooked up `char_buy` in `char.c` to use `profile.c` achievement flags to permanently store unlocked characters (e.g., Gold Monkey uses `ACH_UNLOCK_GOLD`, others use a hashed `STAT_ID`).
*   **Campaign & Story Engine:**
    *   **Scripted Cutscenes:** Upgraded `st_story.c` to parse multi-page slide decks from text files using `story_load_script()`. Slides are pipe-delimited (`image_path.png|Text String`).
*   **Visual Polish:**
    *   **Ghost Rendering:** Upgraded `game_client_draw_ghost` to use the actual `ball_draw_geom` mesh instead of a simple `GL_POINTS` fallback, fully visualizing the ghost with translucency.
    *   **Replay Browser:** `st_demo.c` now displays internal replay metadata (coins, time) underneath the thumbnails.
    *   **HUD Cleanup:** Ensured all mode-specific HUD widgets (crosshair, dash meter, gyro timer) are explicitly hidden during reset phases to prevent visual bleed between minigames.
    *   **Fall-Out Logic:** Shared balls (Soccer/Tennis) now trigger a "Fall Out" sound and reset automatically if they drop off the map.
*   **Phase 4 Scaffolding:**
    *   **Level Editor Prototype:** Created `st_edit.c` and `MODE_EDITOR`, allowing the player to launch into a zero-gravity, free-flight camera mode from the Main Menu to inspect level geometry.
    *   **Research Docs:** Generated `docs/NETWORK_RESEARCH.md` and `docs/LEVEL_EDITOR_RESEARCH.md` to define architectural paths forward.

## Architecture Quirks & Discoveries
1.  **Array Out of Bounds in `game_server.c`:** The engine frequently loops `MAX_PLAYERS` or `player_count`. I had to add explicit `if (p < 0 || p >= MAX_PLAYERS) return;` bounds checking across all `game_*_step` functions to resolve GCC warnings caused by accessing `players[p]`.
2.  **`sim_owner` Logic:** In party modes, `player[0]` is typically the `sim_owner` (Master). For minigames like Dogfight or Race, all players have independent balls but only the Master processes global logic (like scoring goals or resetting shared balls).
3.  **Item Spawning Constraints:** The legacy engine does not easily support dynamic item generation (spawning objects mid-match) because `s_base` geometry arrays are pre-compiled binary (`.sol`). We had to reuse `v_ball` entities for dynamic objects (like the 16 moles in Whack-a-Mole or the Tennis ball).

## Next Steps for Implementor Agent
1.  **Level Editor (Phase 4):** Expand `st_edit.c`. The free-flight camera works. Now, we need a UI overlay (Tile Palette) that allows instantiating pre-compiled `.sol` chunks on a grid.
2.  **Online Multiplayer (Phase 4):** Read `docs/NETWORK_RESEARCH.md`. We need to intercept the `game_proxy_enq` queue and wrap the commands in a UDP packet (using ENet or raw sockets) to sync state between a host and clients.
3.  **Hub World Content:** The `GAME_WARP` logic is done, but we need an actual `.map` file designed as a Hub World to test it thoroughly.

## Status
*   **Version:** `1.6.12-dev`
*   **Build:** Passing and clean (No Warnings).
*   **Active Branch:** `feature/handoff-documentation` -> Merge to `main`.
