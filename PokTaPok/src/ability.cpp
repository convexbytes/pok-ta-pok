#include "ability.h"

void
PassToPoint::call()
{
	double kick_power, kick_direction;
	double dis_to_point;
	double dis_BP;
	double speed;
	double rel_angle;
	double mom_BP;
	double act_kpr;
	Vector2D agent_p =  m_world->me().pos;
	Vector2D ball_p  = m_world->estBallPosition();
	//Distancia del jugador al punto final del balón
	dis_to_point = (m_target - agent_p).mag();

	//Distancia del jugador al balón
	dis_BP		= (ball_p - agent_p).mag();

	speed      = kickSpeedToTravel(dis_to_point, m_desired_vel);    //velocidad inicial necesaria

	rel_angle  = Rad2Deg( atan2(m_target.y - agent_p.y, m_target.x - agent_p.x) )
				 - m_world->me().angleDeg();  //Ángulo para kick

	mom_BP      = Rad2Deg( atan2(ball_p.y - agent_p.y, ball_p.x - agent_p.x) )
				 - m_world->me().angleDeg(); //Ángulo entre posicíon del balon y jugador

	act_kpr    = actualKickPowerRate(mom_BP, dis_BP);        // Valor real del poder del kick
	kick_power = rint( kickPowerForSpeed(speed, act_kpr) );

	if(kick_power > 100.0)
		kick_power = 100.0;

	rel_angle 		= entre180(rel_angle);
	kick_direction  = rint( rel_angle );

	m_command->append_kick( kick_power, kick_direction );
}

void
PassToPoint::setTarget( const Vector2D & v )
{
	m_target = v;
}

void
PassToPoint::setDesiredVel( double desired_vel )
{
	m_desired_vel = desired_vel;
}

double
PassToPoint::actualKickPowerRate(double dir,double dist)
{
	double const kick_power_rate = 0.027,kickable_margin = 0.7;
	double act_kpr = kick_power_rate * (( 1.0 - ((0.25 * dir) / 180.0)) - ((0.25 * dist) / kickable_margin));

	return act_kpr;
}

double
PassToPoint::kickPowerForSpeed(double speed,double actkpr)
{
	double kickPower = 0.0;

	if( actkpr != 0.0 )
		kickPower  = speed / actkpr;

	return kickPower;
}

double
PassToPoint::kickSpeedToTravel(double d,double e)
{
	if( e == 0.0 )
	{
		e = 0.01;
	}
	double const ball_decay = 0.94;
	double r = 1.0 / ball_decay;
	double n = log(d*(r - 1.0)/e + 1.0 ) / log(r);
	double s = e / pow(ball_decay,n);

	return s;   // Vel inicial necesaria para que cumpla la Vel final deseada
}

void
GoToPoint::call()
{
	double  dis_to_point;
	double  dir_to_point;
	Vector2D agent_p = m_world->me().pos;
	double  ang_permisible;
	double  turn_parameter;
	Vector2D velocidad;
	double  dash_parameter;
	double const dash_power_rate = 0.006, effort = 0.8;
	double const inertia_moment = 5.0;

	dis_to_point = (m_target - agent_p).mag();
	dir_to_point = Rad2Deg( atan2( m_target.y - agent_p.y , m_target.x - agent_p.x) );
	if( dis_to_point > m_radius )  // El agente no ha llegado al punto
	{
		ang_permisible = Rad2Deg( atan2( m_radius , dis_to_point ) );
		turn_parameter = dir_to_point - m_world->me().angleDeg();
		turn_parameter = entre180( turn_parameter );
		velocidad.x	  = m_world->me().speed_amount;
		velocidad.y   = m_world->me().speed_dir;
		if( ang_permisible < turn_parameter * 0.1  ||  dis_to_point > 25.0 )
			ang_permisible = 25.0;

		if( fabs(turn_parameter) > ang_permisible )  // el agente no esta bien alineado al punto
		{
			velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y) );
			turn_parameter =  turn_parameter *(1.0 + inertia_moment*velocidad.mag() );
			turn_parameter = entre180( turn_parameter );
			m_command->append_turn( turn_parameter );
		}
		else
		{
			velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y - m_world->me().angleDeg()) );
			dash_parameter = ( dis_to_point - velocidad.x )  / ( dash_power_rate * effort );
			if( dash_parameter > 100.0)
				dash_parameter = 100.0;
			if( m_dash_override )
				m_command->append_dash( m_dash_power );
			else
				m_command->append_dash( dash_parameter );
		}
	}
	else
	{
		// El agente llegó al punto
	}
}

