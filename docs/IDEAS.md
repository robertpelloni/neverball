# Neverball Ideas & Brainstorming

This document outlines creative, constructive, and potentially radical ideas to improve the Neverball project from every perspective.

## 1. Architectural Refactoring & Porting
*   **The Rust Rewrite:** Gradually port critical math and physics functions in `share/` from C to Rust. This would provide memory safety, modern tooling (Cargo), and easier WebAssembly compilation.
*   **Entity Component System (ECS):** Refactor the monolithic `v_ball` and `server_player` structs into an ECS architecture. This would drastically simplify adding new power-ups, status effects, and minigame-specific logic without bloating the core structs.
*   **Vulkan/Metal Renderer:** Abstract the legacy OpenGL 1.4/2.0 rendering code into a generic graphics API layer, allowing for modern Vulkan (Linux/Windows) and Metal (macOS) backends, unlocking better performance and shader effects.

## 2. Gameplay Pivots & Expansions
*   **"Neverball Maker" (In-Game Level Editor):** Integrate a simplified, grid-based level editor directly into the game UI, allowing players to build, play, and share levels without needing TrenchBroom or Radiant.
*   **Rogue-lite Campaign Mode:** Create a randomized "Tower" mode where players face a sequence of generated/selected levels, earning temporary power-ups (jump, dash, shrink) between floors, with perma-death.
*   **VR Support:** Implement native OpenXR support. The camera already acts as a follow-cam; locking it to a headset and allowing tilt via VR motion controllers could provide a highly immersive (and dizzying) experience.

## 3. Minigame Concepts (Beyond SMB)
*   **Monkey Katamari:** A party mode where balls start small and must roll over scattered items (coins, bananas, geometric shapes) to grow larger, eventually allowing them to consume other players.
*   **Monkey Rocket League:** Evolve "Monkey Soccer" by enabling the Jump and Spin Dash mechanics permanently, allowing for aerial hits and wall-riding in a fully enclosed arena.
*   **Monkey Fall Guys:** An obstacle course race mode for 16-32 players (AI or Networked) featuring spinning arms, collapsing floors, and seesaws.

## 4. Quality of Life & Polish
*   **Dynamic Music System:** Implement a music engine that layers tracks based on game state (e.g., adding a fast percussion track when the timer drops below 10 seconds).
*   **Rewind Feature:** In single-player, allow the player to hold a button to reverse time (Braid/Forza style) at the cost of a final score penalty, making difficult levels more accessible.
*   **Rich Presence:** Integrate Discord Rich Presence to show what level or party game the user is currently playing.
