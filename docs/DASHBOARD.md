# Master Project Dashboard & System Architecture

## 1. Project Overview & High-Value Organization
This document serves as the singular consolidated dashboard and technical reference for the Neverball ecosystem, detailing repository structure, engine subsystems, server lifecycle, UI layout, desktop environment integration, and submodule tracking.

Neverball is an arcade ball-rolling game and physics platformer targeting feature parity with classic arcade physics games (*Super Monkey Ball*, *Marble Blast*) while preserving the open-source community legacy.

### Key Subprojects
1. **Neverball (`ball/`)**: Core physics-puzzle platformer featuring single-player arcade campaign, story mode hub, 14 party minigames, replay theater, character shop, and in-engine level editor.
2. **Neverputt (`putt/`)**: Turn-based multiplayer minigolf companion game utilizing the shared physics simulation engine.
3. **Map Compiler (`share/mapc.c`, `share/mapclib.c`)**: Standalone tool (`mapc`) compiling Quake 3 brush geometry (`.map`) into precomputed binary collision/rendering files (`.sol`).
4. **Shared Engine (`share/`)**: Low-level math (`vec3`), OpenGL graphics (`solid_draw`, `glsl`, `fbo`), audio mixer (`audio`), font rendering (`font`, `text`), file system abstraction (`fs_stdio`, `miniz`), input management (`joy`), and user interface (`gui`).
5. **Level Editor Tooling (`TrenchBroom/`, `Radiant/`, `ball/st_edit.c`)**: Official level editing configs and entities for TrenchBroom & GtkRadiant, plus the in-engine dynamic block layout editor.

---

## 2. Directory Hierarchy & Codebase Map

| Directory | Role & Description | Key Files |
|---|---|---|
| `ball/` | Neverball game client, server, game states, party modes, HUD | `main.c`, `game_server.c`, `game_client.c`, `st_title.c`, `st_party.c`, `st_edit.c`, `st_story.c` |
| `putt/` | Neverputt minigolf application code | `main.c`, `game.c`, `course.c`, `hole.c`, `st_all.c`, `st_conf.c` |
| `share/` | Core portable engine routines, solid physics, UI framework | `solid_sim_sol.c`, `solid_base.c`, `gui.c`, `video.c`, `audio.c`, `cmd.c` |
| `data/` | Asset packages: levels (`map-*/`), sound/music (`snd/`, `bgm/`), textures, UI | `map-easy/`, `map-medium/`, `map-hard/`, `gui/`, `item/` |
| `doc/` | User manuals and gameplay instructions | `manual.txt`, `party_games.txt`, `changes.txt`, `install.txt` |
| `docs/` | Developer documentation, architecture, research, specifications | `DASHBOARD.md`, `VISION.md`, `DEPLOY.md`, `IDEAS.md`, `AGENTS_UNIVERSAL.md` |
| `dist/` | Desktop integration, icons, appdata, man pages | `neverball.desktop`, `neverputt.desktop`, modern icon sets (16px - 512px) |
| `scripts/` | Build helpers, asset hashing, translation extraction, versioning | `version.sh`, `gen-words.sh`, `hash-assets.sh`, `extractpo.sh` |
| `po/` & `locale/` | Internationalization catalogs (30+ languages) | `neverball.pot`, `*.po`, compiled `.mo` binaries |
| `TrenchBroom/` | Game configuration and entity definitions for TrenchBroom | `GameConfig.cfg`, `entities.ent` |
| `Radiant/` | Quake-style level editor definitions | `neverball.game/` |

---

## 3. Submodule & External Dependency Tracking

The repository embeds lightweight zero-friction libraries within `share/` to avoid fragile external git submodules while keeping dependencies minimal:

| Package | Location | Version / Build | Notes |
|---|---|---|---|
| **miniz** | `share/miniz.c`, `share/miniz.h` | v1.16 | Embedded lossless data compression and zip archive extractor |
| **SDL2** | System dependency | 2.0.0+ | Windowing, OpenGL context creation, audio device, event dispatch |
| **SDL2_ttf** | System dependency | 2.0.12+ | TrueType font rendering engine (FreeType wrapper) |
| **SDL2_mixer**| System dependency | 2.0.0+ | Multi-channel audio mixer supporting Ogg Vorbis and WAV |
| **libpng / libjpeg** | System dependency | Latest | Texture image format decoding |
| **libcurl** | System dependency | 7.x / 8.x | Background asynchronous addon download manager (`fetch_curl.c`) |

---

## 4. UI Architecture & Menu Consolidation

The dashboard UI follows a consolidated, categorized hierarchy designed to maximize feature visibility and eliminate nested dead-ends:

### Title Screen Categorization (`ball/st_title.c`)
1. **Primary Play** (Prominent focus):
   - **Play (Arcade Mode)**: Classic single-player levels with coin unlocking and time attack.
   - **Story Mode**: Non-linear hub progression with world cutscenes.
   - **Party Games**: Consolidated hub for all 14 multiplayer party minigames.
   - **Level Editor**: Live interactive level layout and flythrough inspection.
2. **Extras & Economy**:
   - **Monkey Shop**: Persistent banana currency store for unlocking bonus characters and modes.
   - **Replay Theater**: Playback recorded runs and race high-score ghost data.
3. **System & Guide**:
   - **Help & Guide**: Rules, camera tricks, advanced tilting controls.
   - **Options**: Video resolution, audio sliders, gamepad rebinding, and SMB Arcade Physics toggle.
   - **Exit**: Clean shutdown and return to OS.
4. **Live Contextual Tooltips**:
   - Interactive description banner dynamically updates on mouse hover or gamepad/stick navigation to explain the highlighted mode before launching.

---

## 5. Server Lifecycle, Headless Execution & System Tray Integration

### Game Proxy & Client-Server Architecture
Neverball operates with a decoupled client-server architecture:
- **Server (`game_server.c`)**: Runs fixed-timestep deterministic physics (`solid_sim_sol.c`, `DT = 0.01f`), updates ball dynamics, checks triggers/goals, and processes player inputs.
- **Queue (`share/cmd.c`, `ball/game_proxy.c`)**: Bidirectional serialized command stream passing state updates (`CMD_BALL_POSITION`, `CMD_STATUS`, `CMD_HELD_ITEM`, `CMD_PLACE_ITEM`).
- **Client (`game_client.c`)**: Interpolates rendering transforms (`game_client_blend`), manages camera follow modes, and renders 3D geometry via OpenGL.

### Server Lifecycle & Headless Execution
Neverball supports execution in headless or continuous integration environments without a physical display:
- **Launch Command**:
  ```bash
  xvfb-run -a ./neverball --debug > neverball.log 2>&1 &
  ```
- **Process Status & Monitoring**:
  ```bash
  pgrep -af neverball
  tail -f neverball.log
  ```
- **Graceful Termination**:
  ```bash
  pkill -TERM neverball || kill $(pgrep -f neverball)
  ```

### Desktop System Tray & Window Manager Integration
- **Desktop Entry**: Configured via `dist/neverball.desktop` and `dist/neverputt.desktop` specifying categories `Game;ArcadeGame;`.
- **System Tray / Window Management**:
  - Window creation and lifecycle are driven through SDL2 (`video.c`).
  - Desktop integration handles focus gain/loss gracefully, automatically pausing audio and invoking `st_pause` to prevent background CPU waste.
  - Shutdown signals (`SIGINT`, `SIGTERM`, SDL_QUIT) trigger state exit cleanup via `video_quit()` and `gui_free()`.