void
GoToPoint::setTarget( const Vector2D & v )
{
	m_target = v;
}

void
GoToPoint::setRadius( double r )
{
	m_radius = r;
}

void
GoToPoint::setDashOverride( bool on )
{
	m_dash_override = false;
}

void
GoToPoint::setDashPower( double p )
{
	m_dash_power = p;
}

void
FreezeBall::call()
{
	const BodySensor  & body   = m_world->gameData().sensor_handler.last_sense;
	const ServerParam & param  = m_world->gameData().game_parameter.server_param;
	double pow_needed;
	double angle_needed;
	Vector2D zero_vel(0.0, 0.0);

	Vector2D p 	   =  m_world->me().pos; // Posición del agente
	double  theta = m_world->me().angleDeg(); // Orientación del agente

	Vector2D vn = m_world->estBallVelocity();
	Vector2D bn = m_world->estBallPosition();

	Vector2D pv	= Vector2D::fromPolar( body.speed_amount,
			Deg2Rad( body.speed_direction  + theta ) );

	Vector2D pn_bn; // vector de bn a pn

	Vector2D pn = p + pv; //Posición del agente en el siguiente ciclo

	pn_bn = pn-bn;

	double dist 	 = pn_bn.mag();
	double dir_diff = std::abs( entre180( theta - Rad2Deg( pn_bn.angle() ) ) );

	double f = 1 - 0.25*(dir_diff/180.0) -0.25*(dist/param.kickable_margin);

	if( f == 0.0 ) f = 0.001;

	if( vn.x == 0.0 && vn.y == 0.0 )
	{
		pow_needed = 0.0;
		angle_needed = 0.0;
	}
	else
	{
		pow_needed = vn.mag() / ( param.kick_power_rate * f ) ;
		angle_needed = entre180( Rad2Deg( std::atan2( vn.y, vn.x )  ) + 180 - theta);
	}

	m_command->append_kick( pow_needed, angle_needed );
}

void
BallInterception::start()
{
	m_state = START;
	m_turn_requested = false;
	searchBall();
}

void
BallInterception::call( )
{
	// Detectamos si la habilidad ha sido interrumpuda
	if( m_world->time() - m_last_call_time > 1 )
		m_state = START;

	m_last_call_time = m_world->time();

	State tmp_state = m_state;
	switch( tmp_state )
	{
	case START:
		start();
		break;
	case SEARCH_BALL:
		searchBall();
		break;
	case COMPUTE_POINT_TURN:
		computePointTurn();
		break;
	case GO:
		go();
		break;
	case FREEZE:
		freeze();
		break;
	}
}

