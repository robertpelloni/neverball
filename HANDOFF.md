# Handoff - Party Mode Polish & HUD Cleanup

## Summary
This session focused on polishing the Party Mode experience by implementing robust fall-out handling for shared game balls (Soccer, Tennis) and cleaning up the HUD state transitions. We also fixed several compilation issues related to missing headers and undeclared functions.

## Completed Tasks
*   **HUD Polish:** Updated `hud_update` in `ball/hud.c` to properly hide all party mode widgets (crosshair, toast, item, gyro, dash, jump) when the HUD is reset, preventing visual glitches between modes.
*   **Fall-Out Logic:** Implemented logic in `game_server.c` to detect when shared game objects (Soccer ball, Tennis ball) fall out of bounds. The system now plays a sound, resets the ball to its spawn point, and displays a "Ball Reset!" toast notification.
*   **Bug Fixes:**
    *   Fixed implicit declaration of `v_dist` in `game_server.c` by implementing manual distance calculation using `v_sub` and `v_len`.
    *   Added missing `#include "progress.h"` to `ball/game_client.c` to resolve `curr_mode()` and `MODE_*` enum errors.
    *   Added missing `#include "st_play.h"` to `ball/st_demo.c` to fix `st_play_ready` undeclared error.
    *   Fixed build warnings related to unused variables in `game_server.c`.

## Current State
*   **Build:** Passing (verified with `make`).
*   **Version:** `1.6.4-dev` (Bumped from `1.6.3-dev` in `VERSION` file).
*   **Active Branch:** `feature/party-mode-polish` -> ready to merge to `main`.

## Next Steps (Immediate)
1.  **Merge & Deploy:** Commit current changes and merge to `main`.
2.  **Baseball Implementation:** The `game_baseball_step` logic exists but needs refinement (pitching/batting mechanics are basic).
3.  **Online Multiplayer:** Begin feasibility study for networked physics as per roadmap (Phase 4).

## Notes for Next Agent
*   The `v_dist` function does not exist in `vec3.h`; use `v_sub` followed by `v_len`.
*   Accessing `game_mode` in client-side code (`game_client.c`) requires `curr_mode()` from `progress.h`.
*   The `VERSION` file is the source of truth; ensure it is incremented for every new feature set.
