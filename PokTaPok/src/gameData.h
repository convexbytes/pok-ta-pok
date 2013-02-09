
#ifndef GAMEDATA_H
#define GAMEDATA_H
#include "gameParameter.h"
#include "gameObject.h"
#include "obsHandler.h"
#include "worldState.h"
#include "observation.h"
#include "agentResponse.h"

class GameData
{
public:
    WorldState 		world_state;
    GameParameter 	game_parameter;
    ObsHandler 		obs_handler;
    PlayMode 		play_mode;
    //AgentCommand agent_response;
};

#endif // GAMEDATA_H