void
BallInterception::computePointTurn()
{
	m_state = COMPUTE_POINT_TURN;

	const SeeSensor &	visual = m_world->gameData().sensor_handler.last_see;
	const BodySensor & body  = m_world->gameData().sensor_handler.last_sense;
	const ServerParam & param = m_world->gameData().game_parameter.server_param;
	Vector2D	ball_pos;
	Vector2D	player_pos;
	Vector2D	ball_vel;
	Vector2D	player_vel;
	Vector2D	vel_needed;
	Vector2D	intercept_point;
	double 		angle_deg;
	// Detectamos si se ejecutó nuestro comando turn
	if(   m_world->gameData().command_commited.turn_is_set()
			&& m_world->gameData().command_commited.turn_angle == m_turn_param
			&& m_turn_requested == true )
	{
		m_turn_requested = false;
		go();
	}
	else
	{
		ball_pos 	   = m_world->me().pos
				+ Vector2D::fromPolar( visual.ball.dis,
						Deg2Rad( visual.ball.dir + m_world->me().angleDeg() + m_world->me().headAngleDeg() ) );
		player_pos    = m_world->me().pos;

		m_start_time   = m_world->time();

		player_vel   = Vector2D::fromPolar(  body.speed_amount,
				Deg2Rad(body.speed_direction + m_world->me().angleDeg() + m_world->me().headAngleDeg() ) );

		if( visual.ball.dir_chg != NDEF_NUM && visual.ball.dis_chg != NDEF_NUM )
		{
			ball_vel = fromChgToVel(  player_pos, // Posición del jugador, absoluta.
					player_vel, // Velocidad del jugador, absoluta.
					ball_pos, // Posición del objeto que ve, absoluta.
					visual.ball.dis,
					visual.ball.dis_chg,
					visual.ball.dir_chg// + world->me().headAngleDeg()
			);
		}
		else
		{
			ball_vel = 0.0;
		}
		// Iteramos hasta que encontramos una velocidad promedio
		// que puede lograr el agente a lo largo de los m_time_to_reach ciclos
		m_time_to_reach = 0;
		do
		{
			++m_time_to_reach;
			vel_needed = velToInterceptBall( m_time_to_reach );
		}
		while( vel_needed.mag() >= AVERAGE_VEL );

		//Calculamos el punto de interceptación teórico
		intercept_point = player_pos + vel_needed * m_time_to_reach;

		// Calculamos la velocidad necesaria para llegar en el siguiente ciclo,
		// contemplamos 1 ciclo de atraso por el ajuste de giro

		vel_needed = velToInterceptBall( m_time_to_reach + 1 );

		// Calculamos el punto de intersección
		m_inter_point 	= player_pos +  vel_needed *( m_time_to_reach +1 );

		// Calculamos el ángulo que hay que girar
		angle_deg     =	Rad2Deg ( ( m_inter_point- player_pos ).angle() );

		// Calculamos el turn necesario para alinearnos con el punto
		m_turn_param =
				entre180( angle_deg - m_world->me().angleDeg() /* - world->me().headAngleDeg() */)
				* (1.0 + param.inertia_moment * body.speed_amount ) ;

		if( std::fabs(m_turn_param ) > 4.0 )
		{
			m_command->append_turn( m_turn_param );
			m_turn_requested = true;
		}
		else
		{
			go();
		}
	}
}

Vector2D
BallInterception::velToInterceptBall( int t )
{
	// Regresa la velocidad en x y en y necesarias para interceptar el baĺón en el tiempo t

	Vector2D vel_needed;
	double 	 ln_decay;
	double  decay_ala_t;
	Vector2D b = m_world->estBallPosition();
	Vector2D p = m_world->me().pos;
	Vector2D v = m_world->estBallVelocity();
	double const BALL_DECAY = 0.94;

	if( t == 0 )
		t = 1;

	ln_decay= log( BALL_DECAY );
	decay_ala_t = std::pow( BALL_DECAY, t);
	vel_needed.x = ( b.x - p.x + v.x * ( decay_ala_t - 1 )  / ln_decay )
	                						/
	                					(double)t;
	vel_needed.y = ( b.y - p.y + v.y * ( decay_ala_t - 1 )  / ln_decay )
	                						/
	                					(double)t;
	return vel_needed;
}
void
BallInterception::searchBall()
{
	m_state = SEARCH_BALL;
	if( m_world->gameData().sensor_handler.last_see.ballIsVisible() )
	{
		computePointTurn();
	}
	else
	{
		m_search_ball->call();
	}
}

