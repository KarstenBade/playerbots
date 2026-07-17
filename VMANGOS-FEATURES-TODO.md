# VMANGOS port — remaining work & stubbed features

Status: the module's PCH compiles and the full `.cpp` compile is at
**1862 error occurrences** (build_current11.log), down from 2299 at the start
of the previous session and an initial 5876 once the PCH itself started
compiling. Raw counts are inflated by the same header hitting many TUs; the
distinct-site counts below are the real workload. Fully-cleared categories so
far (0 remaining errors each): `CreatureInfo` field renames, `CreatureData`/
`GameObjectData` nested-position structs, Player/Unit cmangos method aliases,
`AreaTriggerEntry` geometry/teleport split, `TrainerSpell` struct gap,
`GameTele` position fields, and all `ObjectMgr` deltas (locale index, condition
eval, locale-string fillers, gossip text). Each remaining category below has an
established fix pattern (see git log on the `vmangos` branch + VMANGOS-PORT.md).

**Gotcha found: `GetAuraCount` and any core-header inline that dereferences an
incomplete type** must be declared in the header and defined out-of-line in the
.cpp (`SpellAuraHolder` is only forward-declared in Unit.h). Same applies to
other alias helpers if they touch incomplete types.

**Gotcha found this session**: the module builds with `CMANGOS` *and*
`VMANGOS` both defined simultaneously (VMANGOS augments/overrides the cmangos
dialect rather than replacing it). Pre-existing standalone `#ifdef CMANGOS`
blocks (not `#else`-chained) will still compile even under VMANGOS — adding a
naive third `#ifdef VMANGOS` arm next to them causes double-declaration.
Fix pattern: change the `#ifdef CMANGOS` guard itself to
`#if defined(CMANGOS) && !defined(VMANGOS)` before adding the VMANGOS arm
(see `RandomPlayerbotMgr.cpp` `LoadBattleMastersCache` for the example). This
only matters for sites with a *pre-existing* CMANGOS/MANGOS-specific `#ifdef`;
plain unguarded lines are safe to wrap in `#ifdef VMANGOS ... #else ... #endif`
as usual.

## Stubbed features (compile to safe no-ops; marked `// VMANGOS-TODO`)

| Feature / subsystem | Where | Notes |
|---------------------|-------|-------|
| Instance templates | `ObjectMgr::GetInstanceTemplate` (core stub → nullptr) | vmangos has no InstanceTemplate store; bot instance level gating inert. |
| Area name / WMO override | `WorldPosition::getAreaOverride` (returns "") ; `AreaTableEntry`/`AreaNameInfo` stub types | vmangos exposes area via `TerrainInfo::GetAreaId`, not AreaTable.dbc. |
| WorldSafeLocs orientation | `WorldPosition(WorldSafeLocsEntry*)` | vmangos `WorldSafeLocsEntry` has no `o` field; orientation lost. |
| ServerFacade methods matching neither core | `ServerFacade.h/.cpp` | ~14 methods still need per-method `#if defined(VMANGOS)` arms (see below). |

## Done this session (see `git log` on `vmangos` branch + VMANGOS-PORT.md)

- **camelCase → PascalCase aliases**: `getClass/getRace/getGender/getClassMask`,
  `isFrozen/isInRoots/isFeared/isInCombat/addUnitState/clearUnitState/hasUnitState`,
  `HasMana/getAttackers/CanReachWithMeleeAttack/GetSpeedInMotion/GetAttackDistance` (Unit);
  `IsInGroup/CanInteract/learnSpell/getQuestStatusMap/GetPlayerMenu/isTaxiCheater/
  isAFK/IsFreeFlying/IsStunnedByLogout/SendMessageToPlayer/Whisper/GetItemByEntry/
  GetCombinedCombatReach` (Player, +Object for the WorldObject-level overloads);
  `IsRaidGroup/GetTargetIcon` (Group); `getPetType/learnSpell` (Pet);
  `GetTypeId` (BattleGround); `GetFlagCarrierGuid` (BattleGroundWS);
  `isLogingOut` (WorldSession); `IsInUse` (GameObject, stubbed false);
  `GetAverageDiff/GetMaxDiff` (World, approximated from `GetCurrentDiff`);
  `SendPlaySpellVisual` (WorldSession, real per-session packet, defined in .cpp).
- **DIST_CALC_\*** mapped onto vmangos's `SizeFactor` enum; `BOTDIST_SQRT` macro
  handles the squared-vs-real-distance return-value difference on the
  coordinate-based `GetDistance`/`GetDistance2d` overloads.
- **DBC/SQLStorage wrappers** in `VmangosBotCompat.h`: `sItemStorage`,
  `sCreatureStorage`, `sGOStorage`, `sSkillLineAbilityStore`, `sAreaTriggerStore`,
  `sTaxiNodesStore`, `sLootMgr` (real vmangos-side lookups). `sFactionStore`/
  `sAreaStore` are **stubs** (vmangos loads neither Faction.dbc nor
  AreaTable.dbc at all — see Stubbed features above).
