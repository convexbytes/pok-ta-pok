
#ifndef WORLD_MODEL_H
#define WORLD_MODEL_H
#include "sensorHandler.h"
#include "sensor.h"
#include "gameCommand.h"
#include "localizationEngine.h"

class AgentStateV1
{
public:
	AgentStateV1();

    Vector2D pos;
    double   angle;
    char 	 side;
    int 	 unum;
    ViewModeQuality view_mode_q;
    ViewModeWidth   view_mode_w;
    bool 	synch_see_on;

    double stamina;
    double effort;
    double head_angle;

};

class WorldModelV1
{
public:
    WorldModelV1( GameData * game_data );
    void update( GameData * game_data );


    // Para guardar las referencias de que se reciben en update
    GameData  	  * game_data;
    AgentCommand  * command_commited;


    AgentStateV1         me;
    LocalizationEngine * loc_engine;

    PlayModeHearable     play_mode;
    int                  time;

private:

    void updateOnBody   ();
    void updateOnInit   ();
    void updateOnSee    ();
    void updateOnHear   ();
    void updateOnOk		();

};



#endif // WORLD_MODEL_H
