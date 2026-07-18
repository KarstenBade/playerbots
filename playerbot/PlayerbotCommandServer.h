#ifndef _PlayerbotCommandServer_H
#define _PlayerbotCommandServer_H

#include "Common.h"
#include "PlayerbotAIBase.h"
#include "PlayerbotMgr.h"

class PlayerbotCommandServer
{
public:
    PlayerbotCommandServer() {}
    virtual ~PlayerbotCommandServer() {}
    static PlayerbotCommandServer& instance()
    {
        static PlayerbotCommandServer instance;
        return instance;
    }

    void Start();
#ifdef VMANGOS
    // Drain remote-command requests on the world thread (thread safety;
    // called from RandomPlayerbotMgr::UpdateAIInternal).
    void ProcessQueuedCommands();
#endif
};

#define sPlayerbotCommandServer PlayerbotCommandServer::instance()

#endif
