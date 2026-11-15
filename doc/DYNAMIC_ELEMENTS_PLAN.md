# Phase 2: Dynamic Level Elements - Implementation Plan

## Overview
As outlined in the Phase 2 roadmap and Level Design Plan, reaching Super Monkey Ball parity requires advanced dynamic mechanics. This document outlines the implementation strategy for the code changes needed to support the new level designs.

## 1. Moving and Morphing Stages
To support levels like "Rising Heat" (World 2) or "Centrifuge" (World 3), we need to enhance the `.sol` file processing.
*   **Engine Work:** Extend `share/solid_vary.c` and `share/solid_all.c` to parse custom movement paths injected via map compilation.
*   **Editor Work:** Update `ball/st_edit.c` to allow tagging placed tiles with movement macros (e.g., `M_LIFT`, `M_SPIN`).

## 2. Switches and Triggers
To support puzzle stages like "The Locks" (World 3), we need an interactive switch entity.
*   **Engine Work:** A `b_swch` structure already partially exists. We must fully hook up the `sol_swch_test` collision detection in `game_server.c` so the ball can toggle it.
*   **Logic Work:** When toggled, the switch needs to transmit a state change (via `game_cmd`) to activate a linked moving platform or open a goal.

## 3. Warp Gates
To support "Warp Zone" (World 3), we need instantaneous player teleporters.
*   **Engine Work:** The `b_jump` entity exists but is currently configured as a physical bumper/spring. We need to implement a paired teleporter logic where entering `Jump A` instantly sets the ball position to `Jump B` with a small cooldown.

## Next Steps
1. Create a `doc/mechanics` directory to store detailed specifications for each element.
2. Begin modifying `game_server.c` to support the Switch collision and state tracking.
