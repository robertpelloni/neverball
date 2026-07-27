# Phase 2: Level Design Plan

## Overview
To achieve Super Monkey Ball feature parity, Neverball's campaign must transition from abstract, standalone levels to themed "Worlds." This document outlines the planned level layouts, environments, and mechanical progressions for the new Campaign Engine.

## Themed Worlds Strategy
Instead of a single linear progression, levels will be grouped into distinct Worlds. Each World introduces specific mechanics and culminates in a challenging finale or transition stage.

### World 1: The Jungle (Beginner)
*   **Theme:** Lush greens, tribal ruins, bright skybox.
*   **Difficulty:** Very Easy.
*   **Mechanics Introduced:** Basic movement, simple straight lines, gentle curves, wide platforms.
*   **Layout Concepts:**
    *   *Stage 1-1:* "Welcome" - A straight path introducing bananas (coins) and the goal tape.
    *   *Stage 1-5:* "Bumps" - Small hills introducing vertical velocity management.
    *   *Stage 1-10:* "The Gap" - A small gap requiring the player to carry momentum to cross.

### World 2: The Volcano (Intermediate)
*   **Theme:** Red and orange hues, lava hazards, dark cavernous skybox.
*   **Difficulty:** Normal.
*   **Mechanics Introduced:** Narrow paths, moving platforms (translation), basic bumpers.
*   **Layout Concepts:**
    *   *Stage 2-1:* "The Bridge" - A long, narrow bridge over a hazard zone.
    *   *Stage 2-5:* "Pendulum" - Navigating past swinging hazards.
    *   *Stage 2-10:* "Rising Heat" - Platforms that slowly move up and down, requiring timing.

### World 3: The Ocean / Underwater (Advanced)
*   **Theme:** Blue water, coral reefs, aquatic life.
*   **Difficulty:** Hard.
*   **Mechanics Introduced:** Switches, warp gates, complex moving geometry (rotation).
*   **Layout Concepts:**
    *   *Stage 3-1:* "Warp Zone" - A maze where the player must use color-coded warp gates.
    *   *Stage 3-5:* "Centrifuge" - A massive rotating cylinder the player must traverse internally.
    *   *Stage 3-10:* "The Locks" - The player must trigger multiple switches to open the path to the goal.

### World 4: The Space Station (Expert)
*   **Theme:** Metallic textures, neon lights, starry skybox.
*   **Difficulty:** Very Hard.
*   **Mechanics Introduced:** Conveyor belts, zero-gravity zones, rapid sequential inputs.
*   **Layout Concepts:**
    *   *Stage 4-1:* "Conveyors" - Paths that push the ball, requiring counter-steering.
    *   *Stage 4-5:* "Pinball" - Dropping down through a vertical maze of bumpers.
    *   *Stage 4-10:* "The Gauntlet" - A long, unforgiving stage combining all previous mechanics.

## Integration with Level Editor (`st_edit.c`)
As these layouts are designed, they will be built and tested using the Phase 4 Level Editor prototype. Designers will use the grid-snapping and tile placement features to quickly iterate on these concepts before finalizing them in the `.sol` compiler.
