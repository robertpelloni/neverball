# Remaining Tasks (TODO)

## Future Tech (Phase 4)
- [ ] **Level Editor:** Implement a basic tile-based layout editor inside `st_edit` using precompiled `.sol` chunks. Currently exists as a free-flight prototype.
- [ ] **Online Multiplayer:** Isolate the physics state into a serialize-able block and wrap the `game_proxy` queue with UDP/ENet logic.
- [ ] **Ghost Data Sharing:** Build a simple web service backend to host and download `.gho` files natively from the Replay Browser.

## Content Creation
- [ ] **Hub World Maps:** Level designers need to build `.map` files containing 3D models for Hub Worlds, integrating the `GAME_WARP` switches.
- [ ] **Story Scripts:** Write `story.txt` files mapping out the campaign narrative to be parsed by `st_story.c`.
