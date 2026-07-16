# VMANGOS port — divergence ledger

This branch (`vmangos`) adapts the module to the vmangos core
(https://github.com/vmangos/core). Every deviation from `master`
(celguar upstream) is listed here to keep future upstream merges reviewable.

Ground rules for this branch:
- vmangos builds with defines `CMANGOS + VMANGOS + MANGOSBOT_ZERO`: the
  module follows the cmangos code paths; where vmangos's API differs, the
  site is wrapped in `#if defined(VMANGOS) ... #else ... #endif` (or an
  existing `MANGOS`/`CMANGOS` split is extended).
- Include paths are NEVER rewritten; the vmangos core provides
  cmangos-style forwarding headers (core repo: `src/modules/PlayerbotsCompat/`).
- Keep edits minimal and additive; no reformatting.

## Divergences

| File | Change | Reason |
|------|--------|--------|
| `CMakeLists.txt` | `VMANGOS` guarded block: defines `-DCMANGOS -DVMANGOS -DMANGOSBOT_ZERO` instead of project-name detection; vanilla conf `configure_file` also triggered by `VMANGOS` | vmangos's CMake project is named "MaNGOS" — auto-detection would define `MANGOS` and no expansion |
| `CMakeLists.txt` | `VMANGOS` block also `set(PCH ON)` | The .cpp files never `#include "botpch.h"`; they rely on it being force-included as a PCH. vmangos doesn't set the module's `PCH` var otherwise. |
| `playerbot/PlayerbotAIConfig.cpp` | `#ifdef VMANGOS` uses `Config* configA = &config;` instead of `reinterpret_cast<ConfigAccess*>` | vmangos core `Config` exposes `GetValues()` directly; the reinterpret_cast onto a mirror layout is UB (vmangos `Config` has a different member layout). |
