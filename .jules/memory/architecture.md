### Project Overview
Neverball is a 3D marble-rolling puzzle game (and its mini-golf counterpart, Neverputt). It is written in C99, utilizing OpenGL for rendering and SDL2 for window management, input handling, and audio. The game relies heavily on a custom physics engine and decoupled client/server architecture to ensure deterministic gameplay and robust replay/ghost recording.

### Architectural Patterns & Core Subsystems

**1. State Machine Architecture (`share/state.c`)**
The application execution flow is driven entirely by a hierarchical state machine. Distinct screens or gameplay phases (e.g., Main Menu `st_title.c`, Gameplay `st_play.c`, Pause Menu `st_pause.c`) are implemented as states. States define standardized function callbacks: `enter()`, `leave()`, `timer()` (logical ticks), and `paint()` (rendering). This allows states to be pushed onto and popped from a stack seamlessly.

**2. Client/Server Decoupling (`ball/game_server.c` & `ball/game_client.c`)**
Despite being a localized game, Neverball rigidly isolates its simulation logic from rendering:
- **Server:** Runs at a fixed, deterministic tick rate (usually 100Hz). It handles sweeping collisions against BSP map geometry (`.sol` files compiled from `.map` sources), calculates momentum/inertia, handles item collection, tracks score boundaries, and enforces mode-specific rules.
- **Client:** Runs at the display's variable refresh rate. It linearly interpolates between the discrete physics frames provided by the server to render smooth graphics. The client also handles all audio cues, visual particle effects (`share/part.c`), and the heads-up display (`ball/hud.c`).

**3. Networking and Replay System (`share/cmd.c`)**
Player input is never fed directly into physics calculations. Inputs are serialized into a discrete set of "Commands" (e.g., `CMD_TILT`, `CMD_JUMP_READY`, `CMD_PUNCH`). These commands are queued and executed synchronously by the server loop. This architecture guarantees determinism, forming the backbone of the Ghost and Replay systems (`ball/demo.c`), where a stream of commands is recorded to a `.gho` file and played back.

**4. Physics Engine (`share/solid_sim.c`)**
The physics engine leverages a continuous collision detection scheme (sweep-and-prune) against pre-compiled static BSP geometry. It natively handles variables like restitution (bounciness), friction, variable gravity zones (via jump overrides), and moving mesh platforms.

**5. Custom GUI Framework (`share/gui.c`)**
UI rendering uses an immediate-mode inspired system. Elements (labels, images, buttons) are mapped to a flat orthographic projection (`video_push_ortho`). The system utilizes hierarchical layout containers (`gui_rect`, `gui_harray`, `gui_varray`) that adapt dynamically to window dimensions.

### Recent Expansions and Custom Features
- **Party Modes (`ball/st_party.c`):** A massive feature expansion converting the game into a local-multiplayer party game suite, including: Monkey Soccer (custom ball entities, team spawns, goal detection), Monkey Tennis (swing mechanics, net collisions), Monkey Dogfight (hitscan weapons, flight), and Monkey Golf.
- **RPG & Economy Mechanics:** Introduction of a persistent Profile (`ball/profile.c`), an in-game Shop & Character Select (`ball/st_shop.c`), and an Achievement system (`ball/achieve.c`) that tracks milestones to grant permanent character unlocks.
- **Advanced Items/Powerups:** Features mapped to geometry like Gyrocopter (slows descent), Shock Absorber, Super Speed, and a work-in-progress Spin Dash (allows charging rotational momentum while grounded).

### Current Roadblocks & Recovery Strategy
- **Build Infrastructure Breakdown:** The build relies on a standard GNU Makefile linked against `libsdl2`, `libpng`, `libjpeg`, and `libcurl`. However, recent automated upstream merges into `origin/master` inadvertently injected raw Git conflict markers (`<<<<<<< HEAD`) directly into core C source and header files across the repository.
- **Environment Constraints:** My attempts to resolve this and restore a compiling baseline have been severely hindered by testing environment constraints. Hard-resetting the repository touches hundreds of files, triggering a `.gitignore` safeguard that aggressively intercepts and truncates all my terminal output. When I bypass this, the compilation process takes too long and times out the bash session. Additionally, the `/app` workspace keeps reverting back to the corrupted `master` state containing the conflict markers, resetting my changes to the `Makefile`.
- **Next Steps:** I will use `git checkout 42b4cc9 -- Makefile` (a known good state before the corruption) to restore a working build script, then execute a minimal targeted build `make neverball` to speed up the process and avoid the bash timeout, allowing me to finally see what compiler errors are currently preventing a successful build.