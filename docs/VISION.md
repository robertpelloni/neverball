# Neverball Project Vision

## Executive Summary
The ultimate goal of the Neverball project is to evolve the classic open-source rolling ball game into a feature-complete spiritual successor to both the *Super Monkey Ball* (SMB) series and the *Marble Blast* (MB) series. The goal is to create the greatest marble game ever made by synthesizing the best mechanics, modes, and physics from these genre-defining titles while maintaining Neverball's open-source legacy.

This vision encompasses both the precision platforming of SMB, the power-up driven exploration of Marble Blast, and the extensive "Party Game" library that defined the genre.

## Core Design Pillars

### 1. Robust Gameplay Foundation
*   **Physics Parity:** Offer a choice between Neverball's classic "heavy/inertial" physics and a new "Arcade Physics" mode that mimics the snappy, high-friction feel of SMB 1/2.
*   **Input Precision:** Support modern controllers with precise analog input, rumble feedback, and customizable deadzones.
*   **Camera Control:** Implement intelligent camera behaviors (auto-follow, snap-to-back, fixed angles) to reduce player frustration and focus on platforming skill.

### 2. Comprehensive Feature Set (SMB & MB Parity)
*   **Mechanics:**
    *   **SMB:** Tilt, Jump (Banana Blitz), Spin Dash.
    *   **Marble Blast:** Super Jump, Super Speed, Gyrocopter, Gravity Modifiers, Blast.
*   **Main Game:** Timer-based levels, bonus stages, warp gates, and moving platforms.
*   **Party Games:** A full suite of multiplayer minigames:
    *   **Monkey Target:** Flight physics, landing zones, wind, and items.
    *   **Monkey Fight:** Knockback physics, power-ups (big punch, speed), and varied stages.
    *   **Monkey Race:** Kart-style racing with items and shortcuts on existing levels.
    *   **Monkey Billiards:** 9-ball, 8-ball, and trick shot challenges.
    *   **Monkey Bowling:** Lane physics, spin control, and 10-pin scoring.
    *   **Monkey Golf:** Stroke play and mini-golf modes.
*   **Unlockables:** A persistent economy (Bananas/Coins) to unlock characters, costumes, ball skins, and game modes.

### 3. Polish & Documentation
*   **UI/UX:** Every feature must be clearly represented in the user interface. No hidden mechanics. Use tooltips, descriptive labels, and intuitive menus.
*   **Manuals:** Maintain high-quality, up-to-date documentation (`doc/manual.txt`, `doc/party_games.txt`) accessible from within the game or launcher.
*   **Visual Feedback:** Clear indicators for speed, altitude (in flight), charge power (in bowling/billiards), and item status.

## Long-Term Roadmap
1.  **Phase 1: Foundation (Current):** Establish party game modes (Target, Fight, Race), Arcade Physics toggle, and robust documentation.
2.  **Phase 2: Expansion:** Add depth to party games (AI opponents, more maps), implement campaign features (cutscenes, hub worlds), and refine camera logic.
3.  **Phase 3: Polish:** Character stats system, Shop/Unlock UI, and graphical enhancements (particle effects, modern shaders).
4.  **Phase 4: Community:** Online multiplayer networking, level editor integration, and global leaderboards.

## Development Mantra
"Don't ever stop. Don't ever quit."
We strive for a codebase that is clean, modular, and extensively documented, ensuring that future contributors (human or AI) can pick up the torch instantly.
