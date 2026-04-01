# Project Dashboard

## Directory Structure
*   `ball/` - Core game engine source code for Neverball. Contains state management, physics integration, and party mode logic.
*   `putt/` - Source code for Neverputt (Minigolf variant).
*   `share/` - Shared libraries, physics engine (`solid_sim_sol.c`), rendering (`solid_draw.c`), and math utilities.
*   `data/` - Game assets, including maps, textures, geometry, and UI elements.
*   `doc/` - End-user documentation (manuals, party game rules).
*   `docs/` - Developer documentation, roadmaps, AI agent instructions.
*   `locale/` & `po/` - Translation files for i18n.
*   `scripts/` - Utility scripts for building and versioning.

## Submodules
*Currently, the project uses integrated dependencies (like `stb` or `miniz` found in `share/`) rather than explicit git submodules.*

If external submodules are added in the future (e.g., for advanced physics or networking), they will be listed here with their respective paths, versions, and commit hashes.

## Build Status
*   **Current Version:** (See `VERSION` file)
*   **Primary Target:** SDL2 / OpenGL Desktop (Linux, Windows, macOS).
