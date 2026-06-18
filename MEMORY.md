# MEMORY: Architectural Observations & Guidelines

## Codebase Traits
* The engine is written in C99, primarily using OpenGL 1.2/SDL2.
* Memory management is manual; memory leaks must be actively prevented, especially in long-running states like `game_server.c`.
* The physics simulation (`solid_sim_sol.c`) is deterministic based on a fixed timestep (`DT`).
* `s_base` represents static geometry, while `s_vary` holds dynamic objects (like `v_ball`).

## Design Preferences
* **UI Construction:** Use `gui.c` functions (`gui_vstack`, `gui_hstack`, `gui_label`, `gui_button`) for menu layouts. Hide/show dynamic elements rather than constantly recreating them to save memory.
* **Network & States:** State transitions happen via `goto_state()`. Be careful to clean up (via `_leave` functions) to avoid orphaned resources.
* **Documentation:** Code must be commented in extreme depth. Why the code is there and side effects must be explained.

## Known Quirks
* Array bounds in `game_server.c` must be explicitly checked against `MAX_PLAYERS` to avoid segfaults in minigame step functions.
* The legacy `.sol` architecture makes dynamic geometry loading tricky. A tile-based editor requires clever management of `s_base` or multiple `v_item`/`v_ball` instances if we cannot easily append to the main `s_base`.
