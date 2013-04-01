#include "poktapokAgent.h"
#include "formation.h"
#include "analyticalAbility.h"

PokTaPokAgentV1::PokTaPokAgentV1( GameData * game_data )
{
    world = new WorldModelV1( game_data );
    ball_intercept = new BallIntercept( game_data,
    									world );
}
PokTaPokAgentV1::~PokTaPokAgentV1()
{
	delete world;
	delete ball_intercept;
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
	Vector2D cero(0.0);

	if( world->me.synch_see_on == false )
			command->append_synch_see();
		if( world->me.view_mode_q != HIGH || world->me.view_mode_w != NARROW )
			command->append_change_view( HIGH, NARROW );

	if( (world->me.pos - cero).normita() > 2.0 )
		command->append_move(0, 0);

	ball_intercept->reset();
}

void PokTaPokAgentV1::onPrep()
{

}

void PokTaPokAgentV1::onPlay()
{
	SeeSensor 	& visual = game_data->sensor_handler.last_see;
	if( visual.ballIsVisible() && visual.ball.dis < 1 )
		command->append_kick( -10 , 0);
	else
		ball_intercept->call( command );
}


BallIntercept::BallIntercept( GameData 	   * game_data,
							  WorldModelV1 * world )
{
	this->game_data = game_data;
	this->world		= world;
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
}

void
BallIntercept::reset()
{
	first_turn_done  	 = false;
	first_turn_requested = false;
	second_turn_done 	 = false;
	second_turn_requested= false;

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
	if( game_data->sensor_handler.last_see.ballIsVisible()
		&& game_data->sensor_handler.last_see.ball.dis < 1 )
	{
		freeze(); // calcula kick_angle_param y kick_power_param
		// aún no está completa...
		//std::cout << "time " << world->time << " freeze return " << std::endl;
		return;
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

	if( first_turn_done == false )
	{
		//if( !visual->ballIsVisible() )
		//{
		//	search();
		//	return;
		//}
		computePointTurn1(); // genera turn_param1
		command->append_turn( turn_param1 );
		first_turn_requested = true;
		// Aquí se incluirá la habilidad checksidewalk

		//std::cout << "time " << world->time << " compute 1 " << std::endl;

	}

	else
	{
		if( (world->time - start_time_s) > time_to_reach_s/2
			  && second_turn_done == false )
		{
			computePointTurn2(); // genera turn_param2
			command->append_turn( turn_param2 );
			second_turn_requested = true;

			//std::cout << "time " << world->time << " compute 2 " << std::endl;
		}
		else if( world->time - start_time_s2 > time_to_reach_s2
				&& second_turn_done == true
				 )
		{	//Completamos la habilidad sin éxito
			//std::cout << "time " << world->time << " chase " << std::endl;
			reset();	// Reseteamos los parámetros
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
			time_to_reach_s2 + 1 );
	v_mid_s2= velToInterceptBall( ball_pos_s2,
			player_pos_s2,
			ball_vel_s2,
			time_to_reach_s2 + 2 );
	v2_s2 = velToInterceptBall( ball_pos_s2,
			player_pos_s2,
			ball_vel_s2,
			time_to_reach_s2 + 3 );
	// Calculamos puntos de intersección posibles
	p1_s2		= player_pos_s2 +  v1_s2    *( time_to_reach_s2 +1 );
	p_mid_s2 	= player_pos_s2 +  v_mid_s2 *( time_to_reach_s2 +2 );
	p2_s2 		= player_pos_s2 +  v2_s2    *( time_to_reach_s2 +3 );
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
BallIntercept::freeze()
{
	Vector2D ball_vel;
	if( world->bitacoraBalon.begin()->vel.x != NDEF_NUM &&
			world->bitacoraBalon.begin()->vel.y != NDEF_NUM )
	{
		ball_vel = world->bitacoraBalon.begin()->vel;
	}
	// Completar...
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

