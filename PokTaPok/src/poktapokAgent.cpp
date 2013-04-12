#include "poktapokAgent.h"
#include "formation.h"
#include "analyticalAbility.h"

#include <cmath>
#include <algorithm>

PokTaPokAgentV1::PokTaPokAgentV1( GameData * game_data )
{
    world 			  = new WorldModelV1	 ( game_data );
    freeze_ball	      = new FreezeBall		 ( game_data, world );
    ball_interception = new BallInterception ( game_data, world, freeze_ball );
	this->game_data	  = game_data;
	param			  = & game_data->game_parameter;
	goal_l.set( -50.0, 0.0 );
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

	Vector2D pos;
	switch( world->me.unum )
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
	//if( world->me.side == 'r' )
	//pos.x = -pos.x;
	if( (world->me.pos - pos).normita() > 2.0 )
		command->append_move( pos.x, pos.y );

	if( world->me.synch_see_on == false )
		command->append_synch_see();
	if( world->me.view_mode_w != NARROW )
		command->append_change_view( HIGH, NARROW );


}

void PokTaPokAgentV1::onPrep()
{
	if(    game_data->sensor_handler.last_hear_referee.play_mode != KICK_OFF_L
		&& game_data->sensor_handler.last_hear_referee.play_mode != KICK_OFF_R )
	{
	voronoiPositioning();
	}
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

	if( world->me.unum != 1 )
	{
		switch(world->me.possession_ball)
		{
		case PROPIA: 	  balonPropio(); break; //cout<<"Balon propio"<<endl;		break;        // balón en poder del propio agente
		case EQUIPO:	  balonEquipo();break; //cout<<"Balon con nuestro equipo"<<endl;		break;        // balón en poder del equipo propio
		case RIVAL: 	  balonRival();  break;//cout<<"Balon con el rival"<<endl;		break;        // balón en poder del rival
		case DESCONOCIDA: balonDesconocido();break;//cout<<"Balon con agente desconocido"<<endl;	break;        // balón en poder desconocida
		case SUELTA: 	  balonSuelto(); break;//cout<<"Balon Suelto"<<endl;		break;        // balón suelto
		case PERDIDA: 	  balonPerdido(); break;//  cout<<"No veo el balon"<<endl;	break;        // no se ve el balón
		default:                                break;
		}
	}
	else
	{

	}
}

