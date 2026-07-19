#ifndef _ServerFacade_H
#define _ServerFacade_H

#include "Common.h"
#include "Entities/Unit.h"
#include "Entities/Player.h"
#ifdef CMANGOS
#include "Entities/GameObject.h"
#endif
#ifdef MANGOS
#include "Object/GameObject.h"
#endif
#include "BattleGround/BattleGroundMgr.h"
#include "PlayerbotAIBase.h"
#include "playerbot/PlayerbotAIConfig.h"
#include "playerbot/WorldPosition.h"

class ServerFacade
{
    public:
        ServerFacade();
        virtual ~ServerFacade();
        static ServerFacade& instance()
        {
            static ServerFacade instance;
            return instance;
        }

	public:
        bool UnitIsDead(Unit *unit)
        {
#ifdef MANGOS
            return unit->IsDead();
#endif
#ifdef CMANGOS
            return unit->IsDead();
#endif
        }
        float GetDistance(Unit *unit, WorldObject* wo);
        float GetDistance2d(Unit *unit, WorldObject* wo);
        float GetDistance(Unit *unit, float x, float y, float z);
        float GetDistance2d(Unit *unit, float x, float y);

        DeathState GetDeathState(Unit *unit)
        {
#ifdef MANGOS
#ifndef MANGOSBOT_TWO
            return unit->GetDeathState();
#else
            return unit->getDeathState();
#endif
#endif
#ifdef CMANGOS
            return unit->GetDeathState();
#endif
        }

        bool isSpawned(GameObject *go)
        {
#if defined(MANGOS) || defined(VMANGOS)
            return go->isSpawned();
#endif
#if defined(CMANGOS) && !defined(VMANGOS)
            return go->IsSpawned();
#endif
        }

        bool IsAlive(Unit *unit)
        {
#ifdef MANGOS
            return unit->IsAlive();
#endif
#ifdef CMANGOS
            return unit->IsAlive();
#endif
        }

        bool isMoving(Unit *unit);

        bool IsInCombat(Unit *unit)
        {
#ifdef MANGOS
            return unit->IsInCombat();
#endif
#ifdef CMANGOS
            return unit->IsInCombat();
#endif
        }

        bool IsFrozen(Unit *unit)
        {
#if defined(MANGOS) || defined(VMANGOS)
            return unit->IsFrozen();
#endif
#if defined(CMANGOS) && !defined(VMANGOS)
            return unit->isFrozen();
#endif
        }

        bool IsInRoots(Unit *unit)
        {
#if defined(MANGOS) || defined(VMANGOS)
            return unit->IsInRoots();
#endif
#if defined(CMANGOS) && !defined(VMANGOS)
            return unit->isInRoots();
#endif
        }

        bool IsCharmed(Unit *unit)
        {
#if defined(MANGOS) || defined(VMANGOS)
            return unit->GetCharmerGuid() && unit->GetCharmerGuid().IsPlayer();
#endif
#if defined(CMANGOS) && !defined(VMANGOS)
            return unit->HasCharmer();
#endif
        }

        bool IsFeared(Unit *unit)
        {
#if defined(MANGOS) || defined(VMANGOS)
#ifndef MANGOSBOT_TWO
            return unit->IsFeared();
#else
            return unit->isFeared();
#endif
#endif
#if defined(CMANGOS) && !defined(VMANGOS)
            return unit->isFeared();
#endif
        }

        bool IsInFront(Unit *unit, WorldObject const* target, float distance,  float arc /*= M_PI_F*/)
        {
#ifdef VMANGOS
            // vmangos HasInArc has no distance parameter; distance is ignored.
            return unit->HasInArc(target, arc);
#elif defined(MANGOS)
            return unit->IsInFront(target, distance, arc);
#elif defined(CMANGOS)
            return unit->isInFront(target, distance, arc);
#endif
        }

        HostileRefManager& GetHostileRefManager(Unit *unit)
        {
#if defined(MANGOS) || defined(VMANGOS)
            return unit->GetHostileRefManager();
#endif
#if defined(CMANGOS) && !defined(VMANGOS)
            return unit->getHostileRefManager();
#endif
        }

        ThreatManager& GetThreatManager(Unit *unit)
        {
#if defined(MANGOS) || defined(VMANGOS)
            return unit->GetThreatManager();
#endif
#if defined(CMANGOS) && !defined(VMANGOS)
            return unit->getThreatManager();
#endif
        }

