# Level Editor Research

## Current State
*   Neverball levels use the Quake 3 style `.map` format for level geometry (brushes).
*   These are compiled into `.sol` (binary format) by the `mapc` tool located in `share/mapclib.c`.
*   Currently, developers and mappers use external tools like **TrenchBroom** or **GtkRadiant** to create geometry, export it to `.map`, and run the build process.

## Feasibility of an In-Game Editor
Building a fully-featured constructive solid geometry (CSG) editor inside the Neverball engine (which uses immediate/vertex array OpenGL calls) is a massive undertaking requiring:
1.  A new 3D viewport UI state.
2.  Mouse picking, translation, scaling, and rotation widgets (Gizmos).
3.  CSG math logic for brush intersections and portal generation (BSP).
4.  Real-time recompilation into `s_base` structures for physics.

## Recommendation: The "Tile-Based" Approach
Rather than a full CSG editor, an in-game editor should be **tile-based**, akin to Trackmania or Mario Maker.
*   **Prefabs:** The engine loads a library of pre-compiled `.sol` chunks (e.g., straight track, curve, ramp, loop).
*   **Grid Placement:** The user places these tiles on a 3D grid.
*   **Dynamic Loading:** The engine stitches these tiles together by concatenating the vertex/face data and updating the `s_base` pointers, or by maintaining multiple `s_vary` instances.

## Next Steps
1.  Define a set of 10-20 base tiles (Straight, Turn, Ramp Up, Checkpoint, Goal, Item Box).
2.  Implement a grid snapping UI.
3.  Build a simple JSON/XML format to save/load these tile-based layouts.