- **Enum/constant aliases**: `ITEM_SUBCLASS_POTION/ELIXIR/FLASK/SCROLL/FOOD/
  ITEM_ENHANCEMENT/BANDAGE/CONSUMABLE_OTHER` (re-added; vmangos comments these
  out under vanilla), `ITEM_FLAG_HAS_LOOT`, `ITEM_SPELLTRIGGER_ON_NO_DELAY_USE`
  (approximated as ON_USE), `TEMPSPAWN_TIMED_DESPAWN`, `TRIGGERED_OLD_TRIGGERED`/
  `TRIGGERED_NONE`, `TEAM_INDEX_ALLIANCE/HORDE/NEUTRAL`, `BG_AV_NODE_STATUS_*`
  (mapped onto vmangos's `BG_AV_Event_Control_State`), `MapID`→`id`,
  `NpcFlags`→`npc_flags`, `_Spell`→`_ItemSpell`, `UnitAI`→`CreatureAI`,
  `MaNGOS::AllCreaturesOfEntryInRangeCheck`→`AllCreaturesOfEntryInRange`.
- **Static → instance `ObjectMgr` calls** (~360 sites, 20 files): rewritten to
  `VMANGOS_GET_CREATURE_TEMPLATE`/`VMANGOS_GET_GAMEOBJECT_INFO`/
  `VMANGOS_GET_ITEM_PROTOTYPE` macros (instance-routed under VMANGOS,
  passthrough to the original static syntax under mangos/cmangos).
- **`LootItem::itemId`** (5 real sites — the "104 occurrences" was the same
  header hitting many TUs): per-site `#ifdef VMANGOS` reading `itemid` instead.
- **`AuctionEntry::itemCount`/`itemRandomPropertyId`**: added as real fields on
  the core `AuctionEntry` struct under `ENABLE_PLAYERBOTS` (default 0). The
  PlayerBots ahbot module populates them correctly for auctions it creates
  itself (`ahbot/AhBot.cpp`); auctions from vmangos's native AH flow default to
  0 — VMANGOS-TODO if that path ever needs a real read.

## Done this session (continued)

- **`CreatureInfo` field renames** (~110 call sites across 29 files):
  `Faction`→`faction`, `TrainerType`→`trainer_type`, `Rank`→`rank`,
  `MaxLevel`→`level_max`, `Name`→`name`, `Entry`→`entry`, `MinLevel`→`level_min`,
  `Family`→`pet_family`, `ExtraFlags`→`flags_extra` (+ `CREATURE_EXTRA_FLAG_INVISIBLE`
  →`CREATURE_FLAG_EXTRA_INVISIBLE`), `isTameable()`→`IsTameable()`, per-site
  `#ifdef VMANGOS` (too generic to macro-alias, same reasoning as `LootItem::itemId`).
  Plus safe macro aliases added to `VmangosBotCompat.h` for the non-generic
  fields: `TrainerTemplateId`→`trainer_id`, `VendorTemplateId`→`vendor_id`,
  `TrainerClass`→`trainer_class`, `TrainerRace`→`trainer_race`,
  `GossipMenuId`→`gossip_menu_id`, `LootId`→`loot_id`,
  `PickpocketLootId`→`pickpocket_loot_id`, `SkinningLootId`→`skinning_loot_id`,
  `MinLootGold`→`gold_min`. `CreatureInfo::GetRequiredLootSkill()` (5 sites) has
  no vmangos equivalent (cmangos derives it from `type_flags` HERBLOOT/MININGLOOT
  bits vmangos never loads); replaced with a `VmangosGetRequiredLootSkill()` free
  function in the compat header that always returns `SKILL_SKINNING`
  (VMANGOS-TODO if herb/mining creature-loot is ever needed for classic).
  One genuine upstream bug found in `TravelValues.cpp` (`GameObjectInfo::ExtraFlags`
  — GameObjectInfo never had that field even in cmangos); the check is a no-op
  under VMANGOS now with a comment explaining why.

## Remaining compile-delta categories (as of build_current11.log, 1862 total)

Regenerate the live per-category site list from a fresh build log with:
`grep -oE 'kein Member von "[^"]+"' build_currentN.log | sort | uniq -c | sort -rn`
then per type: `grep 'kein Member von "T"' | sed -E 's/.*error C2039: "([^"]+)".*/\1/' | sort | uniq -c`.

1. **Loot `m_loot` access** (~30 across Unit/Creature/GameObject/WorldObject/Loot
   groups) — **genuine reimplementation**. vmangos stores loot as a by-value
   `Loot loot;` member on Creature/GameObject/Item; the module treats `->m_loot`
   as a `Loot*` pointer (incl. `if (x->m_loot)` and
   `reinterpret_cast<LootAccess const*>(x->m_loot)`, where module-side `LootAccess`
   mirrors `Loot`'s private layout). Plan: add `Loot* const m_loot = &loot;` to
   core Creature/GameObject/Item under `ENABLE_PLAYERBOTS` (fixes the concretely-
   typed sites; note `if (x->m_loot)` becomes always-true — acceptable, downstream
   handles empty loot). The base-pointer sites (`unitTarget->m_loot` Unit*,
   `wo->m_loot`/`object->m_loot` WorldObject* in `PlayerbotAI.cpp:311/7158`,
   `LootValues.cpp:371/404/407`) need per-site resolution to the concrete
   Creature/GameObject first.
2. **`PathInfo` pathfinding API** (~23: `setAreaCost` 12, `setArea` 5, `getArea` 4,
   `getFlags` 1, `ComputePathToRandomPoint` 1) — cmangos PathFinder extensions
   (`src/game/Maps/PathFinder.h` `class PathInfo`) that vmangos lacks.
   `setAreaCost(area,cost)` maps to the internal `dtQueryFilter m_filter`'s
   `setAreaCost`; the rest (`setArea`/`getArea`/`getFlags`/`ComputePathToRandomPoint`,
   mostly `DebugAction.cpp`) are module-specific and need core additions or
   module-side stubs. Investigate `m_filter` accessors before deciding.
3. **`Map`** (~18: `GetReachableRandomPointOnGround` 3, `GetObjectsStore` 3,
   `GetGraveyardManager` 3, `IsMountAllowed` 2, `ForceLoadGrid` 2,
   `HasActiveZones`/`HasActiveZone`/`GetTransports` 1 each) — per-method; some
   have vmangos equivalents, some need stubs.
4. **`MotionMaster`** (~12: `MovePath` 6, `MoveFall` 3, `PauseWaypoints`/
   `MoveInFormation`/`DistanceYourself` 1 each) — movement generators with
   different vmangos signatures; per-method alias or stub.
5. **`CreatureAI`** (~12), **`Creature`** (~12), **`GameObject`** (~10),
   **`WorldObject`** (~7) — per-method; not yet broken down.
6. **`WorldSession` typed opcode handlers** (161 raw, e.g.
   `HandleGameObjectUseOpcode(WorldPackets::Misc::GameObjectUse const&)` vs the
   module's `WorldPacket&`) — **genuine per-handler adaptation**, the single
   largest raw chunk. Build the `WorldPackets::*` struct from the `WorldPacket`
   at each call site, or add cmangos-signature overloads under `ENABLE_PLAYERBOTS`.
7. **Locale / broadcast_text data model** — `GossipText` was stubbed
   (see Stubbed features); other locale gaps may surface similarly.
8. **Misc**: `GameTele` done; `std` (8), `MailItemInfo` (8), `MMAP::MMapManager`
   (7), `BattleGroundMgr` (7), `ChannelMgr` (6), `Spell` (5), `LFGQueue` (5),
   `ai::BotUseItemSpell::m_clientCast` (6) — small per-subsystem clusters, each
   needs a quick look. Syntax-error clusters (`C2143`/`C2059`) are likely
   cascades from the above; re-triage after.

### Stubbed / reimplementation-deferred this session (all marked `VMANGOS-TODO`)

- Core `Player.h`/`Unit.h` stubs: `learnClassLevelSpells`, `TakeQuestSourceItem`,
  `Get/SetDividerGuid` (quest sharing), `GetMountInfo` (returns null),
  `setCinematic`, `SetAtLoginFlag`, `SetCanFly`, `CanEnterNewInstance` (allows),
  `isHonorOrXPTarget` (true), `isRessurectRequested` (false), `isGMChat`,
  `IsTaxiDebug`, `GetHonorRankInfo`/`GetHonorHighestRankInfo` (rank 0),
  `CanAttackOnSight`, `GetCollisionWidth`, `InterruptMoving`, `IsFacingTargetsBack`,
  `IsInSwimmableWater`, `IsInTeam`. Mail box accessors (`GetMailBegin`/`End`/
  `Size`/`GetMItem`/`RemoveMail`/`RemoveMItem`) stubbed to an empty box — vmangos
  routes mail through `MasterPlayer`, not `Player`; `SendMailResult` is a real
  `WorldSession` packet. Taxi flight-spline (`GetTaxiPathSpline` returning
  `Taxi::Map`) `#ifdef`-disabled at 2 sites (`GoAction.cpp`, `MovementActions.cpp`).
- `TrainerSpell::learnedSpell`/`isProvidedReqLevel`/`conditionId`: added as
  defaulted core fields under `ENABLE_PLAYERBOTS` (0/false) — bot trainer logic
  compiles and degrades gracefully; populate from vmangos trainer tables if parity
  is ever needed.
- `GossipText`: vmangos moved gossip text to `broadcast_text` (referenced by
  `NpcText::Options[i].BroadcastTextID`); module gets a compat `GossipText` type +
  `VmangosGetGossipText()` returning a shared empty instance (safe vs the unguarded
  `->Options->Text_0` deref in `SayAction`).
- `ObjectMgr` locale-string fillers (`GetQuestLocaleStrings`/`GetItemLocaleStrings`/
  `GetCreatureLocaleStrings`): no-op stubs (bots use base/English locale).
