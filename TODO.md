# Immediate Action Items (TODO)

## Active Development: Phase 4 Level Editor & Dynamic Elements
- [x] Integrate Level Editor entry into the primary Main Menu hierarchy (`ball/st_title.c`).
- [x] Implement live UI hover tooltips across main menu options.
- [x] Protect headless server and client loops against missing texture/varying crashes (`game_common.c`, `game_draw.c`).
- [x] Implement grid-snapped object placement in `ball/st_edit.c` (selecting from predefined brush pieces).
- [x] Add serialization for custom placed layout objects (`editor_level.txt`).
- [ ] Connect switch triggers (`sol_swch_test`) in `game_server.c` to open goals dynamically.

## Content & Assets
- [ ] Construct themed Hub World brush maps (`data/map-hub/`) with warp trigger zones.
- [ ] Create campaign cutscene scripts (`story.txt`) for World 1 Jungle through World 4 Space.

## Testing & Automation
- [x] Verify headless smoke testing script (`test_run.sh`) covering both Neverball and Neverputt.
- [x] Ensure executable permissions on all shell scripts in `scripts/`.
- [ ] Add automated CI test verifying all 14 party modes initialize without error.
