# VMANGOS port — remaining work & stubbed features

Status: the module's precompiled header (botpch.h → all module headers)
compiles. The 465 `.cpp` files still fail against vmangos APIs. All the
remaining failures are cmangos→vmangos API deltas that fall into the
mechanical categories below. Each has an established fix pattern.

## Stubbed features (compile to safe no-ops; marked `// VMANGOS-TODO`)

| Feature / subsystem | Where | Notes |
|---------------------|-------|-------|
| Instance templates | `ObjectMgr::GetInstanceTemplate` (core stub → nullptr) | vmangos has no InstanceTemplate store; bot instance level gating inert. |
| Area name / WMO override | `WorldPosition::getAreaOverride` (returns "") ; `AreaTableEntry`/`AreaNameInfo` stub types | vmangos exposes area via `TerrainInfo::GetAreaId`, not AreaTable.dbc. |
| WorldSafeLocs orientation | `WorldPosition(WorldSafeLocsEntry*)` | vmangos `WorldSafeLocsEntry` has no `o` field; orientation lost. |
| ServerFacade methods matching neither core | `ServerFacade.h/.cpp` | ~14 methods still need per-method `#if defined(VMANGOS)` arms (see below). |

## Remaining compile-delta categories (mechanical, patterns established)

1. **camelCase → PascalCase method renames** used unconditionally by the module.
   Fix: guarded (`#ifdef ENABLE_PLAYERBOTS`) inline aliases on the core class.
   Examples (with occurrence counts at last build):
   - `Unit/Player::getClass` (533) → `GetClass`; `getRace`→`GetRace`; `getGender`→`GetGender`
   - `Unit::isFrozen/isInRoots/isFeared/isInFront/getThreatManager/getHostileRefManager/HasCharmer/IsUnderWater/getFactionTemplateEntry`
   - `Player::IsInGroup/CanInteract/clearUnitState/addUnitState/hasUnitState/HasMana/learnSpell/Whisper/isTaxiCheater/GetItemByEntry/IsFreeFlying/isAFK/IsStunnedByLogout/getQuestStatusMap/GetPlayerMenu/GetSpeedInMotion/GetCombinedCombatReach/CanReachWithMeleeAttack`
   - `Unit::GetTarget/getAttackers/GetAttackDistance/GetSpeedInMotion`
   - `GameObject::IsSpawned/IsInUse`

2. **Missing enums / constants.** Fix: `#define`/enum in `PlayerbotsCompat/VmangosBotCompat.h` (verify each value):
   `DIST_CALC_COMBAT_REACH`, `DIST_CALC_NONE`, `UNIT_FLAG_CLIENT_CONTROL_LOST`,
   `UNIT_STAT_FOLLOW`, `UNIT_STAT_CHASE`, `ITEM_SPELLTRIGGER_ON_NO_DELAY_USE`,
   `ITEM_SUBCLASS_FOOD/FLASK/POTION/SCROLL/CONSUMABLE_OTHER`, `ITEM_CLASS_MISC`,
   `ITEM_FLAG_HAS_LOOT`, `TEMPSPAWN_TIMED_DESPAWN`, `TRIGGERED_OLD_TRIGGERED`,
   `TRADE_SLOT_TRADED_COUNT`, `BG_AV_NODE_STATUS_HORDE_OCCUPIED/CONTESTED`.

3. **Missing SQLStorage/DBC store globals.** Fix: wrapper objects in
   VmangosBotCompat.h (like sSpellTemplate) or `#ifdef VMANGOS` at use sites:
   `sItemStorage`, `sCreatureStorage`, `sGOStorage`, `sFactionStore`,
   `sSkillLineAbilityStore`, `sAreaTriggerStore`, `sAreaStore`, `sSpellStore`.

4. **Struct field renames** (need `#ifdef VMANGOS` at the use site — can't alias
   a field): `CreatureInfo::NpcFlags`→`npc_flags`/`::Name`→`name`,
   `LootItem::itemId`, `AuctionEntry::itemRandomPropertyId`, etc.

5. **static vs instance calls**: `ObjectMgr::GetCreatureTemplate` /
   `GetGameObjectInfo` are non-static in vmangos → `sObjectMgr.` at use sites.

6. **Typed opcode handlers**: vmangos `WorldSession::HandleGameObjectUseOpcode`
   (and similar) take typed `WorldPackets::...` structs, not `WorldPacket`.
   These need real per-handler adaptation, not a simple alias.

7. **ServerFacade per-method VMANGOS arms** (matches neither MANGOS nor CMANGOS):
   `IsSpellReady`/`HasSpellCooldown`, `IsUnderWater`, `getFactionTemplateEntry`,
   `IsInFront`, `isMoving`/`movementFlagsMask`, `ChaseMovementGenerator` (template),
   `LookupSpellInfo`/`sSpellStore`→`sSpellMgr`, `BgTemplateId`.