void
BallInterception::go()
{

	m_state = GO;
	const SeeSensor &	visual = m_world->gameData().sensor_handler.last_see;

	bool point_reached =
			(m_world->me().pos - m_inter_point).mag() <= 1.0;

	if( visual.ball_is_visible && visual.ball.dis <= 1)
	{
		freeze();
	}
	else if(   m_world->time() - m_start_time > m_time_to_reach
			 || point_reached )
	{
		start();
	}
	else
	{
		m_command->append_dash( 100 );
	}
}

void
BallInterception::freeze()
{
	m_state = FREEZE;
	const SeeSensor &	visual = m_world->gameData().sensor_handler.last_see;

	if( visual.ballIsVisible() && visual.ball.dis > 1 )
		start();
	else
		m_freeze_ball->call();
}

void
SearchBall::call()
{
	double lastDirection = m_world->bitacoraBalon.begin()->dir;
	int    idCono        = m_world->me().view_mode_w;
	SensorType sensor_type	= m_world->gameData().sensor_handler.last_sensor_type;
	double cono;

	switch(idCono)
	{
	case NARROW: cono = 60.0;  break;
	case NORMAL: cono = 120.0; break;
	case WIDE:   cono = 180.0; break;
	default:     cono = 0.0;   break; // incorrecto, error
	}

	if( sensor_type== SENSOR_SEE)
	{
		if( lastDirection < 0.0  )
			m_command->append_turn(-cono);
		else
			m_command->append_turn( 60 );
	}
}

void
CenterBall::call()
{
	if( m_world->bitacoraBalon.empty() )
		return;

	if(    m_world->bitacoraBalon.begin()->dir <=  m_precision
		&& m_world->bitacoraBalon.begin()->dir >= -m_precision  )
	{
		// Balón centrado en direccion al cuerpo
	}
	else
	{   // Si neck_dir es menor al maximo-ball.dir, puede girar el cuello, o
		// si neck_dir es mayor al minimo-ball.dir, puede girar el cuello
		if(    m_world->me().head_angle <  90.0 - m_world->bitacoraBalon.begin()->dir
			&& m_world->me().head_angle > -90.0 - m_world->bitacoraBalon.begin()->dir
			)
			m_command->append_turn_neck( m_world->bitacoraBalon.begin()->dir);
		else
		{
			// Si no, alineamos el cuello con el cuerpo y
			// luego giramos el cuerpo en ball.dir+neck_dir
			align();
			m_command->append_turn( m_world->bitacoraBalon.begin()->dir + m_world->me().head_angle);
		}

	}
}

void
CenterBall::setPrecision( double p )
{
	m_precision = p;
}

void
CenterBall::align()
{
	double neck_dir = m_world->me().headAngleDeg();
	if( neck_dir != 0.0 )
		m_command->append_turn_neck( -neck_dir );
	else
	{
		// El cuello está alineado con el cuerpo
	}
}

void
RunWithBall::call()
{
	double ang_to_point;
	Vector2D aux;
	Vector2D pos  = m_world->me().pos;
	Vector2D ball = m_world->estBallPosition();
	double disBall = m_world->bitacoraBalon.begin()->dis;
	if ( disBall <= m_radius )
	{
		ang_to_point = Rad2Deg( atan2(  m_target.y- pos.y, m_target.x - pos.x) );

		// Este m_pass_distance es similar a lo que queremos que adelante la pelota,
		// para valores grandes la adelanta mucho.
		aux = Vector2D::fromPolar( m_pass_distance, Deg2Rad( ang_to_point ) );

		m_pass_to_point->setDesiredVel( m_ball_final_vel );
		m_pass_to_point->setTarget( pos + aux );
		m_pass_to_point->call();
	}
	else
	{
		m_go_to_point->setDashOverride( false );
		m_go_to_point->setRadius( m_radius );
		m_go_to_point->setTarget( ball );
		m_go_to_point->call();
	}
}

