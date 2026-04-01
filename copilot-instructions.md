See `docs/AGENTS_UNIVERSAL.md` for core instructions.

## Copilot Specific Instructions
*   When autocompleting within `gui_*.c` functions, follow the `vstack`/`hstack` paradigm established by the original author.
*   Prefer `SDL_Log` or the internal `log_printf` over `printf` for debugging output.
*   Ensure generated comments match the succinct, descriptive style of the existing codebase.