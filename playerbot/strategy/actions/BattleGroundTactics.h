#pragma once

#include "MovementActions.h"
#include "BattleGround/BattleGround.h"
#include "BattleGround/BattleGroundMgr.h"
#include "BattleGround/BattleGroundWS.h"
#include "BattleGround/BattleGroundAB.h"
#ifndef MANGOSBOT_ZERO
#include "BattleGround/BattleGroundEY.h"
#endif
#ifdef MANGOSBOT_TWO
#include "BattleGround/BattleGroundIC.h"
#include "BattleGround/BattleGroundSA.h"
#endif
#include "CheckMountStateAction.h"

using namespace ai;

#define SPELL_CAPTURE_BANNER 21651

#ifdef VMANGOS
// vmangos's native BattleBot (src/game/PlayerBots/BattleBotWaypoints.cpp)
// defines waypoint tables with these exact names; prefix the module's own
// copies to avoid duplicate symbols at link time.
#define vPath_AV_Alliance_Base_Bunker_Second_Crossroad_to_Alliance_Base_Bunker_Third_Crossroad bot_vPath_AV_Alliance_Base_Bunker_Second_Crossroad_to_Alliance_Base_Bunker_Third_Crossroad
#define vPath_AV_Alliance_Base_Bunker_First_Crossroad_to_Alliance_Base_Bunker_Second_Crossroad bot_vPath_AV_Alliance_Base_Bunker_First_Crossroad_to_Alliance_Base_Bunker_Second_Crossroad
#define vPath_AV_Stonehearth_Graveyard_Second_Crossroad_to_Stonehearth_Bunker_First_Crossroad bot_vPath_AV_Stonehearth_Graveyard_Second_Crossroad_to_Stonehearth_Bunker_First_Crossroad
#define vPath_AV_Iceblood_Tower_Crossroad_to_Field_of_Strife_Stoneheart_Snowfall_Crossroad bot_vPath_AV_Iceblood_Tower_Crossroad_to_Field_of_Strife_Stoneheart_Snowfall_Crossroad
#define vPath_AV_Alliance_Base_Bunker_Third_Crossroad_to_Alliance_Base_Vanndar_Stormpike bot_vPath_AV_Alliance_Base_Bunker_Third_Crossroad_to_Alliance_Base_Vanndar_Stormpike
#define vPath_AV_Field_of_Strife_Stoneheart_Snowfall_Crossroad_to_Stonehearth_Outpost bot_vPath_AV_Field_of_Strife_Stoneheart_Snowfall_Crossroad_to_Stonehearth_Outpost
#define vPath_AV_Stonehearth_Graveyard_Flag_to_Stonehearth_Graveyard_Second_Crossroad bot_vPath_AV_Stonehearth_Graveyard_Flag_to_Stonehearth_Graveyard_Second_Crossroad
#define vPath_AV_Alliance_Base_Bunker_Second_Crossroad_to_Alliance_Base_South_Bunker bot_vPath_AV_Alliance_Base_Bunker_Second_Crossroad_to_Alliance_Base_South_Bunker
#define vPath_AV_Stonehearth_Graveyard_Second_Crossroad_to_Iceblood_Tower_Crossroad bot_vPath_AV_Stonehearth_Graveyard_Second_Crossroad_to_Iceblood_Tower_Crossroad
#define vPath_AV_Alliance_Base_Bunker_First_Crossroad_to_Alliance_Base_North_Bunker bot_vPath_AV_Alliance_Base_Bunker_First_Crossroad_to_Alliance_Base_North_Bunker
#define vPath_AV_Snowfall_Flag_to_Field_of_Strife_Stoneheart_Snowfall_Crossroad bot_vPath_AV_Snowfall_Flag_to_Field_of_Strife_Stoneheart_Snowfall_Crossroad
#define vPath_AV_Stonehearth_Bunker_First_Crossroad_to_Stonehearth_Bunker_Flag bot_vPath_AV_Stonehearth_Bunker_First_Crossroad_to_Stonehearth_Bunker_Flag
#define vPath_AV_Stonehearth_Graveyard_Crossroad_to_Stonehearth_Graveyard_Flag bot_vPath_AV_Stonehearth_Graveyard_Crossroad_to_Stonehearth_Graveyard_Flag
#define vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_Entrance_DrekThar bot_vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_Entrance_DrekThar
#define vPath_AV_Stormpike_Crossroad_to_Alliance_Base_Bunker_First_Crossroad bot_vPath_AV_Stormpike_Crossroad_to_Alliance_Base_Bunker_First_Crossroad
#define vPath_AV_Horde_Base_First_Crossroads_to_Horde_Base_Second_Crossroads bot_vPath_AV_Horde_Base_First_Crossroads_to_Horde_Base_Second_Crossroads
#define vPath_AV_Stonehearth_Graveyard_Second_Crossroad_to_Iceblood_Garrison bot_vPath_AV_Stonehearth_Graveyard_Second_Crossroad_to_Iceblood_Garrison
#define vPath_AV_Stonehearth_Graveyard_Crossroad_to_Icewing_Bunker_Crossroad bot_vPath_AV_Stonehearth_Graveyard_Crossroad_to_Icewing_Bunker_Crossroad
#define vPath_AV_Alliance_Base_Bunker_Third_Crossroad_to_Alliance_Base_Flag bot_vPath_AV_Alliance_Base_Bunker_Third_Crossroad_to_Alliance_Base_Flag
#define vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_Graveyard_Flag bot_vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_Graveyard_Flag
#define vPath_AV_Horde_Base_First_Crossroads_to_East_Frostwolf_Tower_Flag bot_vPath_AV_Horde_Base_First_Crossroads_to_East_Frostwolf_Tower_Flag
#define vPath_AV_Alliance_Cave_Slop_Crossroad_to_Alliance_Slope_Crossroad bot_vPath_AV_Alliance_Cave_Slop_Crossroad_to_Alliance_Slope_Crossroad
#define vPath_AV_Horde_Base_First_Crossroads_to_West_Frostwolf_Tower_Flag bot_vPath_AV_Horde_Base_First_Crossroads_to_West_Frostwolf_Tower_Flag
#define vPath_AV_Frostwolf_Graveyard_Flag_to_Horde_Base_First_Crossroads bot_vPath_AV_Frostwolf_Graveyard_Flag_to_Horde_Base_First_Crossroads
#define vPath_AV_Stonehearth_Outpost_to_Stonehearth_Graveyard_Crossroad bot_vPath_AV_Stonehearth_Outpost_to_Stonehearth_Graveyard_Crossroad
#define vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_DrekThar2 bot_vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_DrekThar2
#define vPath_AV_Icewing_Bunker_Crossroad_to_Alliance_Slope_Crossroad bot_vPath_AV_Icewing_Bunker_Crossroad_to_Alliance_Slope_Crossroad
#define vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_DrekThar1 bot_vPath_AV_Horde_Base_Second_Crossroads_to_Horde_Base_DrekThar1
#define vPath_AV_Iceblood_Graveyard_Flag_to_Iceblood_Tower_Crossroad bot_vPath_AV_Iceblood_Graveyard_Flag_to_Iceblood_Tower_Crossroad
#define vPath_AV_Stonehearth_Graveyard_to_Stonehearth_Graveyard_Flag bot_vPath_AV_Stonehearth_Graveyard_to_Stonehearth_Graveyard_Flag
#define vPath_AV_Stonehearth_Outpost_to_Captain_Balinda_Stonehearth bot_vPath_AV_Stonehearth_Outpost_to_Captain_Balinda_Stonehearth
#define vPath_AV_Tower_Point_Crossroads_to_Iceblood_Graveyard_Flag bot_vPath_AV_Tower_Point_Crossroads_to_Iceblood_Graveyard_Flag
#define vPath_AV_Frostwolf_Graveyard_to_Frostwolf_Graveyard_Flag bot_vPath_AV_Frostwolf_Graveyard_to_Frostwolf_Graveyard_Flag
#define vPath_AV_Alliance_Slope_Crossroad_to_Stormpike_Crossroad bot_vPath_AV_Alliance_Slope_Crossroad_to_Stormpike_Crossroad
#define vPath_AV_Icewing_Bunker_Crossroad_to_Icewing_Bunker_Flag bot_vPath_AV_Icewing_Bunker_Crossroad_to_Icewing_Bunker_Flag
#define vPath_AV_Tower_Point_Bottom_to_Frostwolf_Graveyard_Flag bot_vPath_AV_Tower_Point_Bottom_to_Frostwolf_Graveyard_Flag
#define vPath_AV_Iceblood_Graveyard_to_Iceblood_Graveyard_Flag bot_vPath_AV_Iceblood_Graveyard_to_Iceblood_Graveyard_Flag
#define vPath_AV_Alliance_Cave_to_Alliance_Cave_Slop_Crossroad bot_vPath_AV_Alliance_Cave_to_Alliance_Cave_Slop_Crossroad
#define vPath_AV_Tower_Point_Crossroads_to_Tower_Point_Bottom bot_vPath_AV_Tower_Point_Crossroads_to_Tower_Point_Bottom
#define vPath_AV_Iceblood_Tower_Crossroad_to_Iceblood_Tower bot_vPath_AV_Iceblood_Tower_Crossroad_to_Iceblood_Tower
#define vPath_AV_Horde_Cave_to_Frostwolf_Graveyard_Flag bot_vPath_AV_Horde_Cave_to_Frostwolf_Graveyard_Flag
#define vPath_AV_Iceblood_Garrison_to_Captain_Galvangar bot_vPath_AV_Iceblood_Garrison_to_Captain_Galvangar
#define vPath_WSG_AllianceFlagRoom_to_AllianceGraveyard bot_vPath_WSG_AllianceFlagRoom_to_AllianceGraveyard
#define vPath_AV_TowerPoint_Bottom_to_Tower_Point_Flag bot_vPath_AV_TowerPoint_Bottom_to_Tower_Point_Flag
#define vPath_AV_Stormpike_Crossroad_to_Stormpike_Flag bot_vPath_AV_Stormpike_Crossroad_to_Stormpike_Flag
#define vPath_AV_Iceblood_Tower_to_Iceblood_Tower_Flag bot_vPath_AV_Iceblood_Tower_to_Iceblood_Tower_Flag
#define vPath_WSG_AllianceGraveyard_to_AllianceTunnel bot_vPath_WSG_AllianceGraveyard_to_AllianceTunnel
#define vPath_AV_Iceblood_Tower_to_Iceblood_Garrison bot_vPath_AV_Iceblood_Tower_to_Iceblood_Garrison
#define vPath_WSG_AllianceTunnel_to_AllianceBaseRoof bot_vPath_WSG_AllianceTunnel_to_AllianceBaseRoof
#define vPath_AV_Horde_Cave_to_Tower_Point_Crossroad bot_vPath_AV_Horde_Cave_to_Tower_Point_Crossroad
#define vPath_WSG_AllianceTunnel_to_AllianceFlagRoom bot_vPath_WSG_AllianceTunnel_to_AllianceFlagRoom
#define vPath_AV_Iceblood_Garrison_to_Snowfall_Flag bot_vPath_AV_Iceblood_Garrison_to_Snowfall_Flag
#define vPath_WSG_AllianceGYJump_to_AllianceTunnel bot_vPath_WSG_AllianceGYJump_to_AllianceTunnel
#define vPath_WSG_HordeTunnel_to_AllianceTunnel_1 bot_vPath_WSG_HordeTunnel_to_AllianceTunnel_1
#define vPath_WSG_HordeGYJump_to_AllianceFlagRoom bot_vPath_WSG_HordeGYJump_to_AllianceFlagRoom
#define vPath_WSG_HordeTunnel_to_AllianceTunnel_2 bot_vPath_WSG_HordeTunnel_to_AllianceTunnel_2
#define vPath_WSG_HordeFlagRoom_to_HordeGraveyard bot_vPath_WSG_HordeFlagRoom_to_HordeGraveyard
#define vPath_WSG_AllianceGYJump_to_HordeFlagRoom bot_vPath_WSG_AllianceGYJump_to_HordeFlagRoom
#define vPath_WSG_HordeGraveyard_to_HordeTunnel bot_vPath_WSG_HordeGraveyard_to_HordeTunnel
#define vPath_WSG_HordeGYJump_to_AllianceTunnel bot_vPath_WSG_HordeGYJump_to_AllianceTunnel
#define vPath_WSG_AllianceGYJump_to_HordeTunnel bot_vPath_WSG_AllianceGYJump_to_HordeTunnel
#define vPath_WSG_HordeTunnel_to_HordeFlagRoom bot_vPath_WSG_HordeTunnel_to_HordeFlagRoom
#define vPath_WSG_HordeTunnel_to_HordeBaseRoof bot_vPath_WSG_HordeTunnel_to_HordeBaseRoof
#define vPath_WSG_HordeGYJump_to_HordeTunnel bot_vPath_WSG_HordeGYJump_to_HordeTunnel
#define vPath_AB_AllianceBase_to_LumberMill bot_vPath_AB_AllianceBase_to_LumberMill
#define vPath_AB_AllianceBase_to_GoldMine bot_vPath_AB_AllianceBase_to_GoldMine
#define vPath_AB_AllianceBase_to_Stables bot_vPath_AB_AllianceBase_to_Stables
#define vPath_AB_HordeBase_to_LumberMill bot_vPath_AB_HordeBase_to_LumberMill
#define vPath_AB_Stables_to_LumberMill bot_vPath_AB_Stables_to_LumberMill
#define vPath_AB_HordeBase_to_GoldMine bot_vPath_AB_HordeBase_to_GoldMine
#define vPath_AB_Stables_to_Blacksmith bot_vPath_AB_Stables_to_Blacksmith
#define vPath_AB_Stables_to_GoldMine bot_vPath_AB_Stables_to_GoldMine
#define vPath_AB_Farm_to_Blacksmith bot_vPath_AB_Farm_to_Blacksmith
#define vPath_AB_Farm_to_LumberMill bot_vPath_AB_Farm_to_LumberMill
#define vPath_AB_HordeBase_to_Farm bot_vPath_AB_HordeBase_to_Farm
#define vPath_AB_Farm_to_GoldMine bot_vPath_AB_Farm_to_GoldMine
#define vPaths_WS bot_vPaths_WS
#define vPaths_AV bot_vPaths_AV
#define vPaths_AB bot_vPaths_AB
#endif // VMANGOS

