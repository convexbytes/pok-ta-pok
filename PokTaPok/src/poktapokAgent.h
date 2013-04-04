#ifndef POKTAPOK_AGENT_H
#define POKTAPOK_AGENT_H
#include "agent.h"
#include "sensorHandler.h"
#include "worldModel.h"
#include "state.h"
#include "potentialField.h"
#ifndef TEAM_NAME
#define TEAM_NAME "PokTaPok"
#endif // TEAM_NAME

class PokTaPokAgentV1;
class BallInterception;
class FreezeBall;

class PokTaPokAgentV1 : public Agent
{
public:
    PokTaPokAgentV1( GameData * game_data );
    ~PokTaPokAgentV1();
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

    // Habilidades de las que depende
    BallInterception	* ball_interception;
    //PasiveInterception  * pasive_interception;
    FreezeBall			* freeze_ball;

    bool	on_incercept;

    void update();

    void onInitial();
    void onPrep();
    void onPlay();

    void playDefault();
    void play2();
    void play3();

};

#endif // POKTAPOK_AGENT_H
