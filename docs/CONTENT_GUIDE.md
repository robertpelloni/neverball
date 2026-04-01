# Content Creation Guide

This guide outlines how to create levels and assets for the new game modes in Neverball (Soccer, Tennis, Hub World).

## 1. Map Format & Tools
Neverball uses the `.map` format (Quake 3 style brushes). Levels are created using **GtkRadiant** or **NetRadiant** and compiled using the `mapc` tool included in the repository.

### Compilation
To compile a map:
```bash
./mapc data/levels/mylevel.map data/
```
This generates a `.sol` (geometry) file.

## 2. Creating Party Game Levels

### Monkey Soccer
*   **Requirements**: A large, flat rectangular field.
*   **Dimensions**: Approximately 40x80 units.
*   **Goals**: Create visual goal posts at `Z = +20` and `Z = -20`.
    *   *Note:* The game logic hardcodes goal detection at `Z > 20.0f` and `Z < -20.0f`. Ensure your geometry aligns with this.
*   **Spawns**:
    *   Soccer Ball: `(0, 5, 0)` (Index 0).
    *   Red Team: `Z < 0`.
    *   Blue Team: `Z > 0`.

### Monkey Tennis
*   **Requirements**: A standard tennis court layout.
*   **Net**: A physical barrier at `Z = 0`, height `~1.0` unit.
*   **Bounds**:
    *   Out of bounds logic checks `Z > 15.0f` and `Z < -15.0f`.
    *   Ensure the court visual lines match these coordinates.

## 3. Creating Hub Worlds
*   **Structure**: A central area with paths leading to switches.
*   **Switches**: Place `switch` entities.
    *   **Logic**: Switch Index 0 warps to Level 1, Index 1 to Level 2, etc.
*   **Signs/Text**: Use textures to indicate level numbers or names near switches.

## 4. Character Stats
To create a new character:
1.  Create a directory: `data/ball/my-character/`.
2.  Add a model (`my-character.sol`) and texture.
3.  Create `stats.txt`:
    ```
    speed 1.0
    acceleration 1.2
    jump 1.0
    control 0.8
    cost 500
    ```
