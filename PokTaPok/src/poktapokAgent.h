#ifndef POKTAPOK_AGENT_H
#define POKTAPOK_AGENT_H
#include "agent.h"
#include "ability.h"
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
    PokTaPokAgent( GameData * game_data, AgentCommand * command );
    ~PokTaPokAgent();
    virtual void do_process();


private:
    WorldModel	        * m_world;
    AgentCommand        * m_command_commited;
    GameParameter       * m_param;
    SensorHandler       * m_sensor_h;
    StateMachine		  m_state_machine;

    PossessionBall		m_ball_possession;
    MasterStatus    	m_master_status;

    // Habilidades
    PassToPoint			* m_pass_to_point;
    GoToPoint			* m_go_to_point;
    FreezeBall			* m_freeze_ball;
    SearchBall			* m_search_ball;
    BallInterception	* m_ball_interception;
    CenterBall			* m_center_ball;
    RunWithBall			* m_run_with_ball;
    Shoot				* m_shoot;
    // Goalie...
    GoToPoint2			* m_go_to_point2;
    GoalieLine			* m_goalie_line;

    Vector2D  	 	 	m_goal_l;
    Vector2D  			m_goal_r;
    Vector2D			m_goal_pos;

    // Para posicionar
    Vector2D			 m_last_voronoi_centroid;
    std::vector<Point2D> m_voronoi_cell;

    Vector2D m_pass_point;

    void update();

    void onInitial();
    void onPrep();
    void onPlay();

    //void irAlBalon();

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


    int   aQuienPasar();
    bool  isShootZone( double x, double y , double radio);
    bool  IamTheClosest();
    bool  balonEnAreaGrande();

    PossessionBall whoHasTheBall();

    bool inGoalieLine();

    ActionOption chooseAction( std::vector<ActionOption> const & actions );
};




class ActionOption
{
public:
	Vector2D 	target;
	double 	 	dist_to_goal;
	double 	 	prob;
	ActionType 	action_type;
	int			unum; // ball = 0, playres = 1,...,11

	static bool distanceLowerThan( ActionOption i, ActionOption j )
	{
		return i.dist_to_goal < j.dist_to_goal;
	}

};

#endif // POKTAPOK_AGENT_H
