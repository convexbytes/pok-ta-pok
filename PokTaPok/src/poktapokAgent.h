#ifndef POKTAPOK_AGENT_H
#define POKTAPOK_AGENT_H
#include "agent.h"
#include "attractorField.h"
#include "sensorHandler.h"
#include "worldModel.h"
#include "state.h"
#define TEAM_NAME "PokTaPok"



class PokTaPokAgentV1 : public Agent
{
public:
    PokTaPokAgentV1( GameData * game_data );
    virtual void do_process( GameData * game_data,
                             AgentCommand *agent_command );




private:
    WorldModelV1        * world;
    GameData			* game_data;
    AgentCommand        * command;
    AgentCommand        * command_c;
    GameParameter       * param;
    SensorHandler       * sensor_h;
    StateMachine		  state_m;

    void update();

    void onInitial();
    void onPrep();
    void onPlay();
};

#endif // POKTAPOK_AGENT_H
