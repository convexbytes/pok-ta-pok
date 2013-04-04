#include "poktapokAgent.h"
#include "formation.h"
#include "analyticalAbility.h"
#include <cmath>
PokTaPokAgentV1::PokTaPokAgentV1( GameData * game_data )
{
    world = new WorldModelV1( game_data );
    freeze_ball	      = new FreezeBall		( game_data, world );
    ball_interception = new BallInterception	( game_data, world, freeze_ball );

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
	Vector2D position;
	switch( world->me.unum )
	{
	case 1:
		position.set( -45, 0 );
		break;
	case 2: position.set( -5, 15 );
		if( world->me.synch_see_on == false )
			command->append_synch_see();
		if( world->me.view_mode_q != HIGH
				|| world->me.view_mode_w != NARROW )
			command->append_change_view( HIGH, NARROW );
		break;
	case 3: position.set( -10, 0 );
		break;
	case 4: position.set( -45, -20 );
		break;
	case 5: position.set( -45, -15 );
		break;
	case 6: position.set( -45, -10 );
		break;
	case 7: position.set( -45, -5 );
		break;
	case 8: position.set( -45, 5 );
		break;
	case 9: position.set( -45, 10 );
		break;
	case 10: position.set( -45, 15 );
		break;
	case 11: position.set( -45, 20 );
		break;
	}
	this->on_incercept= false;
	if( (world->me.pos - position).normita() > 2 )
		command->append_move( position.x, position.y );
}

void PokTaPokAgentV1::onPrep()
{

}

void PokTaPokAgentV1::onPlay()
{
	switch( world->me.unum )
	{
	case 1:playDefault();
		break;
	case 2: play2();
		break;
	case 3: play3();
		break;
	case 4:playDefault();
		break;
	case 5:playDefault();
		break;
	case 6:playDefault();
		break;
	case 7:playDefault();
		break;
	case 8:playDefault();
		break;
	case 9:playDefault();
		break;
	case 10:playDefault();
		break;
	case 11:playDefault();
		break;
	}
}

void
PokTaPokAgentV1::playDefault()
{
	SeeSensor & visual = game_data->sensor_handler.last_see;
	if( world->me.view_mode_q != HIGH || world->me.view_mode_w != WIDE)
	{
		command->append_change_view( HIGH, WIDE );
	}
	if( visual.ballIsVisible() )
	{
		//command->append_turn_neck( visual.ball.dir );
		if( visual.ball.dis < 1.5 )
			command->append_kick( 100, 0 );
	}
	command->append_turn( 180 );
}

void
PokTaPokAgentV1::play2()
{
	BodySensor & body   = game_data->sensor_handler.last_sense;
	SeeSensor & visual = game_data->sensor_handler.last_see;
	Vector2D static target(40, 15);
	Vector2D agent_pos = world->me.pos;
	Vector2D agent_vel_rel_polar( body.speed_amount, body.speed_direction );



	if( (world->me.pos - target).normita() > 3 && on_incercept == false )
	{
		GoToXY( target.x, target.y,
				agent_pos.x, agent_pos.y,
				world->me.angle,
				1.0,
				agent_vel_rel_polar,
				command );
	}
	else
	{
		if( (world->time - world->bitacoraBalon.begin()->ciclo < 15)
				|| on_incercept == true )
		{
			Vector2D ball_pos 	 = world->bitacoraBalon.begin()->pos;
			Vector2D mate_pos    = world->bitacoraAmigos[2].begin()->pos;
			double   dis_to_ball = (agent_pos - ball_pos).normita();
			double   mate_to_ball = (mate_pos - ball_pos).normita();

			if ( dis_to_ball <  15 )
			{
				on_incercept = true;
				if( dis_to_ball <= 1 )
				{
					Vector2D ball_vel = world->bitacoraBalon.begin()->vel;
					if( ball_vel.x == NDEF_NUM )
						ball_vel = 0;
					if( ball_vel.normita() > 0.5 )
					{
						freeze_ball->call( command );
						return;
					}
					//else
					//{
						if( isZoneShoot(agent_pos.x, agent_pos.y, world->me.angleDeg()) )
						{
							Vector2D poste_arriba(52.5, -5);
							Vector2D poste_abajo(52.5, -5);
							Vector2D poste_ar_agente = poste_arriba - agent_pos;
							Vector2D poste_ab_agente = poste_abajo - agent_pos;
							double 	 d1 = poste_ar_agente.normita();
							double	 d2 = poste_ab_agente.normita();
							double alpha1 = Rad2Deg( poste_ar_agente.angle() );
							double alpha2 = Rad2Deg( poste_ab_agente.angle() );
							double kick_ang;
							if( d1<d2 )
								kick_ang = entre180( alpha1 - world->me.angleDeg() );
							else
								kick_ang = entre180( alpha2 - world->me.angleDeg() );


							//		shootAtGoal(agent_pos.x, agent_pos.y,
							//					world->me.angleDeg(),
							//					& visual.flags);
							command->append_kick( 100, kick_ang );
						}
						else
						{
							runWithBall( target.x, target.y,
														 agent_pos.x, agent_pos.y,
														 world->me.angleDeg(),
														 ball_pos.x, ball_pos.y,
														 visual.ball.dis,
														 agent_vel_rel_polar,
														 1.0,
														 command );
						}
					//}
				}
				else
				{
					ball_interception->call(command );
				}
			}
			else
			{
				if( visual.ballIsVisible() )
					command->append_turn( visual.ball.dir );
			}
		}
		else
		{
			command->append_turn( 60 );
		}
	}


}

void
PokTaPokAgentV1::play3()
{
	BodySensor & body   = game_data->sensor_handler.last_sense;
	SeeSensor & visual = game_data->sensor_handler.last_see;
	Vector2D static target(40, 15);
	Vector2D agent_pos = world->me.pos;
	Vector2D agent_vel_rel_polar( body.speed_amount, body.speed_direction );
	Vector2D ball_pos;

	if( world->me.view_mode_q != HIGH || world->me.view_mode_w != NARROW)
	{
		command->append_change_view( HIGH, NARROW );
	}

	if( world->bitacoraBalon.size() > 0 )
		ball_pos 	 = world->bitacoraBalon.begin()->pos;
	else
		ball_pos	 = 0;
	//Vector2D mate_pos    = world->bitacoraAmigos[1].begin()->pos;



	double   dis_to_ball = (agent_pos - ball_pos).normita();
	double   mate_to_ball = (target - ball_pos).normita(); // target
	double   dis_to_mate = ( target - agent_pos).normita();


	if( dis_to_ball < mate_to_ball )
	{
		if( visual.ballIsVisible() && visual.ball.dis > 1 )
		{
			ball_interception->call( command );
		}
		else
		{
			if( dis_to_mate<30 && visual.ballIsVisible() && visual.ball.dis<=1.0 )
			{
				command->append_kick( 90, 0 );
			}
			else
			{
				Vector2D agent_vel_rel_polar( body.speed_amount, body.speed_direction );

				runWithBall( target.x, target.y,
							 agent_pos.x, agent_pos.y,
							 world->me.angleDeg(),
							 ball_pos.x, ball_pos.y,
							 dis_to_ball,
							 agent_vel_rel_polar,
							 1.0,
							 command );
			}
		}
	}
	else
	{
		command->append_turn( 20 );
	}
}