void
RunWithBall::setRadius( double r )
{
	m_radius = r;
}

void
RunWithBall::setTarget( const Vector2D & v)
{
	m_target = v;
}

void
GoToPoint2::call()
{
	const BodySensor & body = m_world->gameData().sensor_handler.last_sense;
	double  dis_to_point;
	double  dash_parameter;
	double const dash_power_rate = 0.006, effort = 0.8;
	//double const inertia_moment = 5.0;
	Vector2D  pos   = m_world->me().pos;
	double   angle = m_world->me().angleDeg();
	Vector2D  velocidad;

	dis_to_point = (m_target - pos).mag();

	if( dis_to_point > m_radius )  // El agente no ha llegado al punto
	{

		if( angle >= 0.0)
		{
			if( m_target.y >= 0.0 )
			{
				if( angle < 100.0 && angle > 80.0 ) // Orientación correcta
				{
					velocidad     = Vector2D::fromPolar( body.speed_amount, Deg2Rad( body.speed_direction - angle) );
					dash_parameter = (dis_to_point - velocidad.x)  / ( dash_power_rate * effort );

					if( dash_parameter > 100.0)
						dash_parameter = 100.0;

					if( pos.y - m_target.y < 0.0 )
					{
						m_command->append_dash( dash_parameter );    // caso 1

					}
					else // m_target.y < y
					{
						m_command->append_dash( -dash_parameter );   // caso 2

					}
				}
				else
				{
					m_command->append_turn( 90 - angle  );
					m_command->append_turn_neck( angle - 90);
				}
			}
			else  // m_target.y < 0.0
			{
				if( angle > -100.0 && angle < -80.0 ) // orientacion correcta
				{
					velocidad     = Vector2D::fromPolar( body.speed_amount, Deg2Rad( body.speed_direction - angle) );
					dash_parameter = (dis_to_point - velocidad.x)  / ( dash_power_rate * effort );

					if( dash_parameter > 100.0)
						dash_parameter = 100.0;

					if( pos.y - m_target.y < 0.0 )
					{
						m_command->append_dash( dash_parameter );    // caso 3
					}
					else // m_target.y < y
					{
						m_command->append_dash( -dash_parameter );   // caso 4
					}
				}
				else
				{
					m_command->append_turn( -90 - angle );
					m_command->append_turn_neck( 90 + angle );
				}
			}
		}
		else   // angle < 0.0
		{
			if( m_target.y >= 0.0 )
			{
				if( angle < 100.0 && angle > 80.0 ) // Orientación correcta
				{
					velocidad     = Vector2D::fromPolar( body.speed_amount, Deg2Rad( body.speed_direction - angle) );
					dash_parameter = (dis_to_point - velocidad.x)  / ( dash_power_rate * effort );

					if( dash_parameter > 100.0)
						dash_parameter = 100.0;

					if( pos.y - m_target.y > 0.0 )
					{
						m_command->append_dash( dash_parameter );    // caso 5
					}
					else // m_target.y < y
					{
						m_command->append_dash( -dash_parameter );   // caso 6
					}
				}
				else  // Mal orientado
				{
					m_command->append_turn( 90 - angle  );
					m_command->append_turn_neck( angle - 90 );
				}
			}
			else  // m_target.y < 0.0
			{
				if( angle > -100.0 && angle < -80.0 ) // orientacion correcta
				{
					velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y - angle) );
					dash_parameter = (dis_to_point - velocidad.x)  / ( dash_power_rate * effort );

					if( dash_parameter > 100.0)
						dash_parameter = 100.0;

					if( pos.y - m_target.y > 0.0  )
					{
						m_command->append_dash( dash_parameter );    // caso 7
					}
					else // m_target.y < y
					{
						m_command->append_dash( -dash_parameter );   // caso 8
					}
				}
				else  // mal orientado
				{
					m_command->append_turn( -90 - angle );
					m_command->append_turn_neck( 90 + angle );
				}
			}
		}
	}
	else
	{
		// El agente llego al punto
	}
}

