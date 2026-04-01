# Feature Parity Research: Super Monkey Ball & Marble Blast

This document catalogs the features, mechanics, and modes from the *Super Monkey Ball* (SMB) and *Marble Blast* (MB) series to guide Neverball's development towards complete feature parity.

## 1. Physics & Mechanics

### Super Monkey Ball Series
*   **Tilt Control:** The core mechanic. The world tilts, gravity moves the ball. (Implemented)
*   **Jump (Banana Blitz/Mania):** An active jump button. (Implemented: `can_jump` logic)
*   **Spin Dash (Sonic):** Charge up a dash for speed. (Implemented: `spin_charge` logic)
*   **Edge Clipping:** High-level play involves clipping edges to gain speed. (Physics engine nuance)
*   **Friction/Acceleration:** SMB 1/2 are "snappy" (high accel/friction), later games are "floaty". (Implemented: `stats.txt` allows tuning)

### Marble Blast Series (Gold / Ultra)
*   **Super Jump:** A powerup that launches the marble vertically. (Implemented: Jump stat)
*   **Super Speed:** A powerup that boosts speed significantly. (Implemented: `ITEM_SPEED`)
*   **Shock Absorber:** Prevents bouncing/damage from falls. (TODO)
*   **Gyrocopter:** Allows slow falling/gliding. (Implemented: `MODE_TARGET` flight physics are similar)
*   **Mega Marble:** Increases mass and size, crushes obstacles. (Implemented: `ITEM_GROW` changes size/physics)
*   **Gravity Modifiers:** Gravity Inverters or directional gravity zones. (TODO)
*   **Friction Surfaces:** Ice (low friction), Grass/Mud (high drag). (Implemented: Material system supports friction)
*   **Blast:** An active ability to "blast" away from surfaces in MB Ultra. (Similar to Jump/Dash)

## 2. Game Modes & Party Games

### Super Monkey Ball Party Games
*   **Monkey Target:** Gliding onto scoring zones. (Implemented: `MODE_TARGET`)
*   **Monkey Fight:** Knocking opponents off. (Implemented: `MODE_FIGHT` + Punch)
*   **Monkey Race:** Kart-style racing. (Implemented: `MODE_BATTLE` has race logic)
*   **Monkey Bowling:** 10-pin bowling. (Implemented: `MODE_BOWLING`)
*   **Monkey Billiards:** 9-ball/8-ball. (Implemented: `MODE_BILLIARDS`)
*   **Monkey Golf:** Mini-golf. (Partially via Neverputt, but need single-engine integration)
*   **Monkey Boat:** Paddling mechanics. (TODO)
*   **Monkey Shot:** FPS rail shooter. (TODO)
*   **Monkey Tennis/Baseball/Soccer:** Sports variations. (TODO)

### Marble Blast Modes
*   **Gem Hunt:** Collect all gems before time runs out. (Implemented: `MODE_NORMAL` is essentially this)
*   **Multiplayer Hunt:** Competitive gem collection. (Implemented: `MODE_BATTLE`)

## 3. Meta-Game & Economy

*   **Currency:** Bananas/Coins. (Implemented: `profile.c`)
*   **Shop:** Unlock characters/skins. (Implemented: `st_char.c`)
*   **Story Mode:** Cutscenes and Hub Worlds. (Implemented: Hub logic in `game_server.c`, `st_story.c`)
*   **Stats:** Characters with different Speed/Jump/Accel. (Implemented: `stats.c`)

## 4. Feature Gap Analysis (Remaining)

| Feature | Source | Priority | Notes |
| :--- | :--- | :--- | :--- |
| **Powerup: Gyrocopter** | MB | Medium | Usable in standard levels (not just Target). |
| **Powerup: Shock Absorber** | MB | Low | visual effect + bounce dampening. |
| **Mechanic: Gravity Zones** | MB | High | Triggers that change gravity vector. |
| **Mode: Monkey Golf** | SMB | High | Integrate Neverputt physics into main engine. |
| **Mode: Monkey Boat** | SMB | Low | Water physics/paddling input. |
| **Visuals: Trails/Particles** | MB | Medium | Speed trails, dust, spark effects. |
| **UI: Radar/Minimap** | Both | High | Essential for Race/Hunt modes. |
