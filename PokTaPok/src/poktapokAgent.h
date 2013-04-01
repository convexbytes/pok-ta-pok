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
class BallIntercept;
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

    // Habilidades
    BallIntercept		* ball_intercept;
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

// Temporal
class BallIntercept
{
public:
	BallIntercept( GameData 		* game_data,
				   WorldModelV1 	* world,
				   FreezeBall		* freeze_ball );
	void reset(); //
	void call( AgentCommand * command);
private:
	FreezeBall	* freeze_ball; // Habilidad de congelar el balón

	GameData 	 * game_data;
	AgentCommand * command;
	WorldModelV1 * world;

	static double const mean_vel = 0.92;

	ServerParam * param ; //= game_data->game_parameter.server_param;
	SeeSensor 	* visual; //= game_data->sensor_handler.last_see;
	BodySensor 	* body  ; //= game_data->sensor_handler.last_sense;

	// Datos calculados cuando la habilidad es llamada por "primera" vez
	Vector2D player_pos_s;
	Vector2D player_vel_s; // Velocidad absoluta del agente, cartesianas. Calculada al "inicio" de la habilidad
	Vector2D ball_pos_s; // Posición absoluta del balón, cartesianas
	Vector2D intercept_point_s; // Punto de intercepción teórico (velocidad constante, sin ruido, sin tener que girar)
	Vector2D p1_s, p2_s, p_mid_s; // Puntos para calcular el intervalo de giro
	Vector2D vel_needed_s, v1_s, v_mid_s, v2_s; // Velocidades para calcular los puntos p1,p2,p_mid
	Vector2D ball_vel_s;	// Velocidad absoluta del balón, cartesianas
	int		 start_time_s; //= NDEF_NUM;
	int		 time_to_reach_s;
	// Datos calculados del segundo ajuste
	Vector2D player_pos_s2;
	Vector2D player_vel_s2; // Velocidad absoluta del agente, cartesianas. Calculada al "inicio" de la habilidad
	Vector2D ball_pos_s2; // Posición absoluta del balón, cartesianas
	Vector2D intercept_point_s2; // Punto de intercepción teórico (velocidad constante, sin ruido, sin tener que girar)
	Vector2D p1_s2, p2_s2, p_mid_s2; // Puntos para calcular el intervalo de giro
	Vector2D vel_needed_s2, v1_s2, v_mid_s2, v2_s2; // Velocidades para calcular los puntos p1,p2,p_mid
	Vector2D ball_vel_s2;	// Velocidad absoluta del balón, cartesianas
	int		start_time_s2;// = NDEF_NUM;
	int		time_to_reach_s2;
	int		last_call_time;

	int		start_time_side_walk;
	bool	on_side_walk;
	bool	side_walk_case;

	bool    first_turn_requested;
	bool	first_turn_done;
	bool	second_turn_requested;
	bool 	second_turn_done;

	double turn_param1;
	double turn_param2;

	double search_turn_param;
	bool   on_search;

	double 			dash_param;
	double 			turn_neck_param;
	ViewModeWidth 	view_mode_w_param;

	double side_walk_dash_param;
	double side_walk_angle_param;

	//double kick_angle_param;
	//double kick_power_param;

	void computePointTurn1();
	void computePointTurn2();
	void go();
	void checkSideWalk();
	//void sidewalk();
	void lookAtBall();
	void search();

};

class FreezeBall
{
public:
	FreezeBall( GameData * game_data, WorldModelV1 * world );
	void call( AgentCommand * command );
private:
	GameData 	 * game_data;
	WorldModelV1 * world;
	AgentCommand * command;

	//double pow_param;
	//double angle_param;
};
#endif // POKTAPOK_AGENT_H
