#include "poktapokAgent.h"
#include "formation.h"
#include "analyticalAbility.h"
#include <cmath>
PokTaPokAgentV1::PokTaPokAgentV1( GameData * game_data )
{
    world = new WorldModelV1( game_data );
    freeze_ball	   = new FreezeBall		( game_data, world );
    ball_intercept = new BallIntercept	( game_data, world, freeze_ball );

}
PokTaPokAgentV1::~PokTaPokAgentV1()
{
	delete world;
	delete ball_intercept;
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
	ball_intercept->reset();
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

	if( (world->me.pos - target).normita() > 3 && on_incercept == false ){
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
				if( dis_to_ball < 1 )
				{
					Vector2D ball_vel = world->bitacoraBalon.begin()->vel;
					if( ball_vel.x == NDEF_NUM )
						ball_vel = 0;
					//if( ball_vel.normita() > 0.5 )
					//{
					//	freeze_ball->call( command );
					//	return;
					//}
					//else
					//{
						if( isZoneShoot(agent_pos.x, agent_pos.y, world->me.angleDeg()) )
						{
							double kick_ang =
									shootAtGoal(agent_pos.x, agent_pos.y,
												world->me.angleDeg(),
												& visual.flags);
							command->append_kick( 100, kick_ang );
							std::cout << "kick done " << world->time << std::endl;
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
					ball_intercept->call( command );
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
	Vector2D ball_pos 	 = world->bitacoraBalon.begin()->pos;
	Vector2D mate_pos    = world->bitacoraAmigos[1].begin()->pos;



	double   dis_to_ball = (agent_pos - ball_pos).normita();
	double   mate_to_ball = (target - ball_pos).normita(); // target
	double   dis_to_mate = ( target - agent_pos).normita();


	if( dis_to_ball < mate_to_ball )
	{
		if( visual.ballIsVisible() && visual.ball.dis > 1 )
		{
			ball_intercept->call( command );
		}
		else
		{
			//bool player2_visible = false;
			//for( vector<Player>::iterator it = visual.players.begin();
			//		it != visual.players.end() && player2_visible == false ;
			//		++it )
			//{
			//	if (it->unum == 2)
			//		player2_visible = true;
			//}

			if( dis_to_mate<30 )
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
		command->append_turn( 60 );
	}
}


BallIntercept::BallIntercept( GameData 	   * game_data,
							  WorldModelV1 * world,
							  FreezeBall   * freeze_ball)
{
	this->game_data = game_data;
	this->world		= world;
	this->freeze_ball = freeze_ball;
	this->param     = &game_data->game_parameter.server_param;
	this->visual	= &game_data->sensor_handler.last_see;
	this->body		= &game_data->sensor_handler.last_sense;
	this->start_time_s  = NDEF_NUM;
	this->start_time_s2 = NDEF_NUM;
	this->last_call_time= NDEF_NUM;
	this->turn_param1	= NDEF_NUM;
	this->turn_param2	= NDEF_NUM;
	time_to_reach_s = NDEF_NUM;
	time_to_reach_s2 = NDEF_NUM;
	reset();
}

void
BallIntercept::reset()
{
	first_turn_done  	 = false;
	first_turn_requested = false;
	second_turn_done 	 = false;
	second_turn_requested= false;
	on_search 	 = false;
	on_side_walk = false;
	turn_param1	= NDEF_NUM;
	turn_param2	= NDEF_NUM;
	start_time_s  = NDEF_NUM;
	start_time_s2 = NDEF_NUM;
	start_time_side_walk = NDEF_NUM;
	time_to_reach_s = NDEF_NUM;
	time_to_reach_s2 = NDEF_NUM;
}

void
BallIntercept::call( AgentCommand * command )
{
	this->command = command;
	if( world->bitacoraBalon.size() < 2 )
	{
		search(); // calcula search_turn_param
		command->append_turn( search_turn_param );
		//std::cout << "time " << world->time << " search return " << std::endl;
		return;
	}
	if( on_search && game_data->sensor_handler.last_see.ballIsVisible()  )
	{
		reset();
		call( command );
		//std::cout << "time " << world->time << " freeze return " << std::endl;
	}

	if( world->time - last_call_time > 1 )
		reset();

	if( first_turn_requested == true )
	{
		if( game_data->command_commited.turn_is_set()
		    && game_data->command_commited.turn_angle == turn_param1 )
		    first_turn_done = true;
	}
	if( second_turn_requested == true )
	{
		if( game_data->command_commited.turn_is_set()
		    && game_data->command_commited.turn_angle == turn_param2 )
		    second_turn_done = true;
	}
	// Elegimos la acción
	Vector2D bn;
	Vector2D v;

	if( (visual->ballIsVisible() && visual->ball.dis<1.0)
			 )
	{

		freeze_ball->call( command );
		reset();
	}
	else
	if( on_search == true )
	{
		search();
		command->append_turn( search_turn_param );
	}
	else
	if( first_turn_done == false )
	{

		computePointTurn1(); // genera turn_param1
		command->append_turn( turn_param1 );
		first_turn_requested = true;
		// Aquí se incluirá la habilidad checksidewalk

		//std::cout << "time " << world->time << " compute 1 " << std::endl;
	}

	else
	{
		if( ( (world->time - start_time_s) > time_to_reach_s/2  && second_turn_done == false )
			   )
		{
			computePointTurn2(); // genera turn_param2
			command->append_turn( turn_param2 );
			second_turn_requested = true;

			//std::cout << "time " << world->time << " compute 2 " << std::endl;
		}
		else if( (world->time - start_time_s2 > time_to_reach_s2 || world->time - start_time_s2 > 15 )
				&& second_turn_done == true
				 )
		{	// Terminó el tiempo en que debimos alcanzar el balón
			//std::cout << "time " << world->time << " chase " << std::endl;
			on_search = true;
			call( command ); //  Volvemos a llamarla para que calcule el nuevo turn
		}
		else
		{
			//std::cout << "time " << world->time << " go " << std::endl;

			go(); // Genera dash_param
			command->append_dash( dash_param );
		}
	}
	last_call_time = world->time;
}

void
BallIntercept::computePointTurn1()
{
	double angle_deg, angle_min_deg, angle_max_deg;

	world->predictBallCurrentVel( &ball_vel_s );
	ball_pos_s		   = world->bitacoraBalon.begin()->pos;
	player_pos_s 	   = world->me.pos;
	start_time_s	   = world->time;
	// Iteramos hasta que encontramos una velocidad promedio
	// que puede lograr el agente a lo largo de los time_to_reach_s ciclos
	time_to_reach_s = 0;
	do
	{
		++time_to_reach_s;
		vel_needed_s = velToInterceptBall( ball_pos_s,
				player_pos_s,
				ball_vel_s,
				time_to_reach_s);
	}
	while( vel_needed_s.normita() >= mean_vel );

	//Calculamos el punto de intercepción teórico
	intercept_point_s = player_pos_s + vel_needed_s* time_to_reach_s;

	// Calculamos velocidades necesarias para llegar en los siguientes
	// ciclos, contemplamos 2 ciclos de ajuste de giro
	v1_s = velToInterceptBall( ball_pos_s,
			player_pos_s,
			ball_vel_s,
			time_to_reach_s + 1 );
	v_mid_s = velToInterceptBall( ball_pos_s,
			player_pos_s,
			ball_vel_s,
			time_to_reach_s + 2 );
	v2_s = velToInterceptBall( ball_pos_s,
			player_pos_s,
			ball_vel_s,
			time_to_reach_s + 3 );
	// Calculamos puntos de intersección posibles, por si "llegamos tarde"
	// Asumimos que no podemos llegar al punto teórico debido a que debemos
	// hacer un turn para alienarnos, lo cual nos resta un ciclo
	p1_s		= player_pos_s +  v1_s    *( time_to_reach_s +1 );
	p_mid_s 	= player_pos_s +  v_mid_s *( time_to_reach_s +2 );
	p2_s 		= player_pos_s +  v2_s    *( time_to_reach_s +3 );
	// Calculamos el ángulo que hay que girar en cada punto
	angle_deg     =	Rad2Deg ( ( p_mid_s- player_pos_s ).angle() );
	angle_min_deg = Rad2Deg ( ( p1_s   - player_pos_s ).angle() );
	angle_max_deg = Rad2Deg ( ( p2_s   - player_pos_s ).angle() );

	//Agregamos margen de error debido al ruido en sensores
	angle_min_deg -=  5 ;
	angle_max_deg +=  5 ;

	// Calculamos el turn necesario para alinearnos con el punto
	turn_param1 =
			entre180( angle_deg - world->me.angleDeg() - world->me.headAngleDeg() )
			* (1.0 + param->inertia_moment * body->speed_amount ) ;

}

void
BallIntercept::computePointTurn2()
{
	// Recalculamos el punto de intercepción ahora que la pelota va más lento
	// y ya que estamos más cerca se necesita más precisión: hay menos ruido
	// en los sensores visuales de posición y velocidad. Aunque esto es posible
	// sólo si el balón no está detrás de nosotros, en tal caso lo hacemos visible
	//
	double velocity_available;
	double angle_deg, angle_min_deg, angle_max_deg;

	velocity_available = world->predictBallCurrentVel( &ball_vel_s2 );;
	player_pos_s2 	   = world->me.pos;
	start_time_s2 	   = world->time;

	if( start_time_s == world->bitacoraBalon.begin()->ciclo )
	{ // No hemos visto el balón desde que calculamos el primer punto
		double time_elapsed = world->time - start_time_s;
		ball_pos_s2 = ball_pos_s + ball_vel_s*(1);
		//ball_vel = world->bitacoraBalon.begin()->vel;
		ball_pos_s2.x = ball_vel_s.x *
					   (std::pow( param->ball_decay, time_elapsed ) -1 ) /
					   log( param->ball_decay ) + ball_pos_s.x;
		ball_pos_s2.y = ball_vel_s.y *
				   	   (std::pow( param->ball_decay, time_elapsed ) -1 ) /
				   	   log( param->ball_decay ) + ball_pos_s.y;
	}
	else
	{
		// Obtenemos la última posición vista
		Vector2D pos_tmp = world->bitacoraBalon.begin()->pos;

		// Calculamos el tiempo que ha pasado desde que la vimos
		double time_elapsed = world->time - world->bitacoraBalon.begin()->ciclo;

		// Calculamos la velocidad actual
		if( world->bitacoraBalon.begin()->vel.x != NDEF_NUM
			&&  world->bitacoraBalon.begin()->vel.y != NDEF_NUM )
			ball_vel_s2 = world->bitacoraBalon.begin()->vel * std::pow(param->ball_decay, time_elapsed);


		ball_pos_s2.x = ball_vel_s2.x *( std::pow( param->ball_decay, time_elapsed) -1 )
						/ log( param->ball_decay ) + pos_tmp.x;
		ball_pos_s2.y = ball_vel_s2.x *( std::pow( param->ball_decay, time_elapsed) -1 )
								/ log( param->ball_decay ) + pos_tmp.y;
	}
	// Iteramos hasta que encontramos una velocidad promedio
	// que puede lograr el agente a lo largo de los time_to_reach_s2 ciclos
	time_to_reach_s2 = 0;
	do
	{
		++time_to_reach_s2;
		vel_needed_s2 = velToInterceptBall( ball_pos_s2,
				player_pos_s2,
				ball_vel_s2,
				time_to_reach_s2);
	}
	while( vel_needed_s2.normita() >= mean_vel );
	// Calculamos velocidades necesarias para llegar en los siguientes
	// ciclos
	v1_s2 = velToInterceptBall( ball_pos_s2,
			player_pos_s2,
			ball_vel_s2,
			time_to_reach_s2 + 0 );
	v_mid_s2= velToInterceptBall( ball_pos_s2,
			player_pos_s2,
			ball_vel_s2,
			time_to_reach_s2 + 1 );
	v2_s2 = velToInterceptBall( ball_pos_s2,
			player_pos_s2,
			ball_vel_s2,
			time_to_reach_s2 + 2 );
	// Calculamos puntos de intersección posibles
	p1_s2		= player_pos_s2 +  v1_s2    *( time_to_reach_s2 + 0 );
	p_mid_s2 	= player_pos_s2 +  v_mid_s2 *( time_to_reach_s2 + 1 );
	p2_s2 		= player_pos_s2 +  v2_s2    *( time_to_reach_s2 + 2 );
	// Calculamos el ángulo que hay que girar en cada punto
	angle_deg     =	Rad2Deg ( ( p_mid_s2 - player_pos_s2 ).angle() );
	angle_min_deg = Rad2Deg ( ( p1_s2  	 - player_pos_s2 ).angle() );
	angle_max_deg = Rad2Deg ( ( p2_s2  	 - player_pos_s2 ).angle() );

	//Agregamos margen de error debido al ruido en sensores
	angle_min_deg -=  5 ;
	angle_max_deg +=  5 ;

	// Calculamos el turn necesario para alinearnos con el punto
	turn_param2 =
			entre180( angle_deg - world->me.angleDeg() - world->me.headAngleDeg() )
			* (1.0 + param->inertia_moment * (body->speed_amount) ) ;

}

void
BallIntercept::go()
{
	// Estamos alineados hacia el punto de intercepción y vamos hechos la madre
	dash_param = 100;

	// Por ahora que no podemos manipular en ángulo del cuello,
	// no podemos usar la habilidad lookAtBall para seguir el balón con la vista
	//lookAtBall();
}

void
BallIntercept::lookAtBall()
{
	int time_elapsed = world->time - world->bitacoraBalon.begin()->ciclo;
	Vector2D ball_pos = world->bitacoraBalon.begin()->pos;
	Vector2D ball_vel;
	Vector2D target;
	// Pendiente de
	if( world->bitacoraBalon.begin()->vel.x != NDEF_NUM &&
		world->bitacoraBalon.begin()->vel.y != NDEF_NUM )
	{
		ball_vel = world->bitacoraBalon.begin()->vel;
		target.x = ball_vel.x *
					   (std::pow( param->ball_decay, time_elapsed ) -1 ) /
					   log( param->ball_decay ) + ball_pos.x;
		target.y = ball_vel.y *
				   	   (std::pow( param->ball_decay, time_elapsed ) -1 ) /
				   	   log( param->ball_decay ) + ball_pos.y;

	}
	else
		target = ball_pos;

	// Look at target algorithm
	double gamma = entre180( world->me.angleDeg() - Rad2Deg(target.angle()) );
	double abs_gamma = std::fabs(gamma);
	ViewModeWidth width;
	double 		  turn_ang;
	if( abs_gamma < 90 )
	{
		width    = NARROW;
		turn_ang = gamma - world->me.headAngleDeg();
	}
	else
	{
		if( abs_gamma  < 90 + 30 ) // max turn_neck angle + narrow_cone_width/2
			width = NARROW;
		else if( abs_gamma < 90 + 60 ) // max turn_neck angle + normal_cone_width/2
			width = NORMAL;
		else
			width = WIDE;
		if( gamma > 0 )
			turn_ang = 90 - world->me.headAngleDeg();
		else
			turn_ang = -90 - world->me.headAngleDeg();
	}
	view_mode_w_param = width;
	turn_neck_param	  = turn_ang;
}

void
BallIntercept::search()
{
	search_turn_param = 60;
}

void
BallIntercept::checkSideWalk()
{
	// Revisamos, en el caso de que el punto pase por el area
	// pateable del jugador

	Vector2D & p = player_pos_s; //
	Vector2D & b = ball_pos_s;	// Posición del balón
	Vector2D & q = p_mid_s;	    // Punto te intercepción
	Vector2D p_b = p-b;
	Vector2D q_b = q-b;

	double	 alpha =  p_b.prodPunto( q_b )
					/ ( p_b.normita()*q_b.normita() ) ;

	double   beta  = std::atan2( 1.0, p_b.normita() );


	alpha = entre180( Rad2Deg(alpha) );
	beta  = entre180( Rad2Deg(beta) );

	if( std::abs(alpha) < std::abs(beta) )
	{	// Entonces el balón pasará por nuestra área pateable
		side_walk_case = true;
		// terminar
	}
}

FreezeBall::FreezeBall( GameData * game_data, WorldModelV1 * world )
{
	this->game_data = game_data;
	this->world 	= world;

}

void
FreezeBall::call( AgentCommand * command )
{
	this->command = command;
	SeeSensor   & visual = game_data->sensor_handler.last_see;
	BodySensor  & body   = game_data->sensor_handler.last_sense;
	ServerParam & param  = game_data->game_parameter.server_param;
	double pow_needed;
	double angle_needed;
	//if( !visual.ballIsVisible() )
	//{	// No podemos congelar el balón sin datos visuales de él
	//	return;
	//	}
	//Vector2D v = world->bitacoraBalon.begin()->vel; // vel...

	int		 t = world->time;	//
	Vector2D p = world->me.pos; // Posición del agente
	double theta = world->me.angleDeg();
	Vector2D b = world->bitacoraBalon.begin()->pos; // posición del balón en el ciclo tb

	Vector2D v;
	world->predictBallCurrentVel( &v ); // No usar esto...
	int		tb = world->bitacoraBalon.begin()->ciclo; // tiempo en que se vio el balón
	int t_diff = t-tb; // Tiempo transcurrido desde que se vio el balón
	Vector2D pv = Vector2D::fromPolar( body.speed_amount,
									   Deg2Rad( body.speed_direction  + theta ) );

	Vector2D pn_bn; // vector de bn a pn
	// Predicciones
	Vector2D pn = p + pv; //Posición del agente en el siguiente ciclo

	Vector2D vn = v*param.ball_decay; // Velocidad del balón en el siguiente ciclo
	Vector2D bn;
	// Calculamos la posición del balón
	bn.x =   v.x*(std::pow( param.ball_decay, t_diff +1 ) - 1 )
			/ log( param.ball_decay ) + b.x;
	bn.y =   v.y*(std::pow( param.ball_decay, t_diff +1 ) - 1 )
			/ log( param.ball_decay ) + b.y;

	Vector2D zero_vel(0.0, 0.0);

	pn_bn = pn-bn;

	double dis 		= pn_bn.normita();
	double dir_diff = std::abs( entre180( theta - Rad2Deg( pn_bn.angle() ) ) );
	double f = (1 - 0.25*(dir_diff/180.0) -0.25*(dis/param.kickable_margin) );



	if( vn.x == 0.0 && vn.y == 0.0 )
	{
		pow_needed = 0.0;
		angle_needed = 0.0;
	}
	else
	{
		pow_needed = vn.normita() / ( param.kick_power_rate * f ) ;
		angle_needed = entre180( Rad2Deg( std::atan2( vn.y, vn.x )  ) + 180 - theta);
	}

	command->append_kick( pow_needed, angle_needed );

}
