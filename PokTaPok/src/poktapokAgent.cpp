#include "poktapokAgent.h"
#include "formation.h"
#include "analyticalAbility.h"

#include <cmath>
#include <algorithm>

PokTaPokAgent::PokTaPokAgent( GameData * game_data )
{
    world 			  = new WorldModel	 ( game_data );
    freeze_ball	      = new FreezeBall		 ( game_data, world );
    ball_interception = new BallInterception ( game_data, world, freeze_ball );
	this->game_data	  = game_data;
	param			  = & game_data->game_parameter;
	goal_l.set( -45.0, 0.0 );
	goal_r.set( 45.0, 0.0 );
}
PokTaPokAgent::~PokTaPokAgent()
{
	delete world;
	delete ball_interception;
	delete freeze_ball;
}

void PokTaPokAgent::do_process( GameData *game_data,
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

void PokTaPokAgent::update()
{
	world->update( this->game_data );
	state_m.update( *this->game_data );
}

void PokTaPokAgent::onInitial()
{

	Vector2D pos;
	switch( world->me().unum )
	{
	case 1: pos = Formation::BeforeKickOff::P1; break;
	case 2: pos = Formation::BeforeKickOff::P2; break;
	case 3: pos = Formation::BeforeKickOff::P3; break;
	case 4: pos = Formation::BeforeKickOff::P4; break;
	case 5: pos = Formation::BeforeKickOff::P5; break;
	case 6: pos = Formation::BeforeKickOff::P6; break;
	case 7: pos = Formation::BeforeKickOff::P7; break;
	case 8: pos = Formation::BeforeKickOff::P8; break;
	case 9: pos = Formation::BeforeKickOff::P9; break;
	case 10: pos = Formation::BeforeKickOff::P10; break;
	case 11: pos = Formation::BeforeKickOff::P11; break;
	}
	//if( world->me().side == 'r' )
	//pos.x = -pos.x;
	if( (world->me().pos - pos).mag() > 2.0 )
		command->append_move( pos.x, pos.y );

	if( world->me().synch_see_on == false )
		command->append_synch_see();
	if( world->me().view_mode_w != NARROW )
		command->append_change_view( HIGH, NARROW );

	//radar( world->me().headAngleDeg(), command, game_data->sensor_handler.last_sensor_type);

}

void PokTaPokAgent::onPrep()
{
	if(world->me().unum == 1)
	{
		goalieBehavior();
	}
	else
	{
        switch(game_data->sensor_handler.last_hear_referee.play_mode)
        {
        case KICK_OFF_L :
        case KICK_IN_L  :
        case FREE_KICK_L:
        case INDIRECT_FREE_KICK_L:
        case CORNER_KICK_L: ballStaticBehaviorSideL();
            break;


        case BACK_PASS_L: break;

        case OFFSIDE_L: break;

        case KICK_OFF_R :
        case KICK_IN_R  :
        case FREE_KICK_R:
        case INDIRECT_FREE_KICK_R:
        case CORNER_KICK_R: ballStaticBehaviorSideR();
            break;


        case BACK_PASS_R: break;
        case OFFSIDE_R: break;

        default:
        	break;
        }
	}
	//radar( world->me().headAngleDeg(), command, game_data->sensor_handler.last_sensor_type);
}

void PokTaPokAgent::onPlay()
{
	m_ball_possession = whoHasTheBall( );

	if(world->me().unum != 1)
	{
		switch( m_ball_possession )
		{
		case PROPIA:     m_master_status = ATAQUE;
		break;
		case EQUIPO:     m_master_status = ATAQUE;
		break;
		case RIVAL:      m_master_status = DEFENSA;
		break;
		case SUELTA:
			break;
		case PERDIDA:
			break;
		}

		if( m_master_status == ATAQUE )
			ataque();
		else
			defensa();
	}
    else // Es el portero
    {
        goalieBehavior();
    }
}

void PokTaPokAgent::ataque()
{
	int const BALL_INFO_EXPIRE_TIME = 6;

	Vector2D ball_vel = world->estBallVelocity();

	if( world->bitacoraBalon.empty() )
	{
		searchBall( command, world, game_data->sensor_handler.last_sensor_type );
	}
	else if( world->time() - world->bitacoraBalon.begin()->ciclo >= BALL_INFO_EXPIRE_TIME )
	{
		searchBall( command, world, game_data->sensor_handler.last_sensor_type );
	}
	else if(   sensor_h->last_see.ballIsVisible()
			&& sensor_h->last_see.ball.dis <= 1.0 )
	{
		balonPropio(); // Ataque activo
	}
	else if( IamTheClosest() )
	{
		if( ball_vel.mag() < 0.4 )
			irAlBalon();
		else
			ball_interception->call( command );
		//radar( world->me().headAngleDeg(), command, game_data->sensor_handler.last_sensor_type );
	}
	else
	{
#ifdef ATTACK_POSITIONING
				this->attackPositioning();
#else
				voronoiPositioning();
#endif
	}
}

void PokTaPokAgent::defensa()
{

	int const BALL_INFO_EXPIRE_TIME = 6;

	Vector2D ball_vel = world->estBallVelocity();

	if( world->bitacoraBalon.empty() )
	{
		searchBall( command, world, game_data->sensor_handler.last_sensor_type );
	}
	else if( world->time() - world->bitacoraBalon.begin()->ciclo >= BALL_INFO_EXPIRE_TIME )
	{
		searchBall( command, world, game_data->sensor_handler.last_sensor_type );
	}
	else if(   sensor_h->last_see.ballIsVisible()
			&& sensor_h->last_see.ball.dis <= 1.0 )
	{
		balonPropio(); // Ataque activo
	}
	else if( IamTheClosest() )
	{

		if( ball_vel.mag() < 0.5 )
			irAlBalon();
		else
			ball_interception->call( command );
		//radar( world->me().headAngleDeg(), command, game_data->sensor_handler.last_sensor_type );

	}
	else
	{
		defencePositioning(); // defensa
		//std::cout << "def" << std::endl;
	}
}

void PokTaPokAgent::ballStaticBehaviorSideL()
{
	int amigo;
	Vector2D kick;

	if( world->bitacoraBalon.empty() )
	{
		searchBall( command, world, game_data->sensor_handler.last_sensor_type);
	}
	else
	if(world->me().side == 'l')   // Si es a mi favor
	{
		if( world->time() - world->bitacoraBalon.begin()->ciclo < 3)
		{
			if( IamTheClosest() )   // si soy el más cercano
			{
				if( world->bitacoraBalon.begin()->dis < 1.0 )  // si el balón esta en mi posesión
				{
					amigo = aQuienPasar();
					if( amigo > 0 && amigo < 11 )   // si regresa -1 no ve ningun agente
					{
						kick = PasePunto( world->bitacoraAmigos[amigo].begin()->pos.x,
								world->bitacoraAmigos[amigo].begin()->pos.y,
								world->me().pos.x,
								world->me().pos.y,
								world->me().angle,
								3.5,
								world->bitacoraBalon.begin()->pos.x,
								world->bitacoraBalon.begin()->pos.y );

						command->append_kick(kick.x,kick.y);
					}
					else
					{
						/*radar(world->me().head_angle,
                              command,
                              game_data->sensor_handler.last_sensor_type);*/

						switch( world->me().view_mode_w)
						{
						case NARROW: command->append_turn(60.0);  break;
						case NORMAL: command->append_turn(120.0); break;
						case WIDE:   command->append_turn(180.0); break;
						default:     break; // incorrecto, error
						}

					}
				}
				else
				{
					irAlBalon();
				}
			}
			else
			{
				// Posicion ATAQUE no soy el más cercano al balón
#ifdef ATTACK_POSITIONING
				this->attackPositioning();
#else
				voronoiPositioning();
#endif
			}
		}
		else  // No veo el balón
		{
			searchBall(command, world, game_data->sensor_handler.last_sensor_type);
		}
	}
	else  // No es a mi favor
	{
		// posicion de DEFENSA
		this->defencePositioning();
	}

}
void PokTaPokAgent::ballStaticBehaviorSideR()
{
	int amigo;
	Vector2D kick;
	if( world->bitacoraBalon.empty() )
	{
		searchBall( command, world, game_data->sensor_handler.last_sensor_type);
	}
	else
	if(world->me().side == 'r')   // Si es a mi favor
	{
		if( world->time() - world->bitacoraBalon.begin()->ciclo < 3)
		{
			if( IamTheClosest() )   // Si soy el más cercano
			{
				if( world->bitacoraBalon.begin()->dis < 1.0 )  // si el balón esta en mi posesión
				{
					amigo = aQuienPasar();
					if( amigo > 0 && amigo < 11 )   // si regresa -1 no ve ningun agente
					{
						kick = PasePunto( world->bitacoraAmigos[amigo].begin()->pos.x,
								world->bitacoraAmigos[amigo].begin()->pos.y,
								world->me().pos.x,
								world->me().pos.y,
								world->me().angle,
								3.5,
								world->bitacoraBalon.begin()->pos.x,
								world->bitacoraBalon.begin()->pos.y );

						command->append_kick(kick.x,kick.y);
					}
					else
					{
						/*radar(world->me().head_angle,
                              command,
                              game_data->sensor_handler.last_sensor_type);*/

						switch( world->me().view_mode_w)
						{
						case NARROW: command->append_turn(60.0);  break;
						case NORMAL: command->append_turn(120.0); break;
						case WIDE:   command->append_turn(180.0); break;
						default:     break; // incorrecto, error
						}

					}
				}
				else
				{
					irAlBalon();
				}
			}
			else
			{
				// Posicion ATAQUE no soy el más cercano al balón
#ifdef ATTACK_POSITIONING
				this->attackPositioning();
#else
				voronoiPositioning();
#endif
			}
		}
		else  // No veo el balón
		{
			searchBall(command, world, game_data->sensor_handler.last_sensor_type);
		}
	}
	else  // No es a mi favor
	{
		// posicion de DEFENSA
		this->defencePositioning();
	}
}

void PokTaPokAgent::balonPropio()
{
	ball_interception->reset();

	BodySensor & body = game_data->sensor_handler.last_sense;

	deque<ObjetoBitacora>::const_iterator opponent_it;
	deque<ObjetoBitacora>::const_iterator teammate_it;


	vector<ActionOption> actions; // acciones disponibles
	ActionOption		 tmp_action;
	ActionOption		 run_action;
	ActionOption		 choosen_action;

	double const V 					= 0.94;
	double const BALL_FINAL_VEL 	= 1.5;
	double const BALL_RUN_FINAL_VEL = 1.0;
	double const k					= 5.0;
	double const RADIUS_1 			= 10.0;
	double const RADIUS_2 			= 25.0;
	double const RADIUS_3 			= 45.0;
	double const RUN_FACTOR			= 1.0;

	Vector2D resultant = 0; // vector resultante, hacia donde deberíamso de correr
	Vector2D g 		   = 0;	// vector hacia la meta
    Vector2D running_pot_field[11];

	double 		 d;
	bool  		is_zone_shoot;

	// Para obtener las probabilidades
	Vector2D 	 p = world->me().pos;
	Vector2D  	 q;
	double		 di, Di;
	double 		 r;
	double 		 n;
	double 		 pi;
	double 		 p_min;

	// general stuff
	int 		 i;
	int 		 j;
	Vector2D kick_vec;
	Vector2D agent_vel = Vector2D::fromPolar( body.speed_amount,
			Deg2Rad( body.speed_direction + world->me().angleDeg() + world->me().headAngleDeg() ) );

	Vector2D ball_vel = world->estBallVelocity();

	if( ball_vel.mag() > 0.5 )
	{
		freeze_ball->call( command );
		return;
	}

	Vector2D my_goal;
	// Actualizamos la meta en base a nuestro lado de la cancha
	if( world->me().side == 'l' )
	{
		my_goal  = goal_l;
		goal_pos = goal_r;
	}
	else
	{
		my_goal  = goal_r;
		goal_pos = goal_l;
	}


	if( std::fabs( my_goal.x - world->me().pos.x) < 15 )
	{
		kick_vec =
				PasePunto( 0,
						0,
						world->me().pos.x,
						world->me().pos.y,
						world->me().angleDeg(),
						3.0,     // velocidad final
						world->bitacoraBalon.begin()->pos.x,
						world->bitacoraBalon.begin()->pos.y);
		command->append_kick( kick_vec.x, kick_vec.y );
		return;
	}


	// Actualizamos el campo potencial y obtenemos la resultante para decidir la dirección de correr
	g =  goal_pos - world->me().pos;
	d = g.mag();
	if ( d != 0.0 )
		g /= d; // aún falta determinar la función para la magnitud de g
	resultant 	=  g;
	for( i=0; i < 11; i++ )
	{
		opponent_it = world->bitacoraRivales[i].begin();
		if( world->bitacoraRivales[i].size() > 0 )
		if( world->time() - opponent_it->ciclo < POT_FIELD_EXPIRE_TIME )
		{ // la información aún es útil

			running_pot_field[ i ] = world->me().pos - opponent_it->pos ;
			d = running_pot_field[i].mag();
			if( d != 0 )
				running_pot_field[i] *= (k/ (d*d) );
			//std::cout << "added repulsor " << running_pot_field[i] << std::endl;
			resultant += running_pot_field[i];
		}

	}

	run_action.target 		= world->me().pos + resultant*2.5; // 2.5 es la distancia que adelanta el balón
	run_action.action_type  = RUN;
	run_action.dist_to_goal = (goal_pos - run_action.target).mag();

	// Obtenemos la probabilidad de correr, se obtiene diferente a las demás, ya que
	// la velocidad final es menor a la del pase
	p_min = 1;
	r = (world->me().pos - run_action.target).mag(); // Calculamos la distancia hacia el punto en que corremos
	n =   std::log( 1 - r*(1-1/0.94)/BALL_RUN_FINAL_VEL ) // Calculamos el tiempo que tarda en llegar el balón
	/ std::log( 1/0.94 );

	if( n <= 0 )
	{
		n = 1; // no se puede llegar en cero ciclos
	}
	n = std::ceil(n) + 2; // Se le aumentan 2 porque es el tiempo que tardamos llegar al balón otra vez
	for( j=0; j<11; j++ )
	{
		opponent_it = world->bitacoraRivales[j].begin();
		if( world->bitacoraRivales[i].size() > 0 )
		if( world->time() - opponent_it->ciclo < PASS_OPTION_EXPIRE_TIME )
		{
			di = (run_action.target-opponent_it->pos).mag();
			Di = di/n;
			pi = 1 - V/(V+Di); // probabilidad de que el pase/movimiento sea exitoso
			if( pi < p_min )
				p_min = pi;
		}
	}
	run_action.prob = p_min*RUN_FACTOR; // Añadido RUN_FACTOR para darle preferencia a los pases y al tiro
	run_action.unum = 0.0;

	// Añadimos la acción al conjunto de acciones posibles
	actions.push_back( run_action );


	for( i=0; i<11; i++ ) // Para cada teammate
	{
		teammate_it = world->bitacoraAmigos[i].begin();
		p_min = 1;
		if( world->bitacoraAmigos[i].size() > 0 )
		if( world->time() - teammate_it->ciclo < PASS_OPTION_EXPIRE_TIME
				&&  i != (world->me().unum -1) // No nos consideramos a nosotros mismos
				&&  i != 0	// No consideramos al portero para los pases
		)
		{ // la información aún es útil

			tmp_action.action_type = PASS;

			tmp_action.target 		= teammate_it->pos;

			tmp_action.dist_to_goal = ( tmp_action.target - goal_pos ).mag();

			r  = (p-tmp_action.target).mag(); // Calculamos la distancia hacia el receptor

			n  =    std::log( 1 - r*(1-1/0.94)/BALL_FINAL_VEL ) // Calculamos el tiempo que tarda en llegar al receptor
			/ std::log( 1/0.94 );

			n  = std::ceil(n);

			if( n <= 0 )
			{
				n = 1; // no se puede llegar en cero ciclos
			}
			for( j=0; j<11; j++ ) // Calculamos la probabilidad de éxito con cada uno de los rivales y tomamos la mínicma
			{
				opponent_it = world->bitacoraRivales[j].begin();
				if( world->bitacoraRivales[j].size() > 0 )
				if( world->time() - opponent_it->ciclo < PASS_OPTION_EXPIRE_TIME )
				{
					di = ( tmp_action.target - opponent_it->pos ).mag();
					Di = di/n;
					pi = 1 - V/(V+Di);

					if( pi < p_min )	p_min = pi;
				}
			}

			tmp_action.prob = p_min;
			tmp_action.unum = teammate_it->num;
			actions.push_back( tmp_action );
		}

	}

	// Calculamos para la acción shoot
	p = world->me().pos;
	Vector2D shoot_target_up  ( 52.5, -5 );
	Vector2D shoot_target_down( 52.5, 5  );
	if( world->me().side == 'r' )
	{
		p.x = -p.x;
		shoot_target_up.x   = -shoot_target_up.x;
		shoot_target_down.x = -shoot_target_up.x;
	}

	is_zone_shoot = isZoneShoot( p.x, p.y, 22.0 );



	p_min = 1;
	if( is_zone_shoot )
	{
		tmp_action.action_type  = SHOOT;
		tmp_action.target 		= shoot_target_up;
		tmp_action.dist_to_goal = 0.0; //

		r  = (p-tmp_action.target).mag(); // Calculamos la distancia hacia el receptor

		n  =    std::log( 1 - r*(1-1/0.94)/BALL_FINAL_VEL ) // Calculamos el tiempo que tarda en llegar al receptor
		/ std::log( 1/0.94 );

		n  = std::ceil(n);

		if( n <= 0 )
		{
			n = 1; // no se puede llegar en cero ciclos
		}

		for( j=0; j<11; j++ ) // Calculamos la probabilidad de éxito con cada uno de los rivales y tomamos la mínicma
		{
			opponent_it = world->bitacoraRivales[j].begin();
			if( world->bitacoraRivales[j].size() > 0 )
			if( world->time() - opponent_it->ciclo < PASS_OPTION_EXPIRE_TIME )
			{
				di = ( tmp_action.target - opponent_it->pos ).mag();

				Di = di/n;
				pi = 1 - V/(V+Di);

				if( pi < p_min )	p_min = pi;
			}
		}
		tmp_action.prob = p_min;
		actions.push_back( tmp_action );

		tmp_action.action_type  = SHOOT;
		tmp_action.target 		= shoot_target_down;
		tmp_action.dist_to_goal = 0.0;

		r  = (p-tmp_action.target).mag(); // Calculamos la distancia hacia el receptor

		n  =    std::log( 1 - r*(1-1/0.94)/BALL_FINAL_VEL ) // Calculamos el tiempo que tarda en llegar al receptor
		/ std::log( 1/0.94 );

		n  = std::ceil(n);

		if( n <= 0 )
		{
			n = 1; // no se puede llegar en cero ciclos
		}

		for( j=0; j<11; j++ ) // Calculamos la probabilidad de éxito con cada uno de los rivales y tomamos la mínicma
		{
			opponent_it = world->bitacoraRivales[j].begin();
			if( world->bitacoraRivales[j].size() > 0 )
			if( world->time() - opponent_it->ciclo < PASS_OPTION_EXPIRE_TIME )

			{
				di = ( tmp_action.target - opponent_it->pos ).mag();
				Di = di/n;
				pi = 1 - V/(V+Di);

				if( pi < p_min )	p_min = pi;
			}
		}
		tmp_action.prob = p_min;
		tmp_action.unum = 12;
		actions.push_back( tmp_action );
	}


	// Ahora que tenemos todas las acciones, seleccionamos la mejor con base en: la ganancia en
	// distancia a la meta, y la probabilidad

	std::sort( actions.begin(), actions.end(), ActionOption::distanceLowerThan );

	vector<ActionOption>::const_iterator action_it = actions.begin();
	double area_r;
	bool area_changed = false;

	if( action_it->dist_to_goal == 0 )
	{
		area_r = 0.0;
	}
	if( action_it->dist_to_goal < RADIUS_1 )
	{
		area_r = RADIUS_1;
	}
	else if( action_it->dist_to_goal < RADIUS_2 )
	{
		area_r = RADIUS_2;
	}
	else if( action_it->dist_to_goal < RADIUS_3 )
	{
		area_r = RADIUS_3;
	}
	else
	{
		area_r = 1000; // area infinita
	}
	choosen_action.prob = 0;
	for( action_it = actions.begin();
			action_it != actions.end() && area_changed == false;
			++action_it )
	{
		if( action_it->dist_to_goal <= area_r )
		{
			if( action_it->prob > choosen_action.prob )
				choosen_action = *action_it;
		}
		else
			area_changed = true;
	}

	switch( choosen_action.action_type )
	{
	case RUN:
		runWithBall( choosen_action.target.x,
				     choosen_action.target.y,
			         world->bitacoraBalon.begin()->pos.x,
			         world->bitacoraBalon.begin()->pos.y,
				     world->bitacoraBalon.begin()->dis,
				     agent_vel,
				     1.0,
				     world,
				     command );
		break;
	case PASS:
		passAction( choosen_action.unum, choosen_action.target );
		break;
	case SHOOT:
		shootAction();
		break;
	}

}

void PokTaPokAgent::balonEquipo()
{

#ifdef ATTACK_POSITIONING
				this->attackPositioning();
#else
				voronoiPositioning();
#endif
}

void PokTaPokAgent::balonRival()
{
	Vector2D ball_vel = world->estBallVelocity();

	if( IamTheClosest()  )
	{
		if( ball_vel.mag() < 0.5 )
			irAlBalon();
		else
			ball_interception->call( command );
	}
	else
	{
		defencePositioning();
	}
}


void PokTaPokAgent::balonSuelto()
{
	Vector2D ball_vel = world->estBallVelocity();

	if( IamTheClosest() )
	{
		if( ball_vel.mag() < 0.5 )
			irAlBalon();
		else
			ball_interception->call( command );
	}
	else
	{
		defencePositioning();
	}
}

void PokTaPokAgent::balonPerdido()
{
	//searchBall(world->bitacoraBalon.begin()->dir,world->me().view_mode_w,command);

	if( world->bitacoraBalon.empty() )
		searchBall(command,world, game_data->sensor_handler.last_sensor_type);
	else if( world->time() - world->bitacoraBalon.begin()->ciclo > 6 )
	{
		searchBall(command,world, game_data->sensor_handler.last_sensor_type);
	}
	else
	{

	}

}

void PokTaPokAgent::irAlBalon()
{
	Vector2D velocidad;
	Vector2D balon;


	double x     = world->me().pos.x;
	double y     = world->me().pos.y;
	double angle = world->me().angle;

	double  neck_dir = game_data->sensor_handler.last_sense.head_angle;

	velocidad.x = game_data->sensor_handler.last_sense.speed_amount;    // dis
	velocidad.y = game_data->sensor_handler.last_sense.speed_direction; // dir



	if( game_data->sensor_handler.last_see.ballIsVisible())
	{
		Ball & ball         = sensor_h->last_see.ball;
		balon = Vector2D::fromPolar(ball.dis,Deg2Rad(ball.dir + angle + neck_dir));
		balon.x = balon.x + x;
		balon.y = balon.y + y;
		GoToXY(balon.x,balon.y,x,y,angle,neck_dir,1.0,velocidad,command);
	}
	else
	{
      if( (world->time() - world->bitacoraBalon.begin()->ciclo) < POT_FIELD_EXPIRE_TIME )
      {
  		GoToXY(world->bitacoraBalon.begin()->pos.x,
  			   world->bitacoraBalon.begin()->pos.y,
  			   x,y,angle,
  			   neck_dir,
  			   1.0,
  			   velocidad,
  			   command);
      }
      else
      {
       searchBall(command,world, game_data->sensor_handler.last_sensor_type);
      }
	}
}


void PokTaPokAgent::voronoiPositioning()
{

	BodySensor & body = sensor_h->last_sense;

	Vector2D agent_vel = Vector2D::fromPolar( body.speed_amount,
			Deg2Rad( body.speed_direction + world->me().angleDeg() + world->me().headAngleDeg() ) );

	this->constructVoronoi();

	GoToXY( last_voronoi_centroid.x, last_voronoi_centroid.y,
			world->me().pos.x, world->me().pos.y,
			world->me().angleDeg(),
			world->me().headAngleDeg(),
			2.0,	// Usamos un radio grande para disminuir la cantidad de movimientos debido al alto dinamismo del entorno
			agent_vel,
			command );


}

void
PokTaPokAgent::defencePositioning()
{
	BodySensor & body = game_data->sensor_handler.last_sense;

	Vector2D b; // Posición del balón
	Vector2D q; // Posición del origen del movimiento
	Vector2D p; // Nuestra posición

	std::deque<ActionOption>  posible_actions;
	std::deque<ActionOption>  posible_actions_in_cell;
	std::vector<Vector2D>	  obstruct_points;

	ActionOption tmp_action;
	ActionOption choosen_action;

	std::deque<ActionOption>::const_iterator action_it;

	int q_unum; // número del jugador más cercano al balón

	// Para predecir la dirección en que correrá el enemigo
	Vector2D tmp_v;
	Vector2D resultant;
	Vector2D g;
	Vector2D goal;
	double d;
	double const INFO_EXPIRE_TIME	= 5;
	double const RUN_MAGNITUDE 		= 2.5;
	double const V 					= 0.94;
	double const BALL_FINAL_VEL 	= 1.5;
	double const BALL_RUN_FINAL_VEL = 1.0;
	double const k					= 5.0;
	double const RADIUS_1 			= 10.0;
	double const RADIUS_2 			= 25.0;
	double const RADIUS_3 			= 45.0;

	std::deque<ObjetoBitacora>::const_iterator opp_it;
	std::deque<ObjetoBitacora>::const_iterator team_it;

	Vector2D voronoi_cell;

	double p_min;
	double pi;
	double di;
	double Di;
	double r;
	double n;
	int i, j;

	double area_r;
	bool   area_changed;

	Vector2D agent_vel = Vector2D::fromPolar( body.speed_amount,
			Deg2Rad( body.speed_direction + world->me().angleDeg() + world->me().headAngleDeg() ) );


	if( world->bitacoraBalon.empty() )
	{
		searchBall( command, world, game_data->sensor_handler.last_sensor_type );
		return;
	}
	if( (world->time() - world->bitacoraBalon.begin()->ciclo) >= INFO_EXPIRE_TIME )
	{
		searchBall( command, world, game_data->sensor_handler.last_sensor_type );
		return;
	}

	this->constructVoronoi();


	b = world->bitacoraBalon.begin()->pos;
	p = world->me().pos;


	// Identificamos al agente rival más cercano al balón
	q 		= 1000;
	q_unum  = 0;
	j 		= 0;
	for( i=0; i < 11; i++ )
	{
		opp_it = world->bitacoraRivales[i].begin();
		if( !world->bitacoraRivales[i].empty() )
		if(    world->time() - opp_it->ciclo < INFO_EXPIRE_TIME )
		{
			if( (opp_it->pos - b).mag() < (q-b).mag() )
			{
				q = opp_it->pos;
				q_unum = opp_it->num;
			}
			j++;
		}
	}

	if ( j == 0 )
	{
		voronoiPositioning();
		return; // no datos de rivales
	}

	// Actualizamos el campo potencial y obtenemos la resultante para predecir la dirección en que
	// correría el agente rival
	if( world->me().side == 'l' )
		goal = goal_l; // el objetivo es nuestra meta
	else
		goal = goal_r;

	g =  goal - b; // El vector hacia nuestra meta

	g.normalize();

	resultant 	=  g;

	for( i=0; i < 11; i++ )
	{
		team_it = world->bitacoraAmigos[i].begin();
		if( !world->bitacoraAmigos[i].empty() )
		if( world->time() - team_it->ciclo < INFO_EXPIRE_TIME
			&& i != world->me().unum-1)
		{ // la información aún es útil

			tmp_v 	= q - team_it->pos ;
			d 		= tmp_v.mag();

			if( d != 0 )
				tmp_v  *= (k/ (d*d) );

			resultant += tmp_v;
		}
	}
	resultant.normalize();
	resultant *= RUN_MAGNITUDE;
	tmp_action.action_type = RUN;
	tmp_action.target = resultant + b;
	tmp_action.dist_to_goal = (goal - tmp_action.target).mag();
	// Obtenemos la probabilidad de correr, se obtiene diferente a las demás, ya que
	// la velocidad final es menor a la del pase
	p_min = 1;
	r = (q - tmp_action.target).mag(); // Calculamos la distancia hacia el punto al que correría
	n =   std::log( 1 - r*(1-1/0.94)/BALL_RUN_FINAL_VEL ) // Calculamos el tiempo que tarda en llegar el balón
	/ std::log( 1/0.94 );

	if( n == 0  || isnan(n) ) n = 1;

	n = std::ceil(n) + 2; // Se le aumentan 2 porque es el tiempo que tardamos llegar al balón otra vez
	for( j=0; j<11; j++ )
	{
		team_it = world->bitacoraAmigos[j].begin();
		if( !world->bitacoraAmigos[j].empty() )
		if( world->time() - opp_it->ciclo < PASS_OPTION_EXPIRE_TIME )
		{
			di = (tmp_action.target-team_it->pos).mag();
			Di = di/n;
			pi = 1 - V/(V+Di); // probabilidad de que el pase/movimiento sea exitoso
			if( pi < p_min )
				p_min = pi;
		}
	}
	tmp_action.prob = p_min;//*RUN_FACTOR; // Añadido RUN_FACTOR para darle preferencia a los pases y al tiro

	posible_actions.push_back( tmp_action );

	// Obtenemos todas sus opciones de pase
	//std::cout << "mis coord: " << world->me().pos << std::endl;
	for( i=0; i<11; i++ )
	{
		opp_it = world->bitacoraRivales[i].begin();
		p_min = 1;
		if( ! world->bitacoraRivales[i].empty() )
		if( world->time() - opp_it->ciclo < INFO_EXPIRE_TIME // info reciente
				//&&  i != (q_unum -1) // No consideramos al que tiene el balón
				//&&  i != 0	// No consideramos al portero para los pases
		)
		{

			tmp_action.action_type  = PASS;
			tmp_action.target 		= opp_it->pos;
			//std::cout << "opp pos " <<  opp_it->pos << std::endl;
			tmp_action.dist_to_goal = ( tmp_action.target - goal ).mag();

			r  = (p-tmp_action.target).mag(); // Calculamos la distancia hacia el receptor
			n  =    std::log( 1 - r*(1-1/0.94)/BALL_FINAL_VEL ) // Calculamos el tiempo que tarda en llegar al receptor
			/ std::log( 1/0.94 );

			n  = std::ceil(n);
			if( n == 0 || isnan(n) ) n = 1;
			for( j=0; j<11; j++ ) // Calculamos la probabilidad de éxito con cada uno de los rivales y tomamos la mínima
			{
				team_it = world->bitacoraAmigos[j].begin();
				if( ! world->bitacoraAmigos[j].empty() )
				if( world->time() - team_it->ciclo < INFO_EXPIRE_TIME )
				{
					di = ( tmp_action.target - team_it->pos ).mag();
					Di = di/n;
					pi = 1 - V/(V+Di);

					if( pi < p_min )	p_min = pi;
				}
			}
			tmp_action.prob = p_min;
			posible_actions.push_back( tmp_action );
		}
	}

	if( (b-goal).mag() < 25 )
	{
		// añadimos el tiro a gol
		tmp_action.action_type = SHOOT;
		tmp_action.dist_to_goal = 0.0;
		tmp_action.target = goal;

		p_min = 1;
		r  = (p-tmp_action.target).mag(); // Calculamos la distancia hacia el receptor
		n  =    std::log( 1 - r*(1-1/0.94)/BALL_FINAL_VEL ) // Calculamos el tiempo que tarda en llegar al receptor
		/ std::log( 1/0.94 );
		n  = std::ceil(n);
		if( n <= 0 )
		{
			n = 1; // no se puede llegar en cero ciclos
		}
		for( j=0; j<11; j++ ) // Calculamos la probabilidad de éxito con cada uno de los rivales y tomamos la mínima
		{
			team_it = world->bitacoraAmigos[j].begin();
			if( ! world->bitacoraAmigos[j].empty() )
			if( world->time() - team_it->ciclo < INFO_EXPIRE_TIME )
			{
				di = ( tmp_action.target - team_it->pos ).mag();
				Di = di/n;

				pi = 1 - V/(V+Di);

				if( pi < p_min )	p_min = pi;
			}
		}
		tmp_action.prob = p_min;
		posible_actions.push_back( tmp_action );
	}


	// Guardamos todas las acciones que están dentro de nuestra celda
	Point2D target_point;
	for( 	action_it = posible_actions.begin();
			action_it != posible_actions.end();
			++action_it )
	{
		target_point.set( action_it->target.x, action_it->target.y );
		if( insideConvexPolygon( target_point, this->voronoi_cell) )
			posible_actions_in_cell.push_back( *action_it );
	}

	std::sort( posible_actions_in_cell.begin(),
			   posible_actions_in_cell.end(),
			   ActionOption::distanceLowerThan );


	if( posible_actions_in_cell.empty() )
	{
		GoToXY( last_voronoi_centroid.x, last_voronoi_centroid.y,
				    world->me().pos.x, world->me().pos.y,
				    world->me().angleDeg(),
				    world->me().headAngleDeg(),
				    2.0,
				    agent_vel,
				    command );
		return;
	}

	action_it = posible_actions_in_cell.begin();

	if( action_it->dist_to_goal == 0 )
	{
		area_r = 0.0;
	}
	if( action_it->dist_to_goal < RADIUS_1 )
	{
		area_r = RADIUS_1;
	}
	else if( action_it->dist_to_goal < RADIUS_2 )
	{
		area_r = RADIUS_2;
	}
	else if( action_it->dist_to_goal < RADIUS_3 )
	{
		area_r = RADIUS_3;
	}
	else
	{
		area_r = 1000; // area infinita
	}

	choosen_action.prob = 0;
	area_changed = false;
	for( 	action_it = posible_actions_in_cell.begin();
			action_it != posible_actions_in_cell.end() && area_changed == false;
			++action_it )
	{
		if( action_it->dist_to_goal <= area_r )
		{
			if( action_it->prob > choosen_action.prob )
				choosen_action = *action_it;
		}
		else
			area_changed = true;
	}


	// Calculamos al punto al que tendríamos que ir para obstruir el movimiento
	Vector2D bm = choosen_action.target - b; // Vector del balón hacia el resultado del movimiento
	Vector2D bp = world->me().pos - b; // Vector del balón hacia mi posición
	double   bm_mag = bm.mag();
	double	 proy_mag;
	Vector2D proy;

	if( bm_mag != 0.0 )
	{
		proy_mag = bp.prodPunto( bm ) / bm_mag ;

		if( proy_mag < 0 ) // Magnitud negativa, el punto más cercano es b.
		{
			tmp_v = b;
		}
		else if( proy_mag >= bm.mag() )
		{ // La magnitud es más grande que la que hay del balón  al destino, el punto más cercano es el destino.
			tmp_v = choosen_action.target;
		}
		else // la proyección se encuentra dentro de v
		{
			proy = bm * (proy_mag / bm_mag );
			tmp_v = b + proy;
		}
		// tmp_v es ahora el punto de obstrucción más cercano
	}
	else
		tmp_v = b;
	if( posible_actions_in_cell.size() <= 1 )
	{
		GoToXY( tmp_v.x, tmp_v.y,
				world->me().pos.x, world->me().pos.y,
				world->me().angleDeg(),
				world->me().headAngleDeg(),
				2.0,
				agent_vel,
				command );
		return;
	}

	//std::cout << "choosen " << choosen_action.action_type << " " << choosen_action.target << " proy_p " << tmp_v << std::endl;

	//Verificamos si algún compañero está más cerca
	bool someone_is_closer = false;
	double dist_to_point = (world->me().pos - tmp_v).mag();
	for( i=0; i<11; i++ )
	{
		team_it = world->bitacoraAmigos[i].begin();
		if( !  world->bitacoraAmigos[i].empty() )
		if(    world->time() - team_it->ciclo < INFO_EXPIRE_TIME
			&& i != world->me().unum -1
			&& someone_is_closer == false )
		{
			if( (team_it->pos - tmp_v).mag() < dist_to_point )
				someone_is_closer = true;
		}
	}

	if( someone_is_closer && posible_actions_in_cell.size() >= 2 )
	{
		choosen_action.prob = 0;
		area_changed = false;
		posible_actions.pop_front(); // descartamos nuestra anterior acción
		if( action_it->dist_to_goal == 0 )
			area_r = 0.0;
		if( action_it->dist_to_goal < RADIUS_1 )
			area_r = RADIUS_1;
		else if( action_it->dist_to_goal < RADIUS_2 )
			area_r = RADIUS_2;
		else if( action_it->dist_to_goal < RADIUS_3 )
			area_r = RADIUS_3;
		else
			area_r = 1000; // area infinita
		for( 	action_it = posible_actions_in_cell.begin();
				action_it != posible_actions_in_cell.end() && area_changed == false;
				++action_it )
		{
			if( action_it->dist_to_goal <= area_r )
			{
				if( action_it->prob > choosen_action.prob )
					choosen_action = *action_it;
			}
			else
				area_changed = true;
		}
		bm = choosen_action.target - b;; // Vector del balón hacia el resultado del movimiento
		bp = world->me().pos - b; // Vector del balón hacia mi posición
		bm_mag = bm.mag();
		if( bm_mag != 0 )
		{
			proy_mag = bp.prodPunto( bm ) / bm_mag ;
			if( proy_mag < 0 ) // la magnitud es negativa, entonces el punto más cercano es b
			{
				tmp_v = b;
			}
			else if( proy_mag >= bm.mag() ) // La magnitud es más grande que la que hay a del balón destino, el punto más cercano es el destino
			{
				tmp_v = choosen_action.target;
			}
			else // la proyección se encuentra dentro de v
			{
				proy = bm * (proy_mag / bm_mag );
				tmp_v = b + proy;
			}
		}
		else
			tmp_v = b;
	} // someone is closer


	GoToXY( tmp_v.x, tmp_v.y,
			p.x, p.y,
			world->me().angleDeg(), world->me().headAngleDeg(),
			1.0,
			agent_vel,
			command);

}	// termina defensa

void PokTaPokAgent::goalieBehavior()
{

    int num,timeCatch;
    static bool atrapada,free_kick,goal_kick,varCatch;
    double  xTarget;
    Vector2D kick;
    Vector2D velocidad;

    if( world->me().side == 'l' )
        xTarget = -48.0;
    else
        xTarget = 48.0;

    velocidad.x = game_data->sensor_handler.last_sense.speed_amount;    // dis
    velocidad.y = game_data->sensor_handler.last_sense.speed_direction; // dir

    switch( game_data->sensor_handler.last_hear_referee.play_mode )
    {
    case GOALIE_CATCH_BALL_L : if(world->me().side == 'l') atrapada=true;
        break;
    case GOALIE_CATCH_BALL_R : if(world->me().side == 'r') atrapada=true;
        break;
    case FREE_KICK_L:  if( world->me().side == 'l') free_kick = true;
        break;
    case FREE_KICK_R:  if( world->me().side == 'r') free_kick = true;
        break;
    case  GOAL_KICK_L: if(world->me().side == 'l') goal_kick = true;
        break;
    case  GOAL_KICK_R: if(world->me().side == 'r') goal_kick = true;
        break;
    default:
    	break;
    }

    if( (world->time() - world->bitacoraBalon.begin()->ciclo ) < 5 )
    {
        if(  balonEnAreaGrande()  &&   // el balon se encuentra dentro del area grande propia?
             IamTheClosest()    )        // y soy el más cercano?
        {
            if( game_data->sensor_handler.last_see.ballIsVisible() &&
                game_data->sensor_handler.last_see.ball.dis < 1.2  )
            {   // Puedo atrapar la pelota
                if(atrapada!= true)  // si no la he atrapado hago catch()
                {
                    command->append_catch( world->bitacoraBalon.begin()->dir );
                    varCatch = true;
                    timeCatch = world->time();
                }

                if( (free_kick == true && atrapada == true && varCatch ==true )
                      || goal_kick == true ) // Atrape el balon, o es saque de meta, y puedo pasar
                {
                    num = aQuienPasar();

                    if(num == -1)   // si regresa -1 no ve ningun agente
                    {
                        //  Pase al centro del campo
                        kick = PasePunto( 0.0,
                                          0.0,
                                          world->me().pos.x,
                                          world->me().pos.y,
                                          world->me().angle,
                                          3.5,
                                          world->bitacoraBalon.begin()->pos.x,
                                          world->bitacoraBalon.begin()->pos.y );

                        command->append_kick(kick.x,kick.y);

                    }
                    else
                    {	// Pase a un amigo
                        kick = PasePunto( world->bitacoraAmigos[num].begin()->pos.x,
                                          world->bitacoraAmigos[num].begin()->pos.y,
                                          world->me().pos.x,
                                          world->me().pos.y,
                                          world->me().angle,
                                          2.0,
                                          world->bitacoraBalon.begin()->pos.x,
                                          world->bitacoraBalon.begin()->pos.y );

                        command->append_kick(kick.x,kick.y);
                    }
                }
                else
                {
                    if( varCatch == true  &&
                            atrapada == false &&
                            (world->time() - timeCatch) > 1  )  // se realizo catch y no se atrapo el balón
                    {
                        kick = PasePunto( 0.0,
                                          0.0,
                                          world->me().pos.x,
                                          world->me().pos.y,
                                          world->me().angle,
                                          3.5,
                                          world->bitacoraBalon.begin()->pos.x,
                                          world->bitacoraBalon.begin()->pos.y );

                        command->append_kick(kick.x,kick.y);
                    }
                }
            }  // balon en el area pero lejos del portero
            else
            {
                atrapada = false;
                free_kick = false;
                goal_kick = false;
                varCatch = false;
                alignBodyWithNeck(world,command);

                ball_interception->call( command );
            }
        }
        else
        {   // Balón fuera del área grande

            if( inGoalieLine() )
            {  // estoy en la linea de defensa
                porteroLine(xTarget,
                            velocidad,
                            game_data->sensor_handler.last_sensor_type,
                            world,command);
            }
            else
            {  // Regreso al punto clave
                GoToXY( xTarget,
                        0.0,     //yTarget = 0.0
                        1.0,
                        velocidad,
                        command,
                        world);
            }
        }
    }
    else    // No tengo información reciente del balón, busco el balón
    {
        searchBall(command, world, game_data->sensor_handler.last_sensor_type);
    }
} // Termina el comportamiento del portero

void
PokTaPokAgent::attackPositioning()
{
	BodySensor & body = game_data->sensor_handler.last_sense;

	std::vector<Point2D> sites;
	std::vector<Point2D> cell_container;

	std::deque<ObjetoBitacora>::const_iterator teammate_it;
	std::deque<ObjetoBitacora>::const_iterator opp_it;
	std::vector<Point2D>::const_iterator 	   cell_it;

	Vector2D centroid;


	int    const BALL_INFO_EXPIRE_TIME = 6;
	double const CENTROID_THRESHOLD_RADIUS = 9.0;
	double const OPP_THRESHOLD_RADIUS = 9.0;
	double d;
	Vector2D centroid_attractor;
	Vector2D resultant;

	int 	 i;
	Vector2D tmp_v;
	Point2D  tmp_p;

	Vector2D agent_vel = Vector2D::fromPolar( body.speed_amount,
			Deg2Rad( body.speed_direction + world->me().angleDeg() + world->me().headAngleDeg() ) );

	Vector2D b;

	if( world->bitacoraBalon.empty() )
	{
		searchBall( command, world, game_data->sensor_handler.last_sensor_type );
		return;
	}
	if( world->time() - world->bitacoraBalon.begin()->ciclo >= BALL_INFO_EXPIRE_TIME )
	{
		searchBall( command, world, game_data->sensor_handler.last_sensor_type );
		return;
	}


	this->constructVoronoi();

	centroid = this->last_voronoi_centroid;

	b = world->bitacoraBalon.begin()->pos;

	// contruimos los repulsores
	centroid_attractor = centroid - world->me().pos; //Vector de nosotros al centroide
	d = centroid_attractor.mag();

	resultant = 0.0;

	if( d != 0 )
	{
		centroid_attractor /= d; // normalizamos
		centroid_attractor *= ( (d*d) / CENTROID_THRESHOLD_RADIUS ); // asignamos magnitud al vector
	}

	resultant += centroid_attractor;

	for( i=0; i<11; i++ )
	{
		opp_it = world->bitacoraRivales[i].begin();
		if( ! world->bitacoraRivales[i].empty() )
			if( world->time() - opp_it->ciclo < 10 )
			{
				tmp_v = world->me().pos - opp_it->pos;
				d = tmp_v.mag();
				if( d != 0 )
				{
					tmp_v /= d; // normalizamos
					tmp_v *= (OPP_THRESHOLD_RADIUS / (d) ); // asignamos magnitud al vector
				}
				resultant += tmp_v;
			}
	}

	// construimos el atractor hacia el balón
	tmp_v 	= b - world->me().pos;
	d 		= tmp_v.mag();
	if( d != 0 )
	{
		tmp_v	/= d;	// normalizamos
		tmp_v 	*= (CENTROID_THRESHOLD_RADIUS / (d*d) ); // aplicamos magnitud
	}

	resultant += tmp_v;

	// Vamos en dirección a la resultante
	// Para adaptarla a la habilidad de correr a un punto, calculamos un punto: hacemos grande el vector resultante
	// y lo sumamos a nuestra posición
	tmp_v = world->me().pos + resultant*10;

	//std::cout << "atk resultant " << resultant << std::endl;

	GoToXY( tmp_v.x, tmp_v.y,
			world->me().pos.x, world->me().pos.y,
			world->me().angleDeg(),
			world->me().headAngleDeg(),
			1.0,
			agent_vel,
			command);

}

void
PokTaPokAgent::constructVoronoi()
{
	// Para la celda inicial
	std::vector<Point2D> initial_cell;
	Point2D q;
	double const INITIAL_CELL_WIDTH  = 50;
	double const INITIAL_CELL_HWIDTH = 25;
	double const MARGIN = 5.0;
	double const VORONOI_EXPIRE_TIME = 10;

	static int   last_construct_time = -1;

	double  y_top	 = 30.0;
	double  y_bottom = -30.0;
	double  x_left;
	double 	x_right;
	Vector2D b;
	Vector2D b_vel;
	// Para la construcción de la celda voronoi
	Point2D p ( world->me().pos.x, world->me().pos.y );
	std::vector<Point2D> sites;
	std::vector<Point2D> cell_container;
	std::deque<ObjetoBitacora>::const_iterator teammate_it;
	std::vector<Point2D>::const_iterator 	   cell_it;

	Vector2D centroid;

	// general stuff
	int i, n;
	Vector2D tmp_v;
	Point2D  tmp_p;


	if( world->bitacoraBalon.empty() )
		return;

	if( world->time() - last_construct_time < VORONOI_EXPIRE_TIME )
		return;

	b = world->bitacoraBalon.begin()->pos;

	if( b.x - INITIAL_CELL_HWIDTH < -52.5 )
	{
		x_left  = -52.5;
		x_right = -52.5 + INITIAL_CELL_WIDTH;
	}
	else if( b.x + INITIAL_CELL_HWIDTH > 52.5 )
	{
		x_left  = 52.5 - INITIAL_CELL_WIDTH;
		x_right = 52.5;
	}
	else
	{
		x_left  = b.x - INITIAL_CELL_HWIDTH;
		x_right = b.x + INITIAL_CELL_HWIDTH;
	}
	// en caso de que estemos fuera del campo
	if( p.y() > y_top )
		y_top = world->me().pos.y + MARGIN;

	if( p.y() < y_bottom )
		y_bottom = world->me().pos.y - MARGIN;

	if( p.x() < x_left )
		x_left = world->me().pos.x - MARGIN;

	if( p.x() > x_right )
		x_right = world->me().pos.x + MARGIN;

	q.set( x_left,  y_bottom ); initial_cell.push_back( q );
	q.set( x_left,  y_top	 ); initial_cell.push_back( q );
	q.set( x_right, y_top 	 ); initial_cell.push_back( q );
	q.set( x_right, y_bottom ); initial_cell.push_back( q );
	q.set( x_left,  y_bottom ); initial_cell.push_back( q );

	for( i=0; i<11; i++ ) // Para cada teammate
	{
		teammate_it = world->bitacoraAmigos[i].begin();
		if( world->bitacoraAmigos[i].size() > 0 )
			if( world->time() - teammate_it->ciclo < VORONOI_EXPIRE_TIME
					&&  i != (world->me().unum -1) // No nos consideramos a nosotros mismos
					&&  i != 0	// No consideramos al portero
			)
			{ // la información aún es útil
				tmp_p.set( teammate_it->pos.x, teammate_it->pos.y );
				sites.push_back( tmp_p );
			}
	}

	constructVoronoiCell( p, sites, initial_cell, &cell_container );

	if( !cell_container.empty() )
	{
		// Voronoi trabaja usando la celda final igual a la primera, descartamos
		// cualquiera de las dos para calcular el centroide
		voronoi_cell = cell_container;
		cell_container.pop_back();
		n = cell_container.size();
		centroid = 0.0;
		for( cell_it = cell_container.begin();
				cell_it != cell_container.end();
				++cell_it	)
		{
			centroid.x += cell_it->x();
			centroid.y += cell_it->y();
		}
		if( n > 0 )
		{
			centroid /= (double)n;
			last_voronoi_centroid = centroid;
		}
	}
	else
	{
		centroid = last_voronoi_centroid;
	}
}


double
PokTaPokAgent::passProb( Vector2D const & target_pos)
{
	double prob = 0;

	return prob;
}

void
PokTaPokAgent::passAction( int unum, Vector2D const & pos )
{
	Vector2D kick_vec;
	char msg[10];

	sprintf( msg,
			"p%1x%+03d%+03d",
			unum, (int)pos.x, (int)pos.y );

	kick_vec =
			PasePunto( pos.x,
					   pos.y,
					world->me().pos.x,
					world->me().pos.y,
					world->me().angleDeg(),
					1.2,     // velocidad final
					world->bitacoraBalon.begin()->pos.x,
					world->bitacoraBalon.begin()->pos.y);


	command->append_say( msg );
	command->append_kick( kick_vec.x, kick_vec.y );

}

void
PokTaPokAgent::shootAction()
{
	double const GOALIE_INFO_EXPIRE_TIME = 6;

	vector<Player>::const_iterator player_it;
	vector<ObjetoBitacora>::const_iterator opp_it;

	Vector2D post_top	( 52.5, -5);
	Vector2D post_bottom( 52.5, 5);

	Vector2D target;

	Vector2D closest_point_top;
	Vector2D closest_point_bot;
	double 	 dist_top;
	double 	 dist_bottom;

	Vector2D goalie_pos;

	Vector2D ball_pos = world->bitacoraBalon.begin()->pos;

	Vector2D kick_vector;

	bool 	 there_is_goalie;

	if( world->me().side == 'r' )
	{
		post_top.x 	  = -post_top.x;
		post_bottom.x = -post_bottom.x;
	}

	// buscamos al portero
	there_is_goalie = false;
	for( int i=0; i<11; ++i )
		if( world->bitacoraRivales[i].empty() == false )
			if( world->bitacoraRivales[i].begin()->goalie
				&& world->time() - world->bitacoraRivales[i].begin()->ciclo < GOALIE_INFO_EXPIRE_TIME
				)
			{
				there_is_goalie = true;
				goalie_pos = world->bitacoraRivales[i].begin()-> pos;
			}


	if( there_is_goalie )
	{
		closest_point_top = closestPointInSegment( goalie_pos, ball_pos, post_top );
		closest_point_bot = closestPointInSegment( goalie_pos, ball_pos, post_bottom);
		dist_top    = (closest_point_top - goalie_pos).mag();
		dist_bottom = (closest_point_bot - goalie_pos).mag();

		if( dist_top > dist_bottom )
			target = post_top;
		else
			target = post_bottom;
	}
	else
	{
		if(   (world->me().pos - post_top).mag()
			< (world->me().pos - post_bottom).mag() )
			target = post_top;
		else
			target = post_bottom;
	}

	kick_vector =
			PasePunto( target.x,
					target.y,
					world->me().pos.x,
					world->me().pos.y,
					world->me().angleDeg(),
					3.0,     // velocidad final
					world->bitacoraBalon.begin()->pos.x,
					world->bitacoraBalon.begin()->pos.y);

	command->append_kick( 100, kick_vector.y );

}

int
PokTaPokAgent::aQuienPasar()
{
    int i;
    int indice;
    double min = 9999.9999;

    indice = -1;

    for(i=0;i<11;i++)
    {
        if( world->bitacoraAmigos[i].size()!=0 &&
           (world->time() - world->bitacoraAmigos[i].begin()->ciclo) < 5 &&
           (world->me().unum - 1) != i
          )
        {
            if( world->bitacoraAmigos[i].begin()->dis < min )
            {
                //cout<<"distancia amigo "<<i<<": "<<world->bitacoraAmigos[i].begin()->dis<<endl;
                indice = i;
            }
        }
    }

    return indice;
}

bool
PokTaPokAgent::isZoneShoot( double x, double y , double radio)
{
	double aux;

	aux = sqrt( (x - 52.0)*(x - 52.0) + y*y );

	if( aux <= radio )
		return true;
	else
		return false;
}

PossessionBall
PokTaPokAgent::whoHasTheBall( )
{
    int            i;
    bool           posesion=false;
    double         disToBall;
    double         disPermisible;

    Vector2D       poseAgente,balon;

    PossessionBall regreso;

    disPermisible = 2.0;

    if( world->bitacoraBalon.size() != 0         &&
            (world->time() - world->bitacoraBalon.begin()->ciclo) <= 5 )
    {
        balon.x = world->bitacoraBalon.begin()->pos.x;
        balon.y = world->bitacoraBalon.begin()->pos.y;

        if(  world->bitacoraBalon.begin()->dis <= disPermisible )
        {
            regreso = PROPIA;
            posesion = true;
            //cout<<world->time()<<" PROPIA"<<endl;
        }

        //pruebo para los amigos
        for( i=0 ; i<11 && posesion == false ; i++ )
        {
            if( world->bitacoraAmigos[i].size()!= 0 )
            {
                poseAgente = world->bitacoraAmigos[i].begin()->pos;

                disToBall = (balon - poseAgente).mag();
                if( disToBall <= disPermisible )
                {
                    regreso = EQUIPO;
                    posesion = true;
                    //cout<<world->time()<<" EQUIPO "<<endl;
                    i=11;
                }
            }
        }
        //pruebo para los rivales
        for( i=0 ; i<11 && posesion == false; i++ )
        {
            if( world->bitacoraRivales[i].size()!= 0)
            {
                poseAgente.x = world->bitacoraRivales[i].begin()->pos.x;
                poseAgente.y = world->bitacoraRivales[i].begin()->pos.y;

                disToBall = sqrt((balon.x-poseAgente.x)*(balon.x-poseAgente.x) +
                                 (balon.y-poseAgente.y)*(balon.y-poseAgente.y)
                                 );

                if( disToBall <= disPermisible )
                {
                    regreso = RIVAL;
                    posesion = true;
                    i=11;
                }
            }
        }

        if( posesion == false )
        {
            regreso = SUELTA;
        }
    }
    else
    {
        regreso = PERDIDA;
    }
    return regreso;
}


bool
PokTaPokAgent::inGoalieLine( )
{
    bool regreso;
    double xTarget;

    regreso = false;

    if( world->me().side == 'l' )
        xTarget = -48.0;
    else
        xTarget = 48.0;

    if(world->me().pos.x < xTarget + 1.0 && world->me().pos.x > xTarget - 1.0)
        regreso = true;

    return regreso;
}

bool
PokTaPokAgent::IamTheClosest(  )

{
    int       i,k,friends;
    bool      regreso;
    double   disToBall;
    Vector2D  balon;
    Vector2D  poseAgente;


    if( world->bitacoraBalon.empty() )
    	return false;

    balon	= world->bitacoraBalon.begin()->pos;

    k=0;
    friends = 0;

    for( i = 0; i<11; i++ )
    {
        if( world->bitacoraAmigos[i].size() != 0 )
        {
            friends++;

            poseAgente.x = world->bitacoraAmigos[i].begin()->pos.x;
            poseAgente.y = world->bitacoraAmigos[i].begin()->pos.y;

            disToBall = (poseAgente - balon).mag();

            if(   world->time() - world->bitacoraAmigos[i].begin()->ciclo >= 10
                || world->bitacoraBalon.begin()->dis 					  < disToBall )
                k++;
        }
    }

    if( friends == 0 || ( friends!=0 && friends==k  )  )
        regreso = true;
    else
        regreso = false;

    return regreso;
}

bool
PokTaPokAgent::balonEnAreaGrande(  )
{
    bool regreso;

    regreso = false;

    if( world->me().side == 'l')
    {
        if(   world->bitacoraBalon.begin()->pos.x < -36 &&    // area grande del portero
              world->bitacoraBalon.begin()->pos.y > -20 &&
              world->bitacoraBalon.begin()->pos.y <  20 &&
              world->bitacoraBalon.begin()->pos.x > -52.5)
        {
            regreso = true;
        }
    }
    else
    {
        if(   world->bitacoraBalon.begin()->pos.x > 36  &&    // area grande del portero
              world->bitacoraBalon.begin()->pos.y > -20 &&
              world->bitacoraBalon.begin()->pos.y <  20 &&
              world->bitacoraBalon.begin()->pos.x < 52.5)
        {
            regreso = true;
        }
    }

    return regreso;
}
