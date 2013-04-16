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

	radar( world->me.headAngleDeg(), command, game_data->sensor_handler.last_sensor_type);

}

void PokTaPokAgentV1::onPrep()
{
	if(    game_data->sensor_handler.last_hear_referee.play_mode != KICK_OFF_L
		&& game_data->sensor_handler.last_hear_referee.play_mode != KICK_OFF_R )
	{
	//voronoiPositioning();
	}

	 if(world->me.unum == 1)
	 {
		 goalieBehavior();
	 }
}

void PokTaPokAgentV1::onPlay()
{
	SensorType sensor_type = game_data->sensor_handler.last_sensor_type;

	world->me.possession_ball = whoHasTheBall( world );

    if(world->me.unum != 1)
    {
        switch( world->me.possession_ball)
        {
        case PROPIA:      balonPropio();     break;
        case EQUIPO:      balonEquipo();      break;
        case RIVAL:       balonRival();       break;
        case SUELTA:      balonSuelto();      break;
        case PERDIDA:     balonPerdido();     break;
        }

        radar(world->me.head_angle,command,sensor_type);
    }
    else
    {
        // Es el portero
        goalieBehavior();
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
	double const RUN_FACTOR			= 1.0;

	Vector2D resultant = 0; // vector resultante, hacia donde deberíamso de correr
	Vector2D g 		   = 0;	// vector hacia la meta
    Vector2D running_pot_field[11];

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
		if( world->bitacoraRivales[i].size() > 0 )
		if( world->time - opponent_it->ciclo < POT_FIELD_EXPIRE_TIME )
		{ // la información aún es útil

			running_pot_field[ i ] = world->me.pos - opponent_it->pos ;
			d = running_pot_field[i].normita();
			running_pot_field[i] *= (k/ (d*d) );
			//std::cout << "added repulsor " << running_pot_field[i] << std::endl;
			resultant += running_pot_field[i];
		}

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

	if( n <= 0 )
	{
		n = 1; // no se puede llegar en cero ciclos
	}
	n = std::ceil(n) + 2; // Se le aumentan 2 porque es el tiempo que tardamos llegar al balón otra vez
	for( j=0; j<11; j++ )
	{
		opponent_it = world->bitacoraRivales[j].begin();
		if( world->bitacoraRivales[i].size() > 0 )
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
		if( world->bitacoraAmigos[i].size() > 0 )
		if( world->time - teammate_it->ciclo < PASS_OPTION_EXPIRE_TIME
				&&  i != (world->me.unum -1) // No nos consideramos a nosotros mismos
				&&  i != 0	// No consideramos al portero para los pases
		)
		{ // la información aún es útil

			tmp_action.action_type = PASS;

			tmp_action.target 		= teammate_it->pos;

			tmp_action.dist_to_goal = ( tmp_action.target - goal_pos ).normita();

			r  = (p-tmp_action.target).normita(); // Calculamos la distancia hacia el receptor

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
				if( world->time - opponent_it->ciclo < PASS_OPTION_EXPIRE_TIME )
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

		if( n <= 0 )
		{
			n = 1; // no se puede llegar en cero ciclos
		}

		for( j=0; j<11; j++ ) // Calculamos la probabilidad de éxito con cada uno de los rivales y tomamos la mínicma
		{
			opponent_it = world->bitacoraRivales[j].begin();
			if( world->bitacoraRivales[j].size() > 0 )
			if( world->time - opponent_it->ciclo < PASS_OPTION_EXPIRE_TIME )
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

		if( n <= 0 )
		{
			n = 1; // no se puede llegar en cero ciclos
		}

		for( j=0; j<11; j++ ) // Calculamos la probabilidad de éxito con cada uno de los rivales y tomamos la mínicma
		{
			opponent_it = world->bitacoraRivales[j].begin();
			if( world->bitacoraRivales[j].size() > 0 )
			if( world->time - opponent_it->ciclo < PASS_OPTION_EXPIRE_TIME )

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

	aux =  amTheClosest(world);

	if( aux  )
	{
		irAlBalon();
		//ball_interception->call( command );
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
		//defence();


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

	aux = amTheClosest(world);

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
		//defence();
	}
}

void PokTaPokAgentV1::balonPerdido()
{
	//searchBall(world->bitacoraBalon.begin()->dir,world->me.view_mode_w,command);

	if( world->bitacoraBalon.empty() )
		searchBall(command,world, game_data->sensor_handler.last_sensor_type);
	else if( world->time - world->bitacoraBalon.begin()->ciclo > 6 )
	{
		searchBall(command,world, game_data->sensor_handler.last_sensor_type);
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
       searchBall(command,world, game_data->sensor_handler.last_sensor_type);
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
	double const INITIAL_CELL_WIDTH = 60;
	double const INITIAL_CELL_RIGHT = 20;
	double const INITIAL_CELL_LEFT  = 40;
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
    std::vector<Point2D>::const_iterator 	  cell_it;

    Vector2D centroid;

    // general stuff
    BodySensor & body = game_data->sensor_handler.last_sense;
    int i, n;
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
	if( b.x - INITIAL_CELL_LEFT < -52.5 )
	{
		x_left  = -52.5;
		x_right = -52.5 + INITIAL_CELL_WIDTH;
	}
	else if( b.x + INITIAL_CELL_RIGHT > 52.5 )
	{
		x_left  = 52.5 - INITIAL_CELL_WIDTH;
		x_right = 52.5;
	}
	else
	{
		x_left  = b.x - INITIAL_CELL_LEFT;
		x_right = b.x + INITIAL_CELL_RIGHT;
	}
	// en caso de que estemos fuera del campo
	//if( world->me.pos.y > 30 )
	//{
	//	y_top = world->me.pos.y + 5;
	//}
	//else if( world->me.pos.y < -30 )
	//{
	//	y_bottom = world->me.pos.y - 5;
	//}
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
	}

	else if( p.x() > x_right ) // No estamos en el area de la celda inicial de voronoi
	{
		GoToXYSlow( p.x() - 10, p.y(),
				p.x(), p.y() ,
				world->me.angleDeg(),
				world->me.headAngleDeg(),
				2.0,
				agent_vel,
				command);
	}
	else
	{
		for( i=0; i<11; i++ ) // Para cada teammate
		{
			teammate_it = world->bitacoraAmigos[i].begin();
			if( world->bitacoraAmigos[i].size() > 0 )
			if( world->time - teammate_it->ciclo < 10
					&&  i != (world->me.unum -1) // No nos consideramos a nosotros mismos
					&&  i != 0	// No consideramos al portero
			)
			{ // la información aún es útil
				tmp_p.set( teammate_it->pos.x, teammate_it->pos.y );
				sites.push_back( tmp_p );
			}
		}

		constructVoronoiCell( p, sites, initial_cell, &cell_container );


		// Voronoi trabaja usando la celda final igual a la primera, descartamos
		// cualquiera de las dos para calcular el centroide
		n = cell_container.size();

		if( !cell_container.empty() )
			cell_container.pop_back();
		else
		{
			//std::cerr << "voronoi cell has no vertex" << std::endl;

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

		GoToXYSlow( centroid.x, centroid.y,
				p.x(), p.y(),
				world->me.angleDeg(),
				world->me.headAngleDeg(),
				2.0,	// Usamos un radio grande para disminuir la cantidad de movimientos debido al alto dinamismo del entorno
				agent_vel,
				command );

	}

}

void
PokTaPokAgentV1::defence()
{
	BodySensor & body = game_data->sensor_handler.last_sense;

	Vector2D b; // Posición del balón
	Vector2D q; // Posición del origen del movimiento
	Vector2D p; // Nuestra posición
	//std::vector<Vector2D> mov; // Movimientos posibles del enemigo
	std::vector<ActionOption> posible_actions;
	std::vector<Vector2D>	obstruct_points;//
	ActionOption tmp_action;
	ActionOption choosen_action;
	int q_unum; // número del jugador más cercano al balón

	// Para predecir la dirección en que correrá el enemigo
	Vector2D tmp_v;
	Vector2D resultant;
	Vector2D g;
	Vector2D goal;
	double d;
	double const V 					= 0.94;
	double const BALL_FINAL_VEL 	= 1.5;
	double const BALL_RUN_FINAL_VEL = 1.0;
	double const k					= 5.0;
	double const RADIUS_1 			= 10.0;
	double const RADIUS_2 			= 25.0;
	double const RADIUS_3 			= 45.0;

	std::deque<ObjetoBitacora>::const_iterator opp_it;
	std::deque<ObjetoBitacora>::const_iterator team_it;

	double p_min;
	double pi;
	double di;
	double Di;
	double r;
	double n;

	int i, j;
	if( world->bitacoraBalon.empty() )
		return;
	if( (world->time - world->bitacoraBalon.begin()->ciclo) >= 5 )
	{
		// No tenemos info del balón, lo buscamos
		return;
	}


	b = world->bitacoraBalon.begin()->pos;
	p = world->me.pos;
	// Identificamos al agente rival más cercano al balón


	q = 1000;
	q_unum =0;
	j = 0;
	for( i=0; i < 11; i++ )
	{
		opp_it = world->bitacoraRivales[i].begin();
		if( !world->bitacoraRivales[i].empty() )
		if(    world->time - opp_it->ciclo < 10 )
		{
			if( (opp_it->pos - b).normita() < (q-b).normita() )
			{
				q = opp_it->pos;
				q_unum = opp_it->num;
			}
			j++;
		}
	}
	if ( j == 0 )
		return; // no datos de rivales

	// Actualizamos el campo potencial y obtenemos la resultante para predecir la dirección en que
	// correría el agente rival
	if( world->me.side == 'l' )
		goal = goal_l; // el objetivo es nuestra meta
	else
		goal = goal_r;

	g =  goal - b; // El vector hacia nuestra meta

	g /= g.normita();

	resultant 	=  g;
	for( i=0; i < 11; i++ )
	{
		team_it = world->bitacoraAmigos[i].begin();
		if( !world->bitacoraAmigos[i].empty() )
		if( world->time - team_it->ciclo < POT_FIELD_EXPIRE_TIME )
		{ // la información aún es útil

			tmp_v 	= q - team_it->pos ;
			d 		= tmp_v.normita();
			if( d == 0 )
			{
				std::cerr << "defence: distancia cero, igualando a 0.0001" << std::endl;
				d = 0.0001;
			}
			tmp_v  *= (k/ (d*d) );

			resultant += tmp_v;
		}
	}

	tmp_action.action_type = RUN;
	tmp_action.target = resultant + q;
	tmp_action.dist_to_goal = (goal - tmp_action.target).normita();
	// Obtenemos la probabilidad de correr, se obtiene diferente a las demás, ya que
	// la velocidad final es menor a la del pase
	p_min = 1;
	r = (q - tmp_action.target).normita(); // Calculamos la distancia hacia el punto al que correría
	n =   std::log( 1 - r*(1-1/0.94)/BALL_RUN_FINAL_VEL ) // Calculamos el tiempo que tarda en llegar el balón
	/ std::log( 1/0.94 );

	if( n == 0  || isnan(n) ) n = 1;

	n = std::ceil(n) + 2; // Se le aumentan 2 porque es el tiempo que tardamos llegar al balón otra vez
	for( j=0; j<11; j++ )
	{
		team_it = world->bitacoraAmigos[j].begin();
		if( !world->bitacoraAmigos[j].empty() )
		if( world->time - opp_it->ciclo < PASS_OPTION_EXPIRE_TIME )
		{
			di = (tmp_action.target-team_it->pos).normita();
			Di = di/n;
			pi = 1 - V/(V+Di); // probabilidad de que el pase/movimiento sea exitoso
			if( pi < p_min )
				p_min = pi;
		}
	}
	tmp_action.prob = p_min;//*RUN_FACTOR; // Añadido RUN_FACTOR para darle preferencia a los pases y al tiro

	posible_actions.push_back( tmp_action );

	// Obtenemos todas sus opciones de pase
	for( i=0; i<11; i++ )
	{
		opp_it = world->bitacoraRivales[i].begin();
		p_min = 1;
		if( ! world->bitacoraRivales[i].empty() )
		if( world->time - opp_it->ciclo < PASS_OPTION_EXPIRE_TIME // info reciente
				&&  i != (q_unum -1) // No consideramos al que tiene el balón
				&&  i != 0	// No consideramos al portero para los pases
		)
		{

			tmp_action.action_type = PASS;
			tmp_action.target 		= opp_it->pos;
			tmp_action.dist_to_goal = ( tmp_action.target - goal ).normita();

			r  = (p-tmp_action.target).normita(); // Calculamos la distancia hacia el receptor
			n  =    std::log( 1 - r*(1-1/0.94)/BALL_FINAL_VEL ) // Calculamos el tiempo que tarda en llegar al receptor
			/ std::log( 1/0.94 );

			n  = std::ceil(n);
			if( n == 0 || isnan(n) ) n = 1;
			for( j=0; j<11; j++ ) // Calculamos la probabilidad de éxito con cada uno de los rivales y tomamos la mínima
			{
				team_it = world->bitacoraAmigos[j].begin();
				if( ! world->bitacoraAmigos[j].empty() )
				if( world->time - team_it->ciclo < PASS_OPTION_EXPIRE_TIME )
				{
					di = ( tmp_action.target - team_it->pos ).normita();
					Di = di/n;
					pi = 1 - V/(V+Di);

					if( pi < p_min )	p_min = pi;
				}
			}
			tmp_action.prob = p_min;
			posible_actions.push_back( tmp_action );
		}
	}

	if( (b-goal).normita() < 25 )
	{
		// añadimos el tiro a gol
		tmp_action.action_type = SHOOT;
		tmp_action.dist_to_goal = 0.0;
		tmp_action.target = goal;

		p_min = 1;
		r  = (p-tmp_action.target).normita(); // Calculamos la distancia hacia el receptor
		n  =    std::log( 1 - r*(1-1/0.94)/BALL_FINAL_VEL ) // Calculamos el tiempo que tarda en llegar al receptor
		/ std::log( 1/0.94 );
		n  = std::ceil(n);
		for( j=0; j<11; j++ ) // Calculamos la probabilidad de éxito con cada uno de los rivales y tomamos la mínima
		{
			team_it = world->bitacoraAmigos[j].begin();
			if( ! world->bitacoraAmigos[j].empty() )
			if( world->time - team_it->ciclo < PASS_OPTION_EXPIRE_TIME )
			{
				di = ( tmp_action.target - team_it->pos ).normita();
				Di = di/n;
				if( n <= 0 )
				{
					n = 1; // no se puede llegar en cero ciclos
				}
				pi = 1 - V/(V+Di);

				if( pi < p_min )	p_min = pi;
			}
		}
		tmp_action.prob = p_min;
		posible_actions.push_back( tmp_action );
	}



	std::sort( posible_actions.begin(), posible_actions.end(), ActionOption::distanceLowerThan );

	std::vector<ActionOption>::const_iterator action_it;
	double area_r;
	bool area_changed = false;

	action_it = posible_actions.begin();

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
	for( 	action_it = posible_actions.begin();
			action_it != posible_actions.end() && area_changed == false;
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
	Vector2D bm; // Vector del balón hacia el resultado del movimiento
	Vector2D bp = (p-b); // Vector del balón hacia mi posición
	double proy_mag;
	Vector2D proy;

	bm = choosen_action.target - b;
	proy_mag = bp.prodPunto( bm );
	if( proy_mag < 0 ) // la magnitud es negativa, entonces el punto más cercano es b
	{
		tmp_v = b;
	}
	else if( proy_mag >= bm.normita() ) // La magnitud es más grande que la que hay a del balón destino, el punto más cercano es el destino
	{
		tmp_v = choosen_action.target;
	}
	else // la proyección se encuentra dentro de v
	{
		proy = bm * (proy_mag / bm.normita() );
		tmp_v = b + proy;
	}

	Vector2D agent_vel = Vector2D::fromPolar( body.speed_amount,
			Deg2Rad( body.speed_direction + world->me.angleDeg() + world->me.headAngleDeg() ) );

	//if( world->me.unum == 2 )
	//{
	//	std:cout << "target " << tmp_v << std::endl;
	//}

	GoToXY( tmp_v.x, tmp_v.y,
			p.x, p.y,
			world->me.angleDeg(), world->me.headAngleDeg(),
			1.0,
			agent_vel,
			command);
}

void PokTaPokAgentV1::goalieBehavior()
{

    double  xTarget = -48.0;  // Linea donde el portero se va a estar moviendo
    bool    xEnAreaGrande;
    int num;
    static bool atrapada,free_kick;
    Vector2D kick;

    if( world->me.side == 'r' )
    {
    	xTarget = 48.0;
    	xEnAreaGrande = world->bitacoraBalon.begin()->pos.x > 36 &&    // area grande del portero
    	             world->bitacoraBalon.begin()->pos.y > -20 &&
    	             world->bitacoraBalon.begin()->pos.y <  20;
    }
    else
    {
    	xEnAreaGrande = world->bitacoraBalon.begin()->pos.x < -36 &&    // area grande del portero
    	    	             world->bitacoraBalon.begin()->pos.y > -20 &&
    	    	             world->bitacoraBalon.begin()->pos.y <  20;
    }
    Vector2D velocidad;

    velocidad.x = game_data->sensor_handler.last_sense.speed_amount;    // dis
    velocidad.y = game_data->sensor_handler.last_sense.speed_direction; // dir

    if( (world->time - world->bitacoraBalon.begin()->ciclo ) < 10 )
    {
        if(  xEnAreaGrande  )
        {
            if( (world->bitacoraBalon.begin()->dis < 1.0 &&
                 world->time - world->bitacoraBalon.begin()->ciclo < 2 )
                    )
            {   // Puedo atrapar la pelota

                //cout<<"Puedo atrapar la pelota: "<<atrapada<<endl;
                if(atrapada!= true)  // si no la he atrapado hago catch()
                {
                    command->append_catch( world->bitacoraBalon.begin()->dir );
                   // cout<<"*** Aplico catch "<<world->time<<" referre dice: "
                     //  <<game_data->sensor_handler.last_hear_referee.play_mode<<endl;
                }

                if( game_data->sensor_handler.last_hear_referee.play_mode ==
                    GOALIE_CATCH_BALL_L
                    || game_data->sensor_handler.last_hear_referee.play_mode ==
                                        GOALIE_CATCH_BALL_R )
                  {  atrapada = true;
                     // cout<<"ATRAPADA correcta "<<world->time<<endl;
                  }

                if( game_data->sensor_handler.last_hear_referee.play_mode ==
                    FREE_KICK_L
                    || game_data->sensor_handler.last_hear_referee.play_mode ==
                            FREE_KICK_R )
                {
                    free_kick = true;
                    //cout<<"PUEDO PASAR "<<world->time<<endl;
                }

                if( free_kick == true && atrapada == true ) // Atape el balon y puedo pasar
                {
                    num = aQuienPasar(world);

                   // cout<<"Pasar ahora, numero: "<<num<<endl;

                    if(num == -1)   // si regresa -1 no ve ningun agente
                    {

                        kick = PasePunto( 0.0,
                                          0.0,
                                          world->me.pos.x,
                                          world->me.pos.y,
                                          world->me.angle,
                                          3.5,
                                          world->bitacoraBalon.begin()->pos.x,
                                          world->bitacoraBalon.begin()->pos.y );
                        //command->append_turn(170);
                        //radar(world->me.head_angle,command,game_data->sensor_handler.last_sensor_type);

                        command->append_kick(kick.x,kick.y);

                    }
                    else
                    {
                        kick = PasePunto( world->bitacoraAmigos[num].begin()->pos.x,
                                          world->bitacoraAmigos[num].begin()->pos.y,
                                          world->me.pos.x,
                                          world->me.pos.y,
                                          world->me.angle,
                                          2.0,
                                          world->bitacoraBalon.begin()->pos.x,
                                          world->bitacoraBalon.begin()->pos.y );

                        command->append_kick(kick.x,kick.y);
                    }
                }


            }  // balon en el area pero lejos del portero, va por el
            else
            {
                radar(world->me.head_angle,command,game_data->sensor_handler.last_sensor_type);
                atrapada = false;
                free_kick = false;
                ball_interception->call( command );
                // irAlBalon();

            }
        }
        else
        {
            if( world->me.pos.x < xTarget + 1.0 && world->me.pos.x > xTarget - 1.0 )
            {  // estoy en la linea de defensa
                porteroLine(xTarget,
                            velocidad,
                            game_data->sensor_handler.last_sensor_type,
                            world,command);
            }
            else
            {
                radar(world->me.head_angle,command,game_data->sensor_handler.last_sensor_type);
                GoToXY( xTarget,
                        0.0,     //yTarget = 0.0
                        1.0,
                        velocidad,
                        command,
                        world);
            }
        }
    }
    else    // No tendo información reciente del balón
    {
        searchBall(command, world, game_data->sensor_handler.last_sensor_type);
    }
}  // Termina el comportamiento del portero
