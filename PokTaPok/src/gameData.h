
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
    GameParameter 	game_parameter;
    ObsHandler 		obs_handler;
};

#endif // GAMEDATA_H
