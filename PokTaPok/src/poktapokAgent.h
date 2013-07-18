#ifndef POKTAPOK_AGENT_H
#define POKTAPOK_AGENT_H
#include "agent.h"
#include "sensorHandler.h"
#include "worldModel.h"
#include "state.h"
#ifndef TEAM_NAME
#define TEAM_NAME "PokTaPok"
#endif // TEAM_NAME

#define POT_FIELD_EXPIRE_TIME 5
#define PASS_OPTION_EXPIRE_TIME 3

class PokTaPokAgent;
class BallInterception;
class FreezeBall;

class ActionOption;

enum ActionType
{
	RUN,
	PASS,
	SHOOT
};

enum MasterStatus
{
  DEFENSA = 0,
  ATAQUE
};

enum PossessionBall
{

    PROPIA = 0,     // balón en poder del agente
    EQUIPO,         // balón en poder del equipo propio
    RIVAL,          // balón en poder del rival
    SUELTA,         // balón suelto
    PERDIDA         // balón sin ver
};


class PokTaPokAgent	 : public Agent
{
public:
    PokTaPokAgent( GameData * game_data );
    ~PokTaPokAgent();
    virtual void do_process( GameData * game_data,
                             AgentCommand *agent_command );


private:
    WorldModel	        * world;
    GameData			* game_data;
    AgentCommand        * command;
    AgentCommand        * command_c;
    GameParameter       * param;
    SensorHandler       * sensor_h;
    StateMachine		  state_m;

    PossessionBall		m_ball_possession;
    MasterStatus    	m_master_status;

    // Habilidades
    BallInterception	* ball_interception;
    FreezeBall			* freeze_ball;


    // Para tomar decisiones
    Vector2D  	 	 	goal_l;
    Vector2D  			goal_r;
    Vector2D			goal_pos;


    // Para posicionar
    Vector2D			 last_voronoi_centroid;
    std::vector<Point2D> voronoi_cell;

    Vector2D pass_point;

    void update();

    void onInitial();
    void onPrep();
    void onPlay();

    void irAlBalon();

    void balonPropio();
    void balonEquipo();
    void balonRival();
    void balonSuelto();
    void balonPerdido();


    void ballStaticBehaviorSideL();
    void ballStaticBehaviorSideR();



    void constructVoronoi();
    void voronoiPositioning();
    void attackPositioning(); // Voronoi con campos potenciales
    void defencePositioning();

    void goalieBehavior();

    void defensa();

    void ataque();

    void passAction( int unum, Vector2D const & pos );

    void shootAction();

    int   aQuienPasar();
    bool  isZoneShoot( double x, double y , double radio);
    bool  IamTheClosest();
    bool  balonEnAreaGrande();

    PossessionBall whoHasTheBall();

    bool inGoalieLine();

    double 		 passProb( Vector2D const & target_pos );

    ActionOption chooseAction( std::vector<ActionOption> const & actions );
};




class ActionOption
{
public:
	Vector2D 	target;
	double 	 	dist_to_goal;
	double 	 	prob;
	ActionType 	action_type;
	int			unum; // ball := 0, playres := 1,...,11

	static bool distanceLowerThan( ActionOption i, ActionOption j )
	{
		return i.dist_to_goal < j.dist_to_goal;
	}

};

#endif // POKTAPOK_AGENT_H
