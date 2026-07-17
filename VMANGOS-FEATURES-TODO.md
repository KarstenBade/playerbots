# VMANGOS port — remaining work & stubbed features

Status: full `.cpp` build is at **872 raw error lines / 685 distinct sites**
(build_current13.log), down from 1859 raw at the start of session 3 and 5876
once the PCH first compiled. Regenerate the census with
`python census13.py`-style parsing or:
`grep -oE 'error C[0-9]+: "[^"]+"' build_currentN.log | sort | uniq -c | sort -rn`.

**Reference "answer key"**: the user's fork of a working (but destructively
rewritten) vmangos+ike3 integration —
https://github.com/KarstenBade/core-with-playerbots branch
`vmangos-ike3-playerbots`, module in-tree at `src/game/PlayerBots/playerbot/`.
Same module lineage; ~197 unguarded "not in vmangos" stubs, so use it to find
*which vmangos API to call*, not for feature completeness. Keep as a shallow
scratch clone; deliberately NOT a submodule.

## Established gotchas (do not relearn)

- `GetAuraCount`-style core-header inlines touching incomplete types must be
  defined out-of-line.
- The module builds with `CMANGOS` **and** `VMANGOS` both defined: never add a
  bare `#ifdef VMANGOS` arm next to a pre-existing standalone
  `#ifdef CMANGOS`/`#ifdef MANGOS` block — re-guard the old arm with
  `#if defined(CMANGOS) && !defined(VMANGOS)` first. Plain unguarded lines are
  safe to wrap in `#ifdef VMANGOS ... #else ... #endif`.
- Forwarding shim headers must use angle-bracket includes (MSVC self-include
  trap).
- Module files are a MIX of LF and CRLF — normalize before scripted patching.
- Per-TU smoke compiles (cl + /FI botpch.h, see scratchpad compile_one.cmd)
  proved unreliable as a "file is clean" signal late in session 3 — trust only
  the full msbuild census.

## Cleared in session 3 (see git log for exact commits)