void PokTaPokAgentV1::balonPropio()
{
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
	double const RUN_FACTOR			= 0.9;

	Vector2D resultant = 0; // vector resultante, hacia donde deberíamso de correr
	Vector2D g 		   = 0;	// vector hacia la meta


	double 		 d;
	bool  		is_zone_shoot;

	// Para obtener las probabilidades
	Vector2D 	 p = world->me.pos;
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
			Deg2Rad( body.speed_direction + world->me.angleDeg() + world->me.headAngleDeg() ) );

	// Actualizamos la meta en base a nuestro lado de la cancha
	if( world->me.side == 'l' )
		goal_pos = goal_r;
	else
		goal_pos = goal_l;


	// Actualizamos el campo potencial y obtenemos la resultante para decidir la dirección de correr
	g =  goal_pos - world->me.pos;
	g /= g.normita(); // aún falta determinar la función para la magnitud de g
	resultant 	=  g;
	for( i=0; i < 11; i++ )
	{
		opponent_it = world->bitacoraRivales[i].begin();
		if( world->time - opponent_it->ciclo < POT_FIELD_EXPIRE_TIME
				&& world->bitacoraRivales[i].size() > 0)
		{ // la información aún es útil

			running_pot_field[ i ] = world->me.pos - opponent_it->pos ;
			d = running_pot_field[i].normita();
			running_pot_field[i] *= (k/ (d*d) );
			//std::cout << "added repulsor " << running_pot_field[i] << std::endl;
			resultant += running_pot_field[i];
			running_pot_field_on[i] = true;
		}
		else // La información es muy vieja y ya no nos sirve para decidir
			running_pot_field_on[i] = false;
	}

	run_action.target 		= world->me.pos + resultant*2.5; // 2.5 es la distancia que adelanta el balón
	run_action.action_type  = RUN;
	run_action.dist_to_goal = (goal_pos - run_action.target).normita();

	// Obtenemos la probabilidad de correr, se obtiene diferente a las demás, ya que
	// la velocidad final es menor a la del pase
	p_min = 1;
	r = (world->me.pos - run_action.target).normita(); // Calculamos la distancia hacia el punto en que corremos
	n =   std::log( 1 - r*(1-1/0.94)/BALL_RUN_FINAL_VEL ) // Calculamos el tiempo que tarda en llegar el balón
	/ std::log( 1/0.94 );
	n = std::ceil(n) + 2; // Se le aumentan 2 porque es el tiempo que tardamos llegar al balón otra vez
	for( j=0; j<11; j++ )
	{
		opponent_it = world->bitacoraRivales[j].begin();
		if( world->time - opponent_it->ciclo < PASS_OPTION_EXPIRE_TIME )
		{
			di = (run_action.target-opponent_it->pos).normita();
			Di = di/n;
			pi = 1 - V/(V+Di); // probabilidad de que el pase/movimiento sea exitoso
			if( pi < p_min )
				p_min = pi;
		}
	}
	run_action.prob = p_min*RUN_FACTOR; // Añadido RUN_FACTOR para darle preferencia a los pases y al tiro


	// Añadimos la acción al conjunto de acciones posibles
	actions.push_back( run_action );


	for( i=0; i<11; i++ ) // Para cada teammate
	{
		teammate_it = world->bitacoraAmigos[i].begin();
		p_min = 1;
		if( world->time - teammate_it->ciclo < PASS_OPTION_EXPIRE_TIME
				&&  i != (world->me.unum -1) // No nos consideramos a nosotros mismos
				&&  i != 0	// No consideramos al portero para los pases
				&& world->bitacoraAmigos[i].size() > 0
		)
		{ // la información aún es útil

			tmp_action.action_type = PASS;

			tmp_action.target 		= teammate_it->pos;

			tmp_action.dist_to_goal = ( tmp_action.target - goal_pos ).normita();

			r  = (p-tmp_action.target).normita(); // Calculamos la distancia hacia el receptor

			n  =    std::log( 1 - r*(1-1/0.94)/BALL_FINAL_VEL ) // Calculamos el tiempo que tarda en llegar al receptor
			/ std::log( 1/0.94 );

			n  = std::ceil(n);


			for( j=0; j<11; j++ ) // Calculamos la probabilidad de éxito con cada uno de los rivales y tomamos la mínicma
			{
				opponent_it = world->bitacoraRivales[j].begin();
				if( world->time - opponent_it->ciclo < PASS_OPTION_EXPIRE_TIME
						&& world->bitacoraRivales[j].size() > 0 )
				{
					di = ( tmp_action.target - opponent_it->pos ).normita();
					Di = di/n;
					pi = 1 - V/(V+Di);

					if( pi < p_min )	p_min = pi;
				}
			}

			tmp_action.prob = p_min;

			actions.push_back( tmp_action );
		}

	}

	// Calculamos para la acción shoot
	p = world->me.pos;
	Vector2D shoot_target_up  ( 52.5, -7 );
	Vector2D shoot_target_down( 52.5, 7  );
	if( world->me.side == 'r' )
	{
		p.x = -p.x;
		shoot_target_up.x   = -shoot_target_up.x;
		shoot_target_down.x = -shoot_target_up.x;
	}

	is_zone_shoot = isZoneShoot( p.x, p.y, 20.0 );



	p_min = 1;
	if( is_zone_shoot )
	{
		tmp_action.action_type  = SHOOT;
		tmp_action.target 		= shoot_target_up;
		tmp_action.dist_to_goal = 0.0; //

		r  = (p-tmp_action.target).normita(); // Calculamos la distancia hacia el receptor

		n  =    std::log( 1 - r*(1-1/0.94)/BALL_FINAL_VEL ) // Calculamos el tiempo que tarda en llegar al receptor
		/ std::log( 1/0.94 );

		n  = std::ceil(n);


		for( j=0; j<11; j++ ) // Calculamos la probabilidad de éxito con cada uno de los rivales y tomamos la mínicma
		{
			opponent_it = world->bitacoraRivales[j].begin();
			if( world->time - opponent_it->ciclo < PASS_OPTION_EXPIRE_TIME
					&& world->bitacoraRivales[j].size() > 0 )
			{
				di = ( tmp_action.target - opponent_it->pos ).normita();
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

		r  = (p-tmp_action.target).normita(); // Calculamos la distancia hacia el receptor

		n  =    std::log( 1 - r*(1-1/0.94)/BALL_FINAL_VEL ) // Calculamos el tiempo que tarda en llegar al receptor
		/ std::log( 1/0.94 );

		n  = std::ceil(n);


		for( j=0; j<11; j++ ) // Calculamos la probabilidad de éxito con cada uno de los rivales y tomamos la mínicma
		{
			opponent_it = world->bitacoraRivales[j].begin();
			if( world->time - opponent_it->ciclo < PASS_OPTION_EXPIRE_TIME
					&& world->bitacoraRivales[j].size() > 0 )
			{
				di = ( tmp_action.target - opponent_it->pos ).normita();
				Di = di/n;
				pi = 1 - V/(V+Di);

				if( pi < p_min )	p_min = pi;
			}
		}
		tmp_action.prob = p_min;
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
		runWithBall( choosen_action.target.x, choosen_action.target.y,
				world->me.pos.x, world->me.pos.y,
				world->me.angleDeg(),
				world->bitacoraBalon.begin()->pos.x, world->bitacoraBalon.begin()->pos.y,
				world->bitacoraBalon.begin()->dis,
				agent_vel,
				world->me.headAngleDeg(),
				1.0,
				command );
		break;
	case PASS:
		kick_vec =
				PasePunto( choosen_action.target.x,
						choosen_action.target.y,
						world->me.pos.x,
						world->me.pos.y,
						world->me.angleDeg(),
						2.0,     // velocidad final
						world->bitacoraBalon.begin()->pos.x,
						world->bitacoraBalon.begin()->pos.y);
		command->append_kick( kick_vec.x, kick_vec.y );
		break;
	case SHOOT:
		kick_vec =
				PasePunto( choosen_action.target.x,
						choosen_action.target.y,
						world->me.pos.x,
						world->me.pos.y,
						world->me.angleDeg(),
						3.0,     // velocidad final
						world->bitacoraBalon.begin()->pos.x,
						world->bitacoraBalon.begin()->pos.y);
		command->append_kick( kick_vec.x, kick_vec.y );
		break;
	}

}

void PokTaPokAgentV1::balonEquipo()
{
	// mantener centrado el balón en el cono de visión

	//centerBall(game_data->sensor_handler.last_see.ballIsVisible(),  // es visible el balon?
	//		sensor_h->last_see.ball.dir,                         // angulo donde se encuentra el balon
	//		world->bitacoraBalon.begin()->dir,                   // angulo del tiempo t-1
	//		game_data->sensor_handler.last_sense.head_angle,     // angulo del cuerpo al cuello
	//		world->me.view_mode_w,                               // modo del cono de vision
	//		command);                                            // comando
	//command->reset(); // para probar voronoi
	voronoiPositioning();

}
void PokTaPokAgentV1::balonRival()
{
	bool aux;

	vector<Player>  &agentes   = sensor_h->last_see.players;

	aux =  amTheClosest(sensor_h->last_see.ball.dis,
			sensor_h->last_see.ball.dir,
			world->me.pos.x,
			world->me.pos.y,
			world->me.angle,
			world->me.head_angle,
			&agentes
	);

	if( aux  )
	{
		//irAlBalon();
		ball_interception->call( command );
		// necesitamos info con mayor frecuencia

	}
	else
	{
		//centerBall(game_data->sensor_handler.last_see.ballIsVisible(),  // es visible el balon?
		//		sensor_h->last_see.ball.dir,                         // angulo donde se encuentra el balon
		//		world->bitacoraBalon.begin()->dir,                   // angulo del tiempo t-1
		//		game_data->sensor_handler.last_sense.head_angle,     // angulo del cuerpo al cuello
		//		world->me.view_mode_w,                               // modo del cono de vision
		//		command);                                     // comando
		//command->reset(); // para probar voronoi
		voronoiPositioning();


	}


}
void PokTaPokAgentV1::balonDesconocido()
{
	// mantener centrado el balón en el cono de visión

	//centerBall(game_data->sensor_handler.last_see.ballIsVisible(),  // es visible el balon?
	//		sensor_h->last_see.ball.dir,                         // angulo donde se encuentra el balon
	//		world->bitacoraBalon.begin()->dir,                   // angulo del tiempo t-1
	//		game_data->sensor_handler.last_sense.head_angle,     // angulo del cuerpo al cuello
	//		world->me.view_mode_w,                               // modo del cono de vision
	//		command);                                     // comando
	//command->reset(); // para probar voronoi
	voronoiPositioning();                                     // comando

}

void PokTaPokAgentV1::balonSuelto()
{
	bool   aux;
	double x        = world->me.pos.x;
	double y        = world->me.pos.y;
	double angle    = world->me.angle;
	double neck_dir = game_data->sensor_handler.last_sense.head_angle;
	vector<Player>  &agentes   = sensor_h->last_see.players;
	//  Vector2D balon;
	Vector2D velocidad;

	velocidad.x = game_data->sensor_handler.last_sense.speed_amount;    // dis
	velocidad.y = game_data->sensor_handler.last_sense.speed_direction; // dir

	aux = amTheClosest(sensor_h->last_see.ball.dis,
			sensor_h->last_see.ball.dir,
			x,y,angle,neck_dir,
			&agentes
	);

	//cout<<"MAS CERCANO: "<<aux<<endl;
	if( aux )
	{
		//irAlBalon();
		ball_interception->call( command );
		//radar( game_data->sensor_handler.last_sense.head_angle,
			//	command );

	}
	else
	{
		//cout<<"CENTRO EL BALON**"<<endl;
		//centerBall(game_data->sensor_handler.last_see.ballIsVisible(),  // es visible el balon?
		//		sensor_h->last_see.ball.dir,                         // angulo donde se encuentra el balon
		//		world->bitacoraBalon.begin()->dir,                   // angulo del tiempo t-1
		//		game_data->sensor_handler.last_sense.head_angle,     // angulo del cuerpo al cuello
		//		world->me.view_mode_w,                               // modo del cono de vision
		//		command);                                     // comando
		//command->reset(); // para probar voronoi
		voronoiPositioning();


	}
}

void PokTaPokAgentV1::balonPerdido()
{
	//searchBall(world->bitacoraBalon.begin()->dir,world->me.view_mode_w,command);

	if( world->bitacoraBalon.empty() )
		searchBall(world->bitacoraBalon.begin()->dir,world->me.view_mode_w,command);
	else if( world->time - world->bitacoraBalon.begin()->ciclo > 6 )
	{
		searchBall(world->bitacoraBalon.begin()->dir,world->me.view_mode_w,command);
	}
	else
	{
		voronoiPositioning();
	}

}

void PokTaPokAgentV1::irAlBalon()
{
	Vector2D velocidad;
	Vector2D balon;


	double x     = world->me.pos.x;
	double y     = world->me.pos.y;
	double angle = world->me.angle;

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
      if( (world->time - world->bitacoraBalon.begin()->ciclo) < POT_FIELD_EXPIRE_TIME )
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
       searchBall(world->bitacoraBalon.begin()->dir,
    		      world->me.view_mode_q,
    		      command);
      }
	}

	/*
    // alineo cuello con el cuerpo
    command->append_turn_neck(-neck_dir);
    // busco balon
    searchBall(world->bitacoraBalon.begin()->dir,
                   world->me.view_mode_w,
                   command);
	 */

}


