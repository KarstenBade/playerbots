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
| `playerbot/ServerFacade.h`/`.cpp` | Per-method `#if defined(VMANGOS)` arms added across ~20 methods (`isSpawned`, `IsFrozen`, `IsInRoots`, `IsCharmed`, `IsFeared`, `IsInFront`, `GetHostileRefManager`, `GetThreatManager`, `SendPacket`, `SendMessageToSet`, `LookupSpellInfo`, `GetSpellInfoRows`, `IsWithinLOSInMap`, `isMoving`, `GetChaseTarget/Angle/Offset`, `GetDistance*`) | vmangos is a genuine third dialect — it matches MANGOS naming for some methods and CMANGOS for others; verified per-method against vmangos's actual API rather than blanket-routing. |
| `playerbot/ServerFacade.cpp`, `playerbot/strategy/actions/BattleGroundTactics.cpp` | `sqrt(...)` around coordinate-based `GetDistance`/`GetDistance2d` calls replaced with `BOTDIST_SQRT(...)` (core repo macro) | vmangos's coordinate-based `GetDistance` overloads already return a real distance (not squared), unlike cmangos's; the macro is `sqrt()` under cmangos and a no-op under vmangos so the call sites stay dialect-agnostic. |
| `playerbot/WorldPosition.h`, `playerbot/GuidPosition.h`, `playerbot/Talentspec.h`, `playerbot/strategy/Value.h` | Various `#ifdef VMANGOS` arms for `CreatureData`/`GameObjectData` position-struct shape (`.position.{mapId,x,y,z,o}` vs flat fields), `GetCreatureData()` const-ness, `HasNpcFlag` field name, `CheckTalents`/`GetBotTalentPoints`, instance vs static `ObjectMgr` calls | Struct-field and API-shape divergences from cmangos; documented in-line at each site. |
| 20 files across `playerbot/` and `ahbot/` (see `git log` on this branch) | `ObjectMgr::GetCreatureTemplate(id)` / `ObjectMgr::GetGameObjectInfo(id)` / `ObjectMgr::GetItemPrototype(id)` (cmangos static-call syntax, ~360 sites) rewritten to `VMANGOS_GET_CREATURE_TEMPLATE(id)` / `VMANGOS_GET_GAMEOBJECT_INFO(id)` / `VMANGOS_GET_ITEM_PROTOTYPE(id)` | vmangos's `ObjectMgr` methods are instance-only (no static overloads). The macros (defined in core repo `VmangosBotCompat.h` for VMANGOS, and as a passthrough to the original static-call syntax in `botpch.h`'s `#else` arm for mangos/cmangos) keep both dialects compiling from the same source text instead of touching ~360 sites individually. |
