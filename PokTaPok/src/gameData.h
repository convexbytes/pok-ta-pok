
#ifndef GAMEDATA_H
#define GAMEDATA_H
#include "gameParameter.h"
#include "gameObject.h"
#include "sensorHandler.h"
#include "worldModel.h"

class GameData
{
public:
    GameParameter 	game_parameter;
    SensorHandler   sensor_handler;
};

#endif // GAMEDATA_H