void PokTaPokAgentV1::voronoiPositioning()
{
	// Para la celda inicial
	std::vector<Point2D> initial_cell;
	Point2D q;
	double const INITIAL_CELL_WIDTH = 30;
	double  y_top	 = 30.0;
	double  y_bottom = -30.0;
	double  x_left;
	double 	x_right;
	Vector2D b;

	// Para la construcción de la celda voronoi
	Point2D p ( world->me.pos.x, world->me.pos.y );
    std::vector<Point2D> sites;
    std::vector<Point2D> cell_container;
    std::deque<ObjetoBitacora>::const_iterator teammate_it;

    Vector2D centroid;

    // general stuff
    BodySensor & body = game_data->sensor_handler.last_sense;
    int i;
    Vector2D tmp_v;
    Point2D  tmp_p;
	Vector2D agent_vel = Vector2D::fromPolar( body.speed_amount,
			Deg2Rad( body.speed_direction + world->me.angleDeg() + world->me.headAngleDeg() ) );

	if( world->bitacoraBalon.empty() )
	{
		// No podemos aplicar posicionamiento
		return;
	}
	b = world->bitacoraBalon.begin()->pos;
	if( b.x - INITIAL_CELL_WIDTH < -52.5 )
	{
		x_left  = -52.5;
		x_right = -52.5 + 2*INITIAL_CELL_WIDTH;
	}
	else if( b.x + INITIAL_CELL_WIDTH > 52.5 )
	{
		x_left  = 52.5 - 2*INITIAL_CELL_WIDTH;
		x_right = 52.5;
	}
	else
	{
		x_left  = b.x - INITIAL_CELL_WIDTH;
		x_right = b.x + INITIAL_CELL_WIDTH;
	}

	q.set( x_left,  y_bottom ); initial_cell.push_back( q );
	q.set( x_left,  y_top	 ); initial_cell.push_back( q );
	q.set( x_right, y_top 	 ); initial_cell.push_back( q );
	q.set( x_right, y_bottom ); initial_cell.push_back( q );
	q.set( x_left,  y_bottom ); initial_cell.push_back( q );

	if( p.x() < x_left ) // No estamos en el area de la celda inicial de voronoi
	{
		// Corremos hacia el area
		GoToXY( p.x() + 10, p.y(),
				p.x(), p.y() ,
				world->me.angleDeg(),
				world->me.headAngleDeg(),
				3.0,
				agent_vel,
				command);
/*
		void GoToXY ( double   xTarget ,
				double   yTarget ,
				double   x,
				double   y,
				double   angle,
				double 	 neck_dir,
				double   radio,
				Vector2D velocidad,
				AgentCommand * command
		);*/
	}

	else if( p.x() > x_right ) // No estamos en el area de la celda inicial de voronoi
	{
		GoToXY( p.x() - 10, p.y(),
				p.x(), p.y() ,
				world->me.angleDeg(),
				world->me.headAngleDeg(),
				3.0,
				agent_vel,
				command);
	}
	else
	{
		for( i=0; i<11; i++ ) // Para cada teammate
		{
			teammate_it = world->bitacoraAmigos[i].begin();
			if( world->time - teammate_it->ciclo < 10
					&&  i != (world->me.unum -1) // No nos consideramos a nosotros mismos
					&&  i != 0	// No consideramos al portero
					&& world->bitacoraAmigos[i].size() > 0
			)
			{ // la información aún es útil
				tmp_p.set( teammate_it->pos.x, teammate_it->pos.y );
				sites.push_back( tmp_p );
			}
		}

		constructVoronoiCell( p, sites, initial_cell, &cell_container );

		// Voronoi trabaja usando la celda final igual a la primera, descartamos
		// cualquiera de las dos para calcular el centroide
		cell_container.pop_back();
		std::vector<Point2D>::const_iterator cell_it;

		int n = cell_container.size();


		if( n == 0 )
		{
			std::cout << "PokTaPok_" << world->me.unum << ": voronoi cell has no vertex" << std::endl;
			return;
		}


		centroid = 0.0;
		for( cell_it = cell_container.begin();
			 cell_it != cell_container.end();
			 ++cell_it	)
		{
			centroid.x += cell_it->x();
			centroid.y += cell_it->y();
		}
		centroid /= (double)n;

		//if( world->me.unum == 5 )
		//{
			//std::cout << "centroid " << centroid << std::endl;
		//}

		GoToXY( centroid.x, centroid.y,
				p.x(), p.y(),
				world->me.angleDeg(),
				world->me.headAngleDeg(),
				2.0,
				agent_vel,
				command );

	}

}
