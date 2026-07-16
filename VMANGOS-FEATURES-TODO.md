# VMANGOS port — remaining work & stubbed features

Status: the module's PCH compiles and the full `.cpp` compile has gone from
total failure to **2299 error occurrences** (build21.log), down from an
initial 5876 once the PCH itself started compiling. Nearly every remaining
occurrence falls into the mechanical categories below (most now have very
few *distinct* source sites — the raw error count is inflated by the same
header being pulled into many translation units). Each category has an
established fix pattern; several were completed this session (see git log
on the `vmangos` branch and VMANGOS-PORT.md).

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

## Remaining compile-delta categories (mechanical, patterns established)

1. **`CreatureInfo` field renames still open** (~80 occurrences, ~45 distinct
   sites across ~20 files — use `grep -oE '[A-Za-z]:[\\][^:]+\.(cpp|h)\([0-9]+,[0-9]+\)'`
   on a fresh build log per field to get the current site list):
   `Faction`→`faction`, `TrainerType`→`trainer_type`, `Rank`→`rank`,
   `MaxLevel`→`level_max`, `Name`→`name`, `Entry`→`entry`. **Not macro-aliased**
   on purpose — these names are too generic (`Name`, `Entry`, `Rank`) and
   collide with unrelated identifiers throughout the module; needs real
   per-site `#ifdef VMANGOS` edits like the `LootItem::itemId` fix above.
2. **`GameObjectData`/`CreatureData`/`AreaTriggerEntry` position-struct shape**
   at sites not yet touched (`GameObjectData::mapid/posX/posY/posZ` ~42 occ.,
   `CreatureData::id/mapid` ~16 occ.): same `.position.{mapId,x,y,z,o}` pattern
   already applied in `WorldPosition.h`/`GuidPosition.h`, just at new sites
   (`PlayerbotAI.cpp`, `TravelNode.cpp`, `RandomPlayerbotMgr.cpp`, etc.).
3. **`AreaTriggerEntry::target_mapId/target_X/Y/Z/Orientation`** (~13 sites,
   `PlayerbotAI.cpp`, `TravelNode.cpp`, `MovementActions.cpp`,
   `WorldPosition.h`) — **genuine reimplementation, not a rename**. cmangos's
   `AreaTriggerEntry` (from `ObjectMgr::GetAreaTrigger`) carries both trigger
   geometry AND teleport destination in one flat struct; vmangos splits these
   into `AreaTriggerEntry` (geometry only) + a separately-looked-up
   `AreaTriggerTeleport` (`sObjectMgr.GetAreaTriggerTeleport(id)`, destination
   in a nested `WorldLocation destination`). Each site needs a second lookup
   call threaded in, not just a field rename.
4. **`TrainerSpell::learnedSpell`** (13 occ.) — check vmangos's actual
   trainer-spell struct field name.
5. **Typed opcode handlers**: vmangos `WorldSession::HandleGameObjectUseOpcode`
   (161 occ.) takes `WorldPackets::Misc::GameObjectUse const&`, not
   `WorldPacket`. Real per-handler adaptation, not a simple alias.
6. **`ai::BotUseItemSpell::m_clientCast`**, **`PathInfo::setAreaCost`** — small
   (6/10 occ.) but not yet investigated.
7. Misc syntax-error clusters (`C2143`/`C2059`/`C2146`, ~100 occ. combined) are
   likely cascades from one or two of the above root causes rather than
   independent bugs — re-triage after the categories above are fixed, since
   the line/column will likely have shifted.
