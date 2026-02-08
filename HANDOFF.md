# Handoff - Party Mode UI & Documentation Update

## Summary
In this session, we significantly improved the user experience for the new Party Game modes. We created dedicated documentation for Monkey Target, Fight, Bowling, and Billiards, and integrated these instructions directly into the game's UI. We also consolidated the project's meta-documentation (Vision, Roadmap, Agents) and standardized the versioning process.

## Completed Tasks
*   **Documentation:** Created `doc/party_games.txt` with detailed rules and controls.
*   **Manual Update:** Linked `doc/manual.txt` to the new party games doc.
*   **UI Enhancement:** Added a `gui_multi` description box to the Party Mode menu in `ball/st_party.c` that dynamically updates based on the selected mode.
*   **Bowling Mode:** Formally exposed `MODE_BOWLING` in the menu cycle.
*   **Meta-Docs:** Created `docs/AGENTS_UNIVERSAL.md`, `docs/VISION.md`, and `docs/DASHBOARD.md`. Updated `ROADMAP.md` and `CHANGELOG.md`.
*   **Versioning:** Bumped version to `1.6.2-dev`.

## Current State
*   **Build:** Passing (verified with `make`).
*   **Version:** `1.6.2-dev`
*   **Active Branch:** `party-games-ui-docs` (local) -> should be merged to `main`.

## Next Steps (Immediate)
1.  **Merge & Deploy:** Commit the current changes and merge to `main`.
2.  **Economy System:** Begin work on persisting player progress (Coins/Bananas) to a save file to support the future "Shop" feature (Phase 3 of Roadmap).
3.  **Campaign:** Develop the "Hub World" levels to utilize the new `GAME_WARP` logic.

## Notes for Next Agent
*   The build environment requires `libsdl2-dev`, `libjpeg-dev`, `libpng-dev`, etc. (see `docs/DASHBOARD.md`).
*   The `VERSION` file is the source of truth for version numbers.
*   Check `ball/progress.h` for mode definitions if adding new game modes.
