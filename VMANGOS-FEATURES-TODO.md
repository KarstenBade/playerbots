# VMANGOS port — remaining work & stubbed features

Status: **RUNTIME SMOKE PASSED** (2026-07-17) — builds clean AND random
bots create accounts/characters, persist them, log in, and enter the
world (6 bots online in an isolated test env; see "Session 5 runtime
fixes" below). Next: `.bot add` with a real client/master, bot behavior
observation, then feature-by-feature triage of the stubbed ledger.

Test env: `C:mangos-server-playerbot-integration` (world port 8086),
private MySQL 8.4 on port 3307 (datadir `mysql-data/` in that folder,
root, empty password; `--mysql-native-password=ON` required), Data
junctioned from `C:mangos-server-dev\Data`. The dev server and its
databases on port 3306 were never touched.

## Session 5 runtime fixes (crash-loop phase, all committed)

Debugging used `minidebug.exe` (scratchpad; DbgHelp debug-loop harness
that symbolizes fatal stacks — vmangos's own handler misses fastfails).
In order of discovery: cmangos SQL column names (creature_template,
item_template, characters, guild — see commit 62e5cc95); GetAreaLevel
stack overflow on self-referencing area 5091 (cycle guards + pre-filled
ai_playerbot_zone_level); playerbots init moved after MovementBroadcaster
creation (Player::Create → speed aura → broadcaster null deref);
LoadTrainers now populates learnedSpell (TrainerValues dereferenced it);
LootLootGroupAccess stride (vmangos LootGroup has a trailing bool);
GetEventValue null-safe reads; bot chars: SetSaveDisabled(false) +
InsertPlayerInCache (vmangos Create path is temp-bot flagged and
guid->account resolution is cache-based).

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

## Cleared in session 4 (link phase)

All census categories from session 3 were driven to zero (private Player
internals via public access windows, ObjectMgr::GetPlayer, RNG, TeamIndex,
Config default-args, BG brackets, packet ref-vs-ptr, renames, urand, etc. —
see git log). Then the mangosd link:

1. **vPath_* duplicate symbols** vs vmangos's native BattleBotWaypoints.cpp →
   79 `#define vPath_X bot_vPath_X` renames in BattleGroundTactics.h.
2. **boost::thread autolink** (no libboost_thread in vmangos deps) →
   std::thread arms in RandomPlayerbotMgr.cpp / PlayerbotCommandServer.cpp.
3. **Bot login machinery** (`PlayerbotHolder::AddPlayerBot`,
   `HandlePlayerBotLoginCallback`) — cmangos patches these into core;
   here they live module-side in PlayerbotMgr.cpp under VMANGOS:
   `PlayerbotLoginQueryHolder` mirrors `LoginQueryHolder::Initialize`
   (CharacterHandler.cpp) and **must be kept in sync with the vmangos
   characters schema**; callback builds a socketless WorldSession, sets
   `SetPlayerLoading(true)` (guarded core setter), calls `HandlePlayerLogin`,
   then `OnBotLogin` + `RandomPlayerbotMgr::OnBotLoginRegistration` (only the
   bookkeeping half of OnPlayerLogin — the full one resets strategies on
   other bots, unsafe from the DB worker thread).
4. **Singleton statics**: vmangos's `MaNGOS::Singleton` needs
   `INSTANTIATE_SINGLETON_1` per type (cmangos's doesn't) — added under
   VMANGOS for TravelMgr, TravelNodeMap, SharedObjectContext,
   PlayerbotLLMInterface, PlayerBotLoginMgr.
5. **libssl**: vmangos's bundled Windows OpenSSL is libcrypto-only
   (libeay32.lib). PlayerbotLLMInterface.cpp stubs the 15 libssl entry
   points under `VMANGOS_NO_LIBSSL` (defined by the module CMake on WIN32):
   HTTPS LLM endpoints fail gracefully at SSL_CTX_new, plain HTTP works.
   On platforms with OPENSSL_FOUND, real OpenSSL is linked instead.

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
| HTTPS LLM endpoints (Windows) | PlayerbotLLMInterface.cpp `VMANGOS_NO_LIBSSL` | no libssl in vmangos Windows deps; HTTP works |
| Full OnPlayerLogin on bot login | RandomPlayerbotMgr::OnBotLoginRegistration | strategy reset of other bots skipped (thread safety) |
