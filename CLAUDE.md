See `docs/AGENTS_UNIVERSAL.md` for core instructions.

## Claude Specific Instructions
*   Prioritize deep analytical read-throughs of `game_server.c` and `solid_sim_sol.c` before making physics changes.
*   When implementing UI, always ensure `gui_set_hidden` is utilized correctly to prevent orphaned HUD elements.
*   Leverage your extended context window to cross-reference `TODO.md`, `ROADMAP.md`, and `CHANGELOG.md` simultaneously to ensure perfect synchronization.