# Universal AI Agent Instructions

This document consolidates instructions for all AI agents working on the Neverball repository. All agents (Claude, GPT, Gemini, Copilot, etc.) must adhere to these guidelines.

## 1. Core Philosophy
*   **Goal:** Achieve complete feature parity with the Super Monkey Ball series (SMB 1, 2, Deluxe, Banana Blitz/Mania) while maintaining the open-source legacy.
*   **Quality:** "Extremely robust," "no bugs," "full detail," "comprehensive documentation."
*   **Persistence:** "Don't ever stop. Don't ever quit." Continue implementing features autonomously.

## 2. Documentation & UI
*   **UI Representation:** Every feature must be fully represented in the UI with tooltips, labels, and descriptions. No hidden functionality.
*   **Manuals:** Comprehensive documentation in `doc/manual.txt`, help files, and specific feature docs (e.g., `doc/party_games.txt`).
*   **Input Documentation:** Document all user input and clarifications to ensure the project direction aligns with the vision.

## 3. Versioning & Changelog
*   **Single Source of Truth:** The version number must reside in a single text file (e.g., `VERSION` or `CHANGELOG.md`), not hardcoded in C source files. Code should read this file.
*   **Incrementing:** Every build/session that produces changes should increment the version number (e.g., `1.6.0` -> `1.6.1-dev`).
*   **Changelog:** Maintain a detailed `CHANGELOG.md`. Sync all version references to this file.

## 4. Git & Workflow
*   **Feature Branches:** Merge all local/AI-created feature branches into `main`.
*   **Submodules:**
    *   Update all submodules recursively.
    *   Merge upstream changes (including forks).
    *   Document all submodules in `docs/DASHBOARD.md`.
*   **Commit Messages:** Descriptive messages referencing the version bump and specific changes.
*   **Continuous Progress:** Commit, push, and proceed to the next feature without pausing for confirmation if possible.

## 5. Project Structure
*   **Source:** `ball/` (Core Game), `putt/` (Minigolf), `share/` (Engine/Shared).
*   **Docs:** `doc/` (User Manuals), `docs/` (Dev Docs, Dashboard, Roadmap).
*   **Data:** `data/` (Assets, Levels).

## 6. Dashboard
*   Maintain `docs/DASHBOARD.md` listing:
    *   Project directory structure explanation.
    *   List of all submodules with versions, dates, and build numbers.

## 7. Handover Protocol
*   Before ending a session:
    1.  Merge branches/Update submodules.
    2.  Update Roadmap/Docs.
    3.  Update Dashboard.
    4.  Update Changelog & Version.
    5.  Commit & Push.
    6.  Create a `HANDOFF.md` with session history and memory.
