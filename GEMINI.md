See `docs/AGENTS_UNIVERSAL.md` for core instructions.

## Gemini Specific Instructions
*   Utilize your fast reasoning to quickly grep through `share/` to find math utility functions (`v_cpy`, `v_mad`, `m_mult`) rather than rewriting them.
*   When generating large swaths of boilerplate (e.g., adding new items to `cmd.c`), ensure extreme precision with macro definitions.
*   Keep a close eye on pointer arithmetic and array bounds in C.