typedef void(*BattleBotWaypointFunc) ();

// from vmangos
struct BattleBotWaypoint
{
    BattleBotWaypoint(float x_, float y_, float z_, BattleBotWaypointFunc func) :
        x(x_), y(y_), z(z_), pFunc(func) {};
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    BattleBotWaypointFunc pFunc = nullptr;
};

typedef std::vector<BattleBotWaypoint> BattleBotPath;

extern std::vector<BattleBotPath*> const vPaths_WS;
extern std::vector<BattleBotPath*> const vPaths_AB;
extern std::vector<BattleBotPath*> const vPaths_AV;
extern std::vector<BattleBotPath*> const vPaths_EY;
extern std::vector<BattleBotPath*> const vPaths_IC;

class BGTactics : public MovementAction
{
public:
    BGTactics(PlayerbotAI* ai, std::string name = "bg tactics") : MovementAction(ai, name) {}

#ifdef GenerateBotHelp
        virtual std::string GetHelpName() { return "bg tactics"; }
        virtual std::string GetHelpDescription()
        {
            return "This action handles the bot's tactical movement in battlegrounds.\n"
                   "It includes pathfinding for objectives, flag handling, and strategic positioning.\n"
                   "Supports WSG, AB, AV, EY, and IC battlegrounds with specialized waypoints.";
        }
        virtual std::vector<std::string> GetUsedActions() { return {}; }
        virtual std::vector<std::string> GetUsedValues() { return {}; }
#endif 
    virtual bool Execute(Event& event) override;
private:
    bool SelectAvObjectiveAlliance(WorldLocation& objectiveLocation);
    bool SelectAvObjectiveHorde(WorldLocation& objectiveLocation);
    bool moveToStart(bool force = false);
    bool selectObjective(bool reset = false);
    bool moveToObjective();
    bool selectObjectiveWp(std::vector<BattleBotPath*> const& vPaths);
    bool moveToObjectiveWp(BattleBotPath* const& currentPath, uint32 currentPoint, bool reverse = false);
    bool startNewPathBegin(std::vector<BattleBotPath*> const& vPaths);
    bool startNewPathFree(std::vector<BattleBotPath*> const& vPaths);
    bool resetObjective();
    bool wsgPaths();
    bool wsgRoofJump();
    bool eotsJump();
    bool atFlag(std::vector<BattleBotPath*> const& vPaths, std::vector<uint32> const& vFlagIds);
    bool CheckFlagAv();
    bool flagTaken();
    bool teamFlagTaken();
    bool protectFC();
    bool useBuff();
    uint32 getDefendersCount(Position point, float range, bool combat = true);
    bool IsLockedInsideKeep();
};

class ArenaTactics : public MovementAction
{
public:
    ArenaTactics(PlayerbotAI* ai, std::string name = "arena tactics") : MovementAction(ai, name) {}

#ifdef GenerateBotHelp
        virtual std::string GetHelpName() { return "arena tactics"; }
        virtual std::string GetHelpDescription()
        {
            return "This action handles the bot's movement strategy in arenas.\n"
                   "Focuses on controlling the center of the arena and maintaining optimal positioning.";
        }
        virtual std::vector<std::string> GetUsedActions() { return {}; }
        virtual std::vector<std::string> GetUsedValues() { return {}; }
#endif 
    virtual bool Execute(Event& event) override;
private:
    bool moveToCenter(BattleGround *bg);
};
