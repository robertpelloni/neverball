# Online Multiplayer Feasibility Study

## Current Architecture
*   Neverball currently relies on a `game_proxy` queue to pass commands (`CMD_BALL_POSITION`, `CMD_STATUS`, etc.) from the server (`game_server.c`) to the client (`game_client.c`).
*   Physics calculations are deterministic within `solid_sim_sol.c` using a fixed timestep (`DT`).
*   Inputs are collected and polled in `st_play.c` before being pushed to the server variables.

## Challenges
1.  **Deterministic Physics:** While the engine uses a fixed timestep, floating-point math across different CPU architectures (x86 vs ARM) may drift over time, making lockstep networking difficult without significant re-engineering of the math libraries (e.g., using fixed-point math).
2.  **Input Latency:** Monkey Ball relies on extremely precise tilt inputs. Any server-side latency would result in noticeable input lag, making the game unplayable.
3.  **State Size:** Synchronizing the full state of every object and ball (`v_ball`) every frame is bandwidth-intensive.

## Proposed Solution: Authoritative Server with Client Prediction
*   **Architecture:** The server runs the definitive physics simulation. Clients send inputs (Tilt X, Z, Action) to the server.
*   **Prediction:** The client immediately applies its own inputs locally using a copied `vary` state.
*   **Rollback:** When the client receives the authoritative state from the server (which is slightly in the past due to ping), it compares it to its historical state. If they diverge, the client snaps back to the server state and quickly simulates forward using stored inputs.

## Next Steps
1.  Isolate the physics state into a serialize-able block.
2.  Implement a UDP wrapper (e.g., ENet) around the `game_proxy` queue.
3.  Begin testing with "Ghost Data" over UDP before attempting live physics correction.
