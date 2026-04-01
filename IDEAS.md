# IDEAS: Future Directions for Neverball

This document contains a brainstorming list of potential improvements, refactors, and pivots for the Neverball project, ranging from practical features to "Pure Autism" level overhauls.

## 1. Architectural Overhauls
*   **Port to Rust / Zig:** Rewrite the core engine in a memory-safe language. The current C99 codebase is robust but susceptible to memory leaks (as seen in `game_server.c`).
*   **ECS (Entity Component System):** Refactor the `server_player` / `v_ball` / `v_item` structures into a proper ECS (e.g., using `flecs`). This would make adding new mechanics (like "Monkey Tennis" racquets) much cleaner than hardcoding them in `game_server.c`.
*   **Scripting Language:** Embed Lua or Python for level logic. Currently, logic is hardcoded in C (`game_server.c` modes). Scripting would allow level designers to create "Puzzle Rooms" (SMB2 style) without recompiling the engine.

## 2. Rendering & Graphics
*   **Vulkan / DX12 Backend:** Replace the legacy OpenGL 1.x/2.x pipeline with a modern bgfx or SDL_GPU backend.
*   **PBR Materials:** Support Physics-Based Rendering for "Marble Blast" style shiny marbles and realistic wood/stone textures.
*   **Ray Tracing:** Real-time reflections on the marble.
*   **VR Support:** Update the HMD implementation (currently LibOVR/OpenHMD) to OpenXR for universal headset support. "First Person Marble" mode.

## 3. Gameplay Mechanics (The "Ultimate Marble Game")
*   **Gravity Modifiers (Marble Blast):**
    *   Arbitrary gravity vectors.
    *   Camera auto-alignment to gravity (wall walking).
*   **Surface Physics:**
    *   **Ice:** Low friction, slippery.
    *   **Mud/Sand:** High drag, slows ball.
    *   **Glue:** Stick to walls.
    *   **Conveyor Belts:** Add velocity to contact manifold.
*   **Morph Ball:**
    *   Ability to change size/mass dynamically.
    *   "Metal Mario" style heavy state (sink in water).
*   **Boss Battles:**
    *   Large entities with hitboxes and health bars.
    *   Scripted attack patterns (Bullet hell phases).

## 4. Content Creation
*   **Procedural Level Generation:** "Endless Roll" mode. Generate track segments randomly.
*   **In-Game Editor:** A block-based level builder (like Trackmania) for rapid user content creation.
*   **Workshop Support:** Integration with Mod.io or Steam Workshop for level sharing.

## 5. User Interface
*   **ImGui Integration:** Replace the custom `gui.c` system with Dear ImGui for debug tools and complex editors.
*   **Theme Engine:** Allow CSS-like styling for the HUD and Menus.

## 6. Networking
*   **Rollback Networking (GGPO):** For lag-free online Monkey Tennis/Fight.
*   **Dedicated Server:** Headless build target for hosting 24/7 Race lobbies.

## 7. Crazy Pivots
*   **"NeverKart":** Reuse the physics engine for a Kart Racer. The ball becomes a wheel?
*   **"Monkey RPG":** Open world hub where you roll between towns, upgrade your ball stats, and enter dungeons (levels).
