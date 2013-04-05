#include "poktapokAgent.h"
#include "formation.h"
#include "analyticalAbility.h"
#include <cmath>


PokTaPokAgentV1::PokTaPokAgentV1( GameData * game_data )
{
	this->game_data	  = game_data;
	param			  = & game_data->game_parameter;
	world 			  = new WorldModelV1	 ( game_data );
	freeze_ball	      = new FreezeBall		 ( game_data, world );
	ball_interception = new BallInterception ( game_data, world, freeze_ball );

	goal_l.set(-50.0, 0.0 );
	goal_r.set( 50.0, 0.0 );


}
PokTaPokAgentV1::~PokTaPokAgentV1()
{
	delete world;
	delete ball_interception;
	delete freeze_ball;
}

void PokTaPokAgentV1::do_process( GameData *game_data,
		AgentCommand *agent_command )
{

	// Guardamos las referencias a los datos del juego
	this->game_data = game_data;
	this->command   = agent_command;
	// Guardamos referencias extra, para acceder más rápido a los miembros
	this->command_c = &game_data->command_commited;
	this->sensor_h  = &game_data->sensor_handler;
	this->param     = &game_data->game_parameter;

	command->reset();

	update();

	switch( state_m.state() )
	{
	case SM_INITIAL:
		this->onInitial();
		break;
	case SM_PREP:
		this->onPrep();
		break;
	case SM_PLAY:
		this->onPlay();
		break;
	}
}

void PokTaPokAgentV1::update()
{
	world->update( this->game_data );
	state_m.update( *this->game_data );
}

void PokTaPokAgentV1::onInitial()
{
	init_pot_field();
}

void PokTaPokAgentV1::onPrep()
{

}

void PokTaPokAgentV1::onPlay()
{

	vector<Player> &agentes =sensor_h->last_see.players;
	Ball &ball =sensor_h->last_see.ball;
	double x=world->me.pos.x;
	double y=world->me.pos.y;
	double angle = world->me.angle;
	double neck_dir = game_data->sensor_handler.last_sense.head_angle;

	SeeSensor *vision;

	vision = &game_data->sensor_handler.last_see;



	world->me.possession_ball = whoHasTheBall( vision->ballIsVisible(),
												ball.dis,
												ball.dir,
												x ,
												y ,
												angle ,
												neck_dir,
												&agentes);

	switch(world->me.possession_ball)
	{
	case PROPIA: 	  balonPropio();		break;        // balón en poder del propio agente
	case EQUIPO:	  balonEquipo();		break;        // balón en poder del equipo propio
	case RIVAL: 	  balonRival(); 		break;        // balón en poder del rival
	case DESCONOCIDA: balonDesconocido();	break;        // balón en poder desconocida
	case SUELTA: 	  balonSuelto();		break;        // balón suelto
	case PERDIDA: 	  balonPerdido();   	break;        // no se ve el balón
	default:                                break;
	}

}

void PokTaPokAgentV1::balonPropio()
{
	update_pot_field();

	Vector2D resultant 		 = pot_field.resultante();
	double   resultant_angle = resultant.angle();

	bool pass = false; //True si el pase es el que tiene menor ángulo con la resultante
	bool goal = false; //True si estamos en la zona de tiro
	bool run  = false;  //True si la meta tiene menor ángulo con la resultante

	double 	 pass_angles[11];
	double 	 min_pass_angle;
	int	     min_pass_angle_position;
	Vector2D pass_point;

	Vector2D run_vector = goal_pos - world->me.pos;
	double 	 run_angle  = run_vector.angle();

	// Calculamos los ángulos de pase y el que tenga menos ángulo con la resultante
	min_pass_angle = pot_field.ally_vec[0].angle() - resultant_angle;
	min_pass_angle_position = 0;
	for( int i=1; i< 11; i++ )
	{
		if( pot_field.ally_on[i]  )
		{
			pass_angles[i] = pot_field.ally_vec[i].angle() - resultant_angle;
			if( pass_angles[i] < min_pass_angle )
			{
				min_pass_angle = pass_angles[i];
				min_pass_angle_position = i;
			}
		}
	}
	pass_point = min_pass_angle_position;

	if( min_pass_angle < run_angle )
	{
		pass = true;	goal = false;	run = false;
	}
	else
	{
		pass = false; 	goal = false;	run = true;
	}
	if( isZoneShoot( world->me.pos.x, world->me.pos.y, world->me.angle) )
	{
		goal = true;
		pass = false;
		run = false;
	}

	if( goal == true )
	{

	}
	else if( run == true )
	{
		runWithBall( goal.x, goal.y,
				     world->me.pos.x, world->me.pos.y,
				     world->me.angle,
				     );
	}
	else
	{

	}


}
void PokTaPokAgentV1::balonEquipo()
{

}
void PokTaPokAgentV1::balonRival()
{

}
void PokTaPokAgentV1::balonDesconocido()
{

}
void PokTaPokAgentV1::balonSuelto()
{
 // GoToXY();
}
void PokTaPokAgentV1::balonPerdido()
{
// searchBall();
}


void PokTaPokAgentV1::init_pot_field()
{

	if( game_data->sensor_handler.last_sensor_type == SENSOR_INIT )
	{
		// Actualizamos la meta en base a nuestro lado de la cancha
		if( world->me.side == 'l' )
			goal_pos = goal_r;
		else
			goal_pos = goal_l;
		pot_field.setDest( goal_pos );
		// Apagamos el vector de nuestra posición
		pot_field.turnOffTeammate( world->me.unum );
	}
}

void PokTaPokAgentV1::update_pot_field()
{
	deque<ObjetoBitacora>::const_iterator teammate_it;
	deque<ObjetoBitacora>::const_iterator opponent_it;
	deque<ObjetoBitacora>::const_iterator ball;

	int i;

	pot_field.setPos( world->me.pos );

	for( i=0; i < 11; i++ )
	{
		if( i == world->me.unum -1 )
			continue;
		teammate_it = world->bitacoraAmigos[i].begin();
		if( world->time - teammate_it->ciclo < POT_FIELD_EXPIRE_TIME )
		{ // la información aún es útil
			pot_field.setTeammate	   ( teammate_it->pos, teammate_it->num );
			pot_field.turnOnTeammate ( teammate_it->num );
		}
		else // La información es muy vieja y ya no nos sirve para decidir
			pot_field.turnOffTeammate( teammate_it->num );

	}

	for( i=0; i < 11; i++ )
	{
		opponent_it = world->bitacoraRivales[i].begin();
		if( world->time - opponent_it->ciclo < POT_FIELD_EXPIRE_TIME )
		{ // la información aún es útil
			pot_field.setOpponent	   ( opponent_it->pos, opponent_it->num );
			pot_field.turnOnOpponent ( opponent_it->num );
		}
		else // La información es muy vieja y ya no nos sirve para decidir
			pot_field.turnOffOpponent( opponent_it->num );
	}


}

