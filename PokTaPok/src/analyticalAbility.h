#ifndef ANALYTICAL_ABILITY_H
#define ANALYTICAL_ABILITY_H
#include "utilities.h"
#include "geometry.h"
#include "gameCommand.h"
#include "worldModel.h"

#include <cstdio>
#include <cmath>
#include <iostream>

class BallInterception;
class FreezeBall;
class ServerParam;
class GameData;
class WorldModelV1;


Vector2D
DePrimera( Vector2D const & vt1, // Velocidad deseada en el siguiente ciclo
		   Vector2D const & vt0, // velocidad del balón en el ciclo actual
		   double ball_dis, // Distancia al balón
		   double ball_dir_deg, //  Dirección del balón con respecto al cuerpo
		   double kickable_margin,
		   double kick_power_rate,
		   double ball_decay
		   );

Vector2D
velToInterceptBall( Vector2D const & b, // Posición del balón
		Vector2D const & p, // Posición del jugador
		Vector2D const & v, // Velocidad del balón
		int t,
		double ball_decay // constante de decaimiento
		);

double ActualKickPowerRate(int dir,double dist);


double KickPowerForSpeed(double speed,double actkpr);

//     KickSpeedToTravel(distancia Player-Objetivo , Vel final Deseada)
double KickSpeedToTravel(double d,double e);

class BallInterception
{
public:
	BallInterception( GameData 		* game_data,
				   WorldModelV1 	* world,
				   FreezeBall		* freeze_ball );
	void reset(); //
	void call( AgentCommand * command);

	enum State
	{
		START,
		SEARCH_BALL,
		COMPUTE_POINT_TURN,
		GO,
		FREEZE,
		PASIVE_INTERCEPTION
	};
private:
	//Datos del juego
	GameData 	 * game_data;
	AgentCommand * command;
	WorldModelV1 * world;
	ServerParam  * param ;
	SeeSensor    * visual;
	BodySensor 	 * body  ;

	// Dependencias con otras habilidades
	FreezeBall	 * freeze_ball;

	// velocidad promedio del jugador, obtenida experimentalmente
	static double const AVERAGE_VEL = 0.94;

	// Estados de la habilidad
	State state;
	bool  turn_requested;
	void start();
	void searchBall();
	void computePointTurn();
	void go();
	void freeze();
	void pasiveInterception();

	// Datos calculados para la intercepción, se calculan en el tiempo start_time_s
	Vector2D player_pos_s; // Posición del agente
	Vector2D player_vel_s; // Velocidad absoluta del agente, cartesianas. Calculada al "inicio" de la habilidad
	Vector2D ball_pos_s; // Posición absoluta del balón, cartesianas
	Vector2D intercept_point_s; // Punto de intercepción teórico (velocidad constante, sin ruido, sin tener que girar)
	Vector2D p1_s, p2_s, p_mid_s; // Puntos para calcular el intervalo de giro
	Vector2D vel_needed_s, v1_s, v_mid_s, v2_s; // Velocidades para calcular los puntos p1,p2,p_mid
	Vector2D ball_vel_s;	// Velocidad absoluta del balón, cartesianas
	int		 start_time_s; //= NDEF_NUM;
	int		 time_to_reach_s;

	int		 last_call_time;

	double 	 turn_param;

	double 	 search_turn_param;

	double 			dash_param;
	double 			turn_neck_param;
	ViewModeWidth 	view_mode_w_param;
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
};

Vector2D PasePunto( double xTarget,    // punto final
		double yTarget,
		double x,          // coordenadas agente
		double y,
		int    angle,      // angulo del cuerpo
		double velDes,     // velocidad final
		double xBall,      // coordenadas de la pelota
		double yBall);


void GoToXY ( double   xTarget ,
		double   yTarget ,
		double   x,
		double   y,
		double   angle,
		double 	 neck_dir,
		double   radio,
		Vector2D velocidad,
		AgentCommand * command
);

void GoToXYSlow( double   xTarget ,
		double   yTarget ,
		double   x,
		double   y,
		double   angle,
		double 	 neck_dir,
		double   radio,
		Vector2D velocidad,
		AgentCommand * command
);

void runWithBall (double   xTarget ,
                  double   yTarget ,
                  double   xBall,
                  double   yBall,
                  double   disBall,
                  Vector2D velocidad,
                  double   radio,
                  WorldModelV1 * world,
                  AgentCommand * command
                  );

//*Define una zona de tiro como un circulo*//
bool  isZoneShoot( double x, double y , double radio);

//* Verdadero si ve un poste al menos *//
bool posteVisible ( vector<Flag> *banderas );

//*Nos regresa un angulo para tirar junto al poste más cercano*//
double shootAtGoal(double x, double y, double angle, vector<Flag> *banderas );


PossessionBall whoHasTheBall(WorldModelV1 *world);

void searchBall( AgentCommand * command,
                 WorldModelV1 * world,
                 SensorType sensor_type);

void align(double neckDir,
            AgentCommand *command);

void centerBall(bool   ballVisible,
                double ballDir,
                double lastAngleBall,
                double neckDir,
                ViewModeWidth cono,
                AgentCommand * command);

bool amTheClosest( WorldModelV1 *world);

void radar( double neck_dir,
		    AgentCommand *command,
		    SensorType sensor_type);

void GoToXY2 (   double   xTarget ,
                 double   yTarget ,
                 double   radio,
                 Vector2D velocidad,
                 AgentCommand * command,
                 WorldModelV1 * world
                 );

void porteroLine( double xTarget, Vector2D velocidad , SensorType sensor_type ,WorldModelV1 *world,AgentCommand *command);

int aQuienPasar(WorldModelV1 *world);

void centerBall( WorldModelV1 *world,
                 AgentCommand * command);

void GoToXY (   double   xTarget ,
		double   yTarget ,
		double   radio,
		Vector2D velocidad,
                AgentCommand * command,
                WorldModelV1 * world
                );

#endif //ANALYTICAL_ABILITY_H