1. **Typed opcode handlers** (was the biggest chunk): call sites wrapped in
   `BOT_TYPED_PACKET(WorldPackets::X::Y, packet)` / `BOT_NULL_PACKET(packet)`
   → `MakeTypedPacket<T>()` (compat header) building the typed struct via
   `ReadFromWorldPacket`; passthrough under cmangos (botpch.h #else).
   `HandleBattlefieldPortOpcode` spelling alias; `HandleMoveWorldportAckOpcode`
   takes `NullClientPacket` under VMANGOS; `WorldSession::HandleBotPackets`
   dropped (VMANGOS-TODO: bot packets use the normal session-update path).
2. **Loot**: `typedef Loot LootAccess` under VMANGOS + cmangos-compat surface
   ON core `Loot` (`GetGoldAmount`, `CanLoot`, `GetLootItemsListFor`,
   `GetLootContentFor`, `IsPlayerLooting`, `IsLootedForAll`,
   `GetLootItemInSlot`, `GetRollForSlot`) — group-roll voting is REAL via the
   `GroupLootRoll` token → `Group::CountRollVote` (fork dropped rolls).
   `LootItemPtrList` typedef for the pointer-element list shape.
3. **Pathfinding/Map/movegens/MMAP**: real `PathInfo::setAreaCost`;
   `setArea/getArea/getFlags/ComputePathToRandomPoint` no-op core stubs
   (area-avoidance inert, VMANGOS-TODO); `MovePath` → `MovePoint(dest,
   MOVE_PATHFINDING)` funnel; `MoveFall/PauseWaypoints/DistanceYourself`
   stubs; `GetWalkRandomPosition` for reachable-random-point; graveyards via
   `sObjectMgr.GetClosestGraveYard`/`GetGraveYardMap`; real
   `MMapManager::IsMMapIsLoaded`; `Map::GetObjectsStore/GetTransports/
   ForceLoadGrid`; typed `begin<T>/end<T>` on TypeUnorderedMapContainer.
   Unit-less `PathFinder(mapId, instanceId)` returns empty path (VMANGOS-TODO:
   travel-node generation needs a real unit) — NOTE census still shows 4
   `PathInfo::PathInfo` sites, likely more unit-less ctors.
4. **Mail is REAL**: core Player mail methods forward to
   `GetSession()->GetMasterPlayer()` (reads, GetMItem item extraction,
   RemoveMail/RemoveMItem). Fork left item extraction stubbed.
5. **Channels/LFG/BG/Spell**: `Channel::Say(Player*)`,
   `ChannelMgr::GetChannel(name, Player*)`, `ChannelMgr::GetChannels()`;
   `WorldSession::QueuePacket(unique_ptr<WorldPacket>)` → `QueueBinaryPacket`
   (raw→typed parse; keeps BG-join queueing real); `Spell::GetCastItem`,
   `SpellStart` → `prepare(*targets)`; `m_clientCast` →
   `m_isClientStarted` define; BotUseItemSpell vmangos ctor arm +
   `ForceSpellStart` → `prepare()` (VMANGOS-TODO: no lock/reagent cheat
   bypass); `Item::SetUsedInSpell` no-op.
6. **Compat-header upgrades**: `sAreaStore`/`GetAreaEntryByAreaID` REAL
   (materialized from SQL-backed `sAreaStorage`; vmangos DOES load area data —
   old stub comment was wrong), `sFactionStore` REAL
   (`sObjectMgr.GetFactionEntry/GetFactionMap`), `using namespace Spells` +
   free-function wrappers for cmangos global spell helpers, `Opcodes` typedef,
   `ForcedMovement` enum, `NAV_AREA_WATER`, `LOOT_SLOT_*` maps,
   FactionTemplate field maps (`factionGroupMask`→`ourMask` etc.),
   `UNIT_STAT_CAN_NOT_REACT_OR_LOST_CONTROL`, EmotesTextSound stub,
   SPELL_RANGE_FLAG defines (+ per-site melee detection via
   `SPELL_RANGE_IDX_COMBAT`), `<random>`.
7. Core Chat.h declares module-defined handlers (`HandlePerfMonCommand`,
   `HandlePlayerbotCommand`, `HandleRandomPlayerbotCommand`,
   `HandleAhBotCommand`); `FindQuestSlot`/`GetQuestSlotQuestId` public under
   ENABLE_PLAYERBOTS; Player std::string chat overloads (Say/Yell/TextEmote/
   Whisper).

## Remaining (build_current13.log census, 685 distinct sites)

Top identifier clusters (many onesies not listed — regenerate for detail):

- **Syntax cascades** C2143 (31) / C2059 (23) / C2146 (11) / C2062 (9) —
  mostly follow-ons from the categories below; re-census after fixing them.
- **Private Player internals**: `CalculateTalentsPoints` (10), `m_taxi` (8),
  `SetQuestSlot` (5) — publicize under ENABLE_PLAYERBOTS (GetQuestSlotQuestId
  precedent) or add accessors.
- **`ObjectMgr::GetPlayer`** (11) — vmangos: `sObjectAccessor.FindPlayer` /
  `ObjectAccessor::FindPlayer`; route per-site or compat macro.
- **RNG**: `GetRandomGenerator` (8) + `std::shuffle` (8) — cmangos exposes a
  world RNG; use a compat `std::mt19937` instance.
- **`GetTeamIndexByTeamId`** (7) — vmangos equivalent/`TeamId` mapping.
- **`Config::GetStringDefault` 1-arg** (7) + `GetIntDefault` (5) — vmangos
  requires the default argument; per-site.
- **`GetBattleGroundBracketIdFromLevel`** (6) — vmangos BG bracket API differs.
- **`WorldSession::SendPacket`/`SendMessageToSet`/`MessageBroadcast`
  ref-vs-pointer** (6+6+4) — vmangos takes `WorldPacket*`.
- **`SetReactState`** (6) — vmangos Creature has ReactState? per-site/alias.
- **`GetLootState`** (6) — GameObject `GetGoState`/loot state rename.
- **`strcmpi(std::string, ...)`** (6) — more SpellName-style std::string sites.
- **`urand` overloads** (6), **`KilledMonster(CreatureInfo const*, guid)`**
  (5), **`GetSpellStore`** (5), **`MailItemInfo::item_guid`** (5),
  **`GetTrainerSpellState`** (5), **`Buff_Entries`** (4, BG buff GO ids —
  fork defines `{179871,179904,179905}`), **`exploreFlag`** (4),
  **`PathInfo::PathInfo`** (4 more unit-less ctor sites), plus long tail.
- **Biggest files**: RandomPlayerbotFactory.cpp (72), LfgActions.cpp (58),
  BattleGroundTactics.cpp (40), PlayerbotAI.cpp (39 — site list parsed in
  session 3: countof, SendPacket, SetCorpseAccelerationDelay/
  ReduceCorpseDecayTimer, GetDbGuid, MINIMUM_LOOTING_TIME, IsEncounter,
  CreateAura/CreateSpellAuraHolder signatures, TAXI/FALL_MOTION_TYPE,
  GetAngleAt, CanBeInterrupted, CanNoReagentCast, HasRealPlayers/HasActiveZone,
  HasCharm, GetMaster, Release, GetState, SetFallInformation, GetPowerPercent,
  RemoveGameObject, SetFacingTo, AnyUnfriendlyUnitInObjectRangeCheck ctor),
  DebugAction.cpp (37), TravelNode.cpp (36), ItemUsageValue.cpp (33),
  RandomPlayerbotMgr.cpp (31, incl. AuctionHouseMgr GetAuctionsMap shape),
  PlayerbotFactory.cpp (31).

## Stubbed features ledger (compile as safe no-ops; marked `VMANGOS-TODO`)

| Feature | Where | Notes |
|---|---|---|
| Instance templates | core `GetInstanceTemplate` stub | bot instance level gating inert |
| Area WMO override | `WorldPosition::getAreaOverride` | area names themselves now REAL via sAreaStorage |
| WorldSafeLocs orientation | `WorldPosition(WorldSafeLocsEntry*)` | vmangos entry has no `o` |
| Area-avoidance pathing | core PathInfo `setArea/getArea/getFlags` no-ops | vmangos mmaps don't tag poly areas |
| MoveFall / PauseWaypoints / DistanceYourself / MoveInFormation | core MotionMaster stubs + DebugAction guard | jump/fall + formation inert |
| Zone-granular bot-teleport activity | RandomPlayerbotMgr | map-level player presence instead |
| Travel-node generation pathfinding | WorldPosition::getPathFromPath | unit-less PathFinder returns empty path |
| Group-roll pre-open pass | LootValues ShouldLootObject | rolls start on first open in vmangos |
| BotUseItemSpell lock/reagent cheats | UseItemAction.cpp | normal prepare() path instead |
| Item::SetUsedInSpell | core Item.h no-op | no in-cast item lock in vmangos |
| EmotesTextSound | compat stub | bot emote sounds silent |
| WorldSession::HandleBotPackets | PlayerbotMgr.cpp guard | normal session-update path |
| Gossip text / locale-string fillers | compat GossipText + no-op fillers | unchanged from session 2 |
