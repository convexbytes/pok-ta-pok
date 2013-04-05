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

#define POT_FIELD_EXPIRE_TIME 10

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


    // Para tomar decisiones
    PotentialField		pot_field;
    Vector2D  	 	 	goal_l;
    Vector2D  			goal_r;
    Vector2D			goal_pos;
    void init_pot_field();
    void update_pot_field();


    void update();

    void onInitial();
    void onPrep();
    void onPlay();

    void playDefault();
    void play2();
    void play3();

    void balonPropio();
    void balonEquipo();
    void balonRival();
    void balonDesconocido();
    void balonSuelto();
    void balonPerdido();

};

#endif // POKTAPOK_AGENT_H