        void SendPacket(Player *player, WorldPacket &packet)
        {
#if defined(MANGOS) || defined(VMANGOS)
            return player->GetSession()->SendPacket(&packet);
#endif
#if defined(CMANGOS) && !defined(VMANGOS)
            return player->GetSession()->SendPacket(packet);
#endif
        }

        void SendMessageToSet(Player *player, WorldPacket &packet, bool self)
        {
#if defined(MANGOS) || defined(VMANGOS)
            return player->SendMessageToSet(&packet, self);
#endif
#if defined(CMANGOS) && !defined(VMANGOS)
            return player->SendMessageToSet(packet, self);
#endif
        }

        SpellEntry const* LookupSpellInfo(uint32 spellId)
        {
#ifdef VMANGOS
            return sSpellMgr.GetSpellEntry(spellId);
#elif defined(MANGOS)
            return sSpellStore.LookupEntry(spellId);
#elif defined(CMANGOS)
            return sSpellTemplate.LookupEntry<SpellEntry>(spellId);
#endif
        }

        SpellRangeEntry const* LookupSpellRangeEntry(uint32 rangeIndex)
        {
            return sSpellRangeStore.LookupEntry(rangeIndex);
        }

        uint32 GetSpellInfoRows()
        {
#ifdef VMANGOS
            return sSpellMgr.GetMaxSpellId();
#elif defined(MANGOS)
            return sSpellStore.GetNumRows();
#elif defined(CMANGOS)
            return sSpellTemplate.GetMaxEntry();
#endif
        }

        bool IsWithinLOSInMap(Player* bot, WorldObject *wo)
        {
#if defined(MANGOS) || defined(VMANGOS)
            return bot->IsWithinLOSInMap(wo);
#endif
#if defined(CMANGOS) && !defined(VMANGOS)
            return bot->IsWithinLOSInMap(wo, true);
#endif
        }

        bool IsWithinStaticLOSInMap(Player* bot, WorldObject* wo) const
        {
            return wo->IsInMap(bot) ? ai::WorldPosition(wo).IsInStaticLineOfSight(ai::WorldPosition(bot)) : false;
        }

        bool IsDistanceLessThan(float dist1, float dist2);
        bool IsDistanceGreaterThan(float dist1, float dist2);
        bool IsDistanceGreaterOrEqualThan(float dist1, float dist2);
        bool IsDistanceLessOrEqualThan(float dist1, float dist2);

        void SetFacingTo(Unit* unit, float angle, bool force = false);
        void SetFacingTo(Unit* unit, WorldObject* wo, bool force = false) {SetFacingTo(unit, unit->GetAngle(wo), force);}

        bool IsFriendlyTo(Unit* bot, Unit* to);
        bool IsFriendlyTo(WorldObject* bot, Unit* to);
        bool IsHostileTo(Unit* bot, Unit* to);
        bool IsHostileTo(WorldObject* bot, Unit* to);

        bool IsSpellReady(Player* bot, uint32 spell, uint32 itemId = 0);

        bool IsUnderwater(Unit *unit);
        FactionTemplateEntry const* GetFactionTemplateEntry(Unit *unit);
        Unit* GetChaseTarget(Unit* target);
        float GetChaseAngle(Unit* target);
        float GetChaseOffset(Unit* target);
        // True unless the chase/follow generator's last pathfind failed to
        // reach the target. True when no chase/follow generator is current.
        bool IsChaseTargetReachable(Unit* target);

        BattleGroundTypeId BgTemplateId(BattleGroundQueueTypeId queueTypeId)
        {
#ifdef MANGOS
            return sBattleGroundMgr.BGTemplateId(queueTypeId);
#endif
#ifdef CMANGOS
            return sBattleGroundMgr.BgTemplateId(queueTypeId);
#endif
        }
#ifndef MANGOSBOT_ZERO
        ArenaType BgArenaType(BattleGroundQueueTypeId queueTypeId)
        {
#ifdef MANGOS
            return sBattleGroundMgr.BGArenaType(queueTypeId);
#endif
#ifdef CMANGOS
            return sBattleGroundMgr.BgArenaType(queueTypeId);
#endif
        }
#endif

        uint32 GetAreaId(WorldObject* wo)
        {
            return sTerrainMgr.GetAreaId(wo->GetMapId(), wo->GetPositionX(), wo->GetPositionY(), wo->GetPositionZ());
        }
};

#define sServerFacade ServerFacade::instance()

#endif
