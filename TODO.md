# Immediate Action Items (TODO)

## Active Development: Phase 4 Level Editor & Dynamic Elements
- [x] Integrate Level Editor entry into the primary Main Menu hierarchy (`ball/st_title.c`).
- [x] Implement live UI hover tooltips across main menu options.
- [x] Protect headless server and client loops against missing texture/varying crashes (`game_common.c`, `game_draw.c`).
- [x] Implement grid-snapped object placement in `ball/st_edit.c` (selecting from predefined brush pieces).
- [x] Add serialization for custom placed layout objects (`editor_level.txt`).
- [x] Implement wireframe 3D rendering for placed editor tiles (`editor_render_tiles`).

## Content & Assets
- [ ] Construct themed Hub World brush maps (`data/map-hub/`) with warp trigger zones.
- [x] Create campaign cutscene scripts (`data/story/campaign_intro.txt`) wired to `st_story.c`.

## Testing & Automation
- [x] Verify headless smoke testing script (`test_run.sh`) covering both Neverball and Neverputt.
- [x] Ensure executable permissions on all shell scripts in `scripts/`.
- [ ] Add automated CI test verifying all 14 party modes initialize without error.
