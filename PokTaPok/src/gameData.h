
#ifndef GAMEDATA_H
#define GAMEDATA_H
#include "gameParameter.h"
#include "gameCommand.h"
#include "gameObject.h"
#include "sensorHandler.h"


class GameData
{
public:
    GameParameter   game_parameter;
    SensorHandler   sensor_handler;
    AgentCommand    command_commited;
};

#endif // GAMEDATA_H
