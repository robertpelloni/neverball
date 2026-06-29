# Phase 1: Arcade Physics Overhaul Status

## Implemented Features
1. **High Friction (`MODE_ARCADE`)**: The physics engine in `share/solid_sim_sol.c` now multiplies global friction by 4x when the `CONFIG_PHYSICS` toggle is enabled via the Options Menu.
2. **Snappy Acceleration**: In `ball/game_server.c`, input response speed is dramatically increased (`s * 0.1f` modifier) when `CONFIG_PHYSICS` is active, providing the requested Super Monkey Ball feel.
3. **Camera Lock-on**: The camera now strictly auto-follows the ball horizontally using a high `v_lerp` factor (30.0f) and enforces a fixed 15-degree downward pitch, bypassing the standard mouse-look momentum entirely.

## Supervisor Notice
This task is entirely complete. No further work is required for Phase 1 Arcade Physics or Camera refinement. The next priority MUST be expanding the Level Editor.