void
GoToPoint2::setTarget( const Vector2D & v )
{
	m_target = v;
}

void
GoToPoint2::setRadius ( double r )
{
	m_radius = r;
}

void
GoalieLine::call()
{
	Vector2D puntoReferencia;  //Coordenadas de la bandera del centro de la porteria
	double  yTarget;
	Vector2D target;
	puntoReferencia.x =-52.5;   // coordenadas de la bandera del centro de la porteria
	puntoReferencia.y = 0.0;
	//
	if(  (m_world->time() - m_world->bitacoraBalon.begin()->ciclo) < 3 ) // si esta reciente los datos del balon
	{

		if( m_world->bitacoraBalon.begin()->pos.x != puntoReferencia.x)
			yTarget = ( (m_world->bitacoraBalon.begin()->pos.y - puntoReferencia.y) /
					(m_world->bitacoraBalon.begin()->pos.x - puntoReferencia.x)
			) * (m_x_target - puntoReferencia.x);
		else
			if(m_world->bitacoraBalon.begin()->pos.y <0.0)
				yTarget = -7.0;
			else
				yTarget = 7.0;

		if(yTarget >= 7.0 )
			yTarget = 7.0;

		if(yTarget <= -7.0)
			yTarget = -7.0;

		if( (m_world->me().pos.y > (yTarget - 0.5)) && (m_world->me().pos.y < (yTarget + 0.5)) )
			m_center_ball->call();
		else
		{
			target.set( m_x_target, yTarget );
			m_go_to_point2->setRadius( 0.5 );
			m_go_to_point2->setTarget( target );
			m_go_to_point2->call();
		}
	}

	else  // No tengo datos recientes del balón, entonces lo busco
	{
		m_search_ball->call();
	}
}

void
GoalieLine::setXTarget( double x )
{
	m_x_target = x;
}

void
Shoot::call()
{
	vector<Player>::const_iterator player_it;
	vector<ObjetoBitacora>::const_iterator opp_it;

	Vector2D post_top	( 52.5, -7 + m_distance_to_post );
	Vector2D post_bottom( 52.5,  7 - m_distance_to_post );

	Vector2D target;

	Vector2D closest_point_top;
	Vector2D closest_point_bot;
	double 	 dist_top;
	double 	 dist_bottom;

	Vector2D goalie_pos;

	Vector2D ball_pos = m_world->bitacoraBalon.begin()->pos;

	bool 	 there_is_goalie;

	if( m_world->me().side == 'r' )
	{
		post_top.x 	  = -post_top.x;
		post_bottom.x = -post_bottom.x;
	}

	// Buscamos al portero
	there_is_goalie = false;
	for( int i=0; i<11; ++i )
		if( m_world->bitacoraRivales[i].empty() == false )
			if( m_world->bitacoraRivales[i].begin()->goalie
					&& m_world->time() - m_world->bitacoraRivales[i].begin()->ciclo < GOALIE_INFO_EXPIRE_TIME
			)
			{
				there_is_goalie = true;
				goalie_pos = m_world->bitacoraRivales[i].begin()-> pos;
			}

	// Si hay portero, escogemos el poste más alejado del portero
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
	else // Si no hay portero, escogemos el poste más cercano
	{
		if(   (m_world->me().pos - post_top).mag()
				< (m_world->me().pos - post_bottom).mag() )
			target = post_top;
		else
			target = post_bottom;
	}
	m_pass_to_point->setDesiredVel( 3.0 );
	m_pass_to_point->setTarget( target );
	m_pass_to_point->call();
}

void
Shoot::setDistanceToPost( double d )
{
	m_distance_to_post = d;
}
