
#ifndef WORLD_MODEL_H
#define WORLD_MODEL_H
#include "sensorHandler.h"
#include "gameCommand.h"
#include "localizationEngine.h"


class AgentStateV1
{
public:


    Vector2D M_pos;
    double   M_angle;
};

class WorldModelV1
{
public:
    WorldModelV1( );
    void update( );


    int              time()     const { return M_time; }
    PlayModeHearable playMode() const { return M_play_mode; }

private:
    AgentStateV1         M_me;
    LocalizationEngine * M_loc_engine;

    PlayModeHearable     M_play_mode;
    int                  M_time;


    void updateOnSense  ( BodySensor const &  body_sense );
    void updateOnInit   ( InitSensor  const & init );
    void updateOnSee    ( SeeSensor   const & see );
    void updateOnHear   ( SensorHandler const & sensor_h);
};



#endif // WORLD_MODEL_H
