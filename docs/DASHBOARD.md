# Project Dashboard

## Overview
Neverball is a 3D rolling ball game. This project aims to extend it with features inspired by Super Monkey Ball.

## Version
Current Version: **1.6.2-dev** (See `VERSION` file)

## Project Structure

### Source Code
*   `ball/`: Core game logic (Neverball), including game modes, physics, and states.
    *   `game_server.c`: Game logic, physics simulation, and mode-specific updates.
    *   `game_client.c`: Rendering, interpolation, and client-side logic.
    *   `st_*.c`: Game states (Title, Play, Party, etc.).
*   `putt/`: Minigolf game logic (Neverputt).
*   `share/`: Shared engine code.
    *   `gui.c`: User Interface system.
    *   `geom.c`: Geometry loading and rendering.
    *   `solid_*.c`: Physics engine (Solid).

### Assets
*   `data/`: Game assets (levels, textures, sounds, fonts).
*   `doc/`: User documentation (manuals).
*   `docs/`: Developer documentation (Roadmap, Vision, Agents).
*   `po/`: Localization files.

## Submodules
*No submodules are currently registered in this repository.*

## Build Information
*   **Build System:** Makefile
*   **Dependencies:** SDL2, JPEG, PNG, Vorbis, PhysFS, Freetype, TTF, Gettext.
*   **Compiler:** GCC/Clang (C99 standard).
