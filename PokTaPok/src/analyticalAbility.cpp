#include "analyticalAbility.h"
#include "gameData.h"
#include "worldModel.h"

#include <cstring>

#ifndef NDEF_NUM
#define NDEF_NUM -999999.0 // Fuera del rango de cualquier información provista por el servidor.
#endif

Vector2D
velToInterceptBall( Vector2D const & b, // Posición del balón
		Vector2D const & p, // Posición del jugador
		Vector2D const & v, // Velocidad del balón
		int t,
		double ball_decay = 0.94 // constante de decaimiento
)
{
	// Regresa la velocidad en x y en y necesarias para interceptar el baĺón en el tiempo t
	if( t == 0 ) t = 1;
	Vector2D vel_needed;
	double ln_decay = log( ball_decay );
	double decay_ala_t = std::pow(ball_decay, t);

	vel_needed.x = ( b.x - p.x + v.x * ( decay_ala_t - 1 )  / ln_decay )
                				/
                				(double)t;
	vel_needed.y = ( b.y - p.y + v.y * ( decay_ala_t - 1 )  / ln_decay )
                				/
                				(double)t;
	return vel_needed;
}

double
ActualKickPowerRate(int dir,double dist)
{
	double const kick_power_rate = 0.027,kickable_margin = 0.7;
	double act_kpr = kick_power_rate * (( 1.0 - ((0.25 * dir) / 180.0)) - ((0.25 * dist) / kickable_margin));

	return act_kpr;
}

double KickPowerForSpeed(double speed,double actkpr)
{
	double kickPower = 0.0;

	if( actkpr != 0.0 )
		kickPower  = speed / actkpr;

	return kickPower;
}

double KickSpeedToTravel(double d,double e)
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

Vector2D PasePunto( double xTarget,    // punto final
		double yTarget,
		double x,          // coordenadas agente
		double y,
		int    angle,      // angulo del cuerpo
		double velDes,     // velocidad final
		double xBall,      // coordenadas de la pelota
		double yBall)
{
	Vector2D Kick;
	double disToPoint;
	double disBP;
	double speed;
	double anguloRel;
	double momBP;
	double act_kpr;
	//Distancia del jugador al punto final del balón
	disToPoint = sqrt(  (xTarget - x)*(xTarget - x) +
			(yTarget - y)*(yTarget - y)
	);

	//Distancia del jugador al balón
	disBP      = sqrt((xBall - x)*(xBall - x) +
			(yBall - y)*(yBall - y));

	speed      = KickSpeedToTravel(disToPoint,velDes);    //velocidad inicial necesaria

	anguloRel  = Rad2Deg( atan2(yTarget - y,xTarget - x) )- angle;  //Ángulo para kick

	momBP      = Rad2Deg( atan2(yBall - y,xBall - x) ) - angle; //Ángulo entre posicíon del balon y jugador

	act_kpr    = ActualKickPowerRate(momBP,disBP);        //valor real del poder del kick

	Kick.x  = rint(KickPowerForSpeed(speed,act_kpr));  //Kick.power  = Kick.x

	if(Kick.x > 100.0)
		Kick.x = 100.0;

	anguloRel = entre180(anguloRel);
	Kick.y = rint( anguloRel );   //Kick.moment = Kick.y

	return Kick;
}

void GoToXY (   double   xTarget ,
		double   yTarget ,
		double   x,
		double   y,
		double   angle,
        double   neck_dir,
		double   radio,
		Vector2D velocidad,
		AgentCommand * command
                )
{

    double  disToPoint;
    double  dirToPoint;
    double  angPermisible;
    double  turnParameter;
    double  dashParameter;
    double const dash_power_rate = 0.006, effort = 0.8;
    double const inertia_moment = 5.0;

    disToPoint = sqrt( (xTarget - x)*(xTarget - x)  + (yTarget - y)*(yTarget - y) );
    dirToPoint = Rad2Deg(atan2( yTarget - y , xTarget - x));

    if( disToPoint > radio )  // el agente no ha llegado al punto
    {

        angPermisible = Rad2Deg( atan2( radio , disToPoint ) );
        turnParameter = dirToPoint - angle;
        turnParameter = entre180( turnParameter );

        if( angPermisible < turnParameter * 0.1  ||  disToPoint > 25.0 )
            angPermisible = 25.0;

        if( fabs(turnParameter) > angPermisible )  // el agente no esta bien alineado al punto
        {

            velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y) );
            turnParameter =  turnParameter *(1.0 + (inertia_moment*velocidad.mag()));
            turnParameter = entre180(turnParameter);
            command->append_turn(turnParameter);

        }
        else
        {

            velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y - angle) );
            dashParameter = (disToPoint - velocidad.x)  / ( dash_power_rate * effort );

            if( dashParameter > 100.0)
                dashParameter = 100.0;
            //cout<<"Avanza DASH"<<endl;
            command->append_dash( dashParameter );
        }
    }
    else
    {
        // el agente llego al punto
    }

}

void runWithBall (double   xTarget ,
                  double   yTarget ,
                  double   xBall,
                  double   yBall,
                  double   disBall,
                  Vector2D velocidad,
                  double   radio,
                  WorldModel * world,
                  AgentCommand * command
                  )
{
    double angToPoint;
    double auxX,auxY;
    double x,y,angle,neck_dir;
    Vector2D Kick;

    x=world->me().pos.x;
    y=world->me().pos.y;
    angle=world->me().angle;
    neck_dir = world->me().head_angle;


    if (disBall <= radio)
    {
        angToPoint = Rad2Deg( atan2(  yTarget   - y,   xTarget   - x) );

        // Este 2.0 es similar a lo que queremos que adelante la pelota,
        // para valores grandes la adelanta mucho.

        auxX = 2.0 * cos( Deg2Rad( angToPoint ) );
        auxY = 2.0 * sin( Deg2Rad( angToPoint ) );

        Kick = PasePunto(x+auxX,y+auxY,x,y,angle,0.5, xBall , yBall);

        //// cout<<"Adelanto KICK"<<endl;
        command->append_kick(Kick.x,Kick.y);
    }
    else
    {
        GoToXY(xBall,yBall,x,y,angle,neck_dir,radio,velocidad,command );  // vamos por la pelota
    }
}

BallInterception::BallInterception( GameData 	   * game_data,
							  WorldModel * world,
							  FreezeBall   * freeze_ball)
{
	this->game_data = game_data;
	this->world		= world;
	this->freeze_ball = freeze_ball;
	this->param     = &game_data->game_parameter.server_param;
	this->visual	= &game_data->sensor_handler.last_see;
	this->body		= &game_data->sensor_handler.last_sense;
	this->start_time_s  = NDEF_NUM;
	this->last_call_time= NDEF_NUM;
	time_to_reach_s = NDEF_NUM;
	turn_requested  = false;
}

void
BallInterception::reset()
{
	state = START;
	start_time_s  	= NDEF_NUM;
	time_to_reach_s = NDEF_NUM;
	turn_requested  = false;
}

void
BallInterception::call( AgentCommand * command )
{

	this->command = command;
	// Eventos generales
	if( world->time() - last_call_time > 1 )
		state = START;

	last_call_time = world->time();

	State tmp_state = state;
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
BallInterception::start()
{
	state = START;
	searchBall();
}

void
BallInterception::searchBall()
{
	state = SEARCH_BALL;
	if( visual->ballIsVisible() )
	{
		computePointTurn();
	}
	else
	{

		//command->append_turn( 60 );
		double lastDirection = world->bitacoraBalon.begin()->dir;
		int    idCono        = world->me().view_mode_w;

		double cono;

		switch(idCono)
		{
		case NARROW: cono = 60.0;  break;
		case NORMAL: cono = 120.0; break;
		case WIDE:   cono = 180.0; break;
		default:     cono = 0.0;   break; // incorrecto, error
		}

		if( game_data->sensor_handler.last_sensor_type== SENSOR_SEE)
		{
			if( lastDirection < 0.0  )
			{
				command->append_turn(-cono);
				//command->append_turn(-15);
			}
			else
			{
				//command->append_turn(cono);
				command->append_turn( 60 );
			}
		}
	}
}

void
BallInterception::computePointTurn()
{
	state = COMPUTE_POINT_TURN;

	double angle_deg, angle_min_deg, angle_max_deg;


	if(   game_data->command_commited.turn_is_set()
			&& game_data->command_commited.turn_angle == turn_param
			&& turn_requested == true )
	{
		turn_requested = false;
		go();
	}
	else
	{
		//world->predictBallCurrentVel( &ball_vel_s );

		ball_pos_s 	   = world->me().pos
				+ Vector2D::fromPolar( visual->ball.dis,
						Deg2Rad( visual->ball.dir + world->me().angleDeg() + world->me().headAngleDeg() ) );
		player_pos_s   = world->me().pos;

		start_time_s   = world->time();

		player_vel_s   = Vector2D::fromPolar(  body->speed_amount,
				Deg2Rad(body->speed_direction + world->me().angle + world->me().head_angle ) );

		if( visual->ball.dir_chg != NDEF_NUM && visual->ball.dis_chg != NDEF_NUM )
		{
			ball_vel_s = fromChgToVel(  player_pos_s, // Posición del jugador, absoluta.
					player_vel_s, // Velocidad del jugador, absoluta.
					ball_pos_s, // Posición del objeto que ve, absoluta.
					visual->ball.dis,
					visual->ball.dis_chg,
					visual->ball.dir_chg// + world->me().headAngleDeg()
			);
		}
		else
		{
			ball_vel_s = 0.0;
		}
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
		while( vel_needed_s.mag() >= AVERAGE_VEL );

		//Calculamos el punto de intercepción teórico
		intercept_point_s = player_pos_s + vel_needed_s* time_to_reach_s;

		// Calculamos velocidades necesarias para llegar en los siguientes
		// ciclos, contemplamos 2 ciclos de ajuste de giro
		v1_s = velToInterceptBall( ball_pos_s,
				player_pos_s,
				ball_vel_s,
				time_to_reach_s + 0 );
		v_mid_s = velToInterceptBall( ball_pos_s,
				player_pos_s,
				ball_vel_s,
				time_to_reach_s + 1 );
		v2_s = velToInterceptBall( ball_pos_s,
				player_pos_s,
				ball_vel_s,
				time_to_reach_s + 2 );
		// Calculamos puntos de intersección posibles, por si "llegamos tarde"
		// Asumimos que no podemos llegar al punto teórico debido a que debemos
		// hacer un turn para alienarnos, lo cual nos resta un ciclo
		p1_s		= player_pos_s +  v1_s    *( time_to_reach_s +0 );
		p_mid_s 	= player_pos_s +  v_mid_s *( time_to_reach_s +1 );
		p2_s 		= player_pos_s +  v2_s    *( time_to_reach_s +2 );
		// Calculamos el ángulo que hay que girar en cada punto
		angle_deg     =	Rad2Deg ( ( p_mid_s- player_pos_s ).angle() );
		angle_min_deg = Rad2Deg ( ( p1_s   - player_pos_s ).angle() );
		angle_max_deg = Rad2Deg ( ( p2_s   - player_pos_s ).angle() );

		//Agregamos margen de error debido al ruido en sensores
		angle_min_deg -=  5 ;
		angle_max_deg +=  5 ;

		// Calculamos el turn necesario para alinearnos con el punto
		turn_param =
				entre180( angle_deg - world->me().angleDeg() /* - world->me().headAngleDeg() */)
				* (1.0 + param->inertia_moment * body->speed_amount ) ;

		if( std::fabs(turn_param ) > 4.0 )
		{
			command->append_turn( turn_param );
			turn_requested = true;
		}
		else
		{
			this->go();
		}

	}
}

void
BallInterception::go()
{
	state = GO;
	bool point_reached =
			(world->me().pos - p_mid_s).mag() <= 1.0;

	if( visual->ball_is_visible && visual->ball.dis < 1)
	{
		freeze();
	}
	else if(    (world->time() - start_time_s) > time_to_reach_s
			 || point_reached )
	{

		start();
	}
	else
	{
		command->append_dash( 100 );
	}
}

void BallInterception::freeze()
{

	state = FREEZE;
	if( visual->ballIsVisible() && visual->ball.dis > 1 )
	{
		start();
	}
	else
	{
		freeze_ball->call( command );
	}
}

FreezeBall::FreezeBall( GameData * game_data, WorldModel * world )
{
	this->game_data = game_data;
	this->world 	= world;

}

void
FreezeBall::call( AgentCommand * command )
{
	this->command = command;
	BodySensor  & body   = game_data->sensor_handler.last_sense;
	ServerParam & param  = game_data->game_parameter.server_param;
	double pow_needed;
	double angle_needed;

	Vector2D p 	  = world->me().pos; // Posición del agente
	double theta = world->me().angleDeg(); // Orientación del agente

	Vector2D vn   = world->estBallVelocity();

	Vector2D pv	  = Vector2D::fromPolar( body.speed_amount,
									   Deg2Rad( body.speed_direction  + theta ) );

	Vector2D pn_bn; // vector de bn a pn

	Vector2D pn = p + pv; //Posición del agente en el siguiente ciclo
	Vector2D bn = world->estBallPosition();

	Vector2D zero_vel(0.0, 0.0);

	pn_bn = pn-bn;

	double dis 		= pn_bn.mag();
	double dir_diff = std::abs( entre180( theta - Rad2Deg( pn_bn.angle() ) ) );
	double f = 1 - 0.25*(dir_diff/180.0) -0.25*(dis/param.kickable_margin);

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

	command->append_kick( pow_needed, angle_needed );

}




void searchBall( AgentCommand * command,
                 WorldModel * world,
                 SensorType sensor_type)
{
    double lastDirection = world->bitacoraBalon.begin()->dir;
    int    idCono        = world->me().view_mode_w;

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
        {
            command->append_turn(-cono);
            //command->append_turn(-15);
        }
        else
        {
            //command->append_turn(cono);
            command->append_turn( 60 );
        }
    }
}


void align(double neckDir,
            AgentCommand *command)
{
    if( neckDir!= 0.0 )
        command->append_turn_neck(-neckDir);
    else
        {
         // el cuello esta alineado con el cuerpo
         }
}



void porteroLine( double xTarget, Vector2D velocidad , SensorType sensor_type ,WorldModel *world,AgentCommand *command)
{
    Vector2D puntoReferencia;  //Coordenadas de la bandera del centro de la porteria
    double yTarget;

    puntoReferencia.x =-52.5;   // coordenadas de la bandera del centro de la porteria
    puntoReferencia.y = 0.0;

    if(  (world->time() - world->bitacoraBalon.begin()->ciclo) < 3 ) // si esta reciente los datos del balon
    {

        //xTarget = -48.0; // Linea donde quieres que se desplace el portero

        if(world->bitacoraBalon.begin()->pos.x != puntoReferencia.x)
            yTarget = ( (world->bitacoraBalon.begin()->pos.y - puntoReferencia.y) /
                        (world->bitacoraBalon.begin()->pos.x - puntoReferencia.x)
                        ) * (xTarget - puntoReferencia.x);
        else
            if(world->bitacoraBalon.begin()->pos.y <0.0)
                yTarget = -7.0;
            else
                yTarget = 7.0;


        if(yTarget >= 7.0 )
            yTarget = 7.0;

        if(yTarget <= -7.0)
            yTarget = -7.0;

        if( (world->me().pos.y > (yTarget - 0.5)) && (world->me().pos.y < (yTarget + 0.5)) )
            centerBall(world,command);
        else
            GoToXY2(xTarget, yTarget,0.5,velocidad,command,world); // utiliza dash() negativo
    }

    else  // no tengo datos recientes del balon, entonces lo busco
    {
        searchBall(command,world,sensor_type);
    }
}


void GoToXY2 (   double   xTarget ,
                 double   yTarget ,
                 double   radio,
                 Vector2D velocidad,
                 AgentCommand * command,
                 WorldModel * world
                 )
{

    double  disToPoint;
    double  dashParameter;
    double const dash_power_rate = 0.006, effort = 0.8;
    //double const inertia_moment = 5.0;

    double   x = world->me().pos.x;
    double   y = world->me().pos.y;
    double   angle = world->me().angle;
    //double   neck_dir = world->me().head_angle;

    //anguloCuerpo =  angle + angleCuelloCuerpo; // angleCuelloCuerpo se obtiene del sense_body
    //consideramos el angle como el angulo del cuerpo

    disToPoint = sqrt( (xTarget - x)*(xTarget - x)  + (yTarget - y)*(yTarget - y) );


    if( disToPoint > radio )  // el agente no ha llegado al punto
    {

        if( angle >= 0.0)
        {
            if( yTarget >= 0.0 )
            {
                if( angle < 100.0 && angle > 80.0 ) // orientacion correcta
                {
                    velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y - angle) );
                    dashParameter = (disToPoint - velocidad.x)  / ( dash_power_rate * effort );

                    if( dashParameter > 100.0)
                        dashParameter = 100.0;

                    if( y - yTarget < 0.0 )
                    {
                        command->append_dash( dashParameter );    // caso 1

                    }
                    else // yTarget < y
                    {
                        command->append_dash( -dashParameter );   // caso 2

                    }
                }
                else
                {
                    command->append_turn( 90 - angle  );
                    command->append_turn_neck( angle - 90);
                }
            }
            else  // yTarget < 0.0
            {
                if( angle > -100.0 && angle < -80.0 ) // orientacion correcta
                {
                    velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y - angle) );
                    dashParameter = (disToPoint - velocidad.x)  / ( dash_power_rate * effort );

                    if( dashParameter > 100.0)
                        dashParameter = 100.0;

                    if( y - yTarget < 0.0 )
                    {    command->append_dash( dashParameter );    // caso 3

                    }
                    else // yTarget < y
                    {
                        command->append_dash( -dashParameter );   // caso 4

                    }

                }
                else
                {
                    command->append_turn( -90 - angle );
                    command->append_turn_neck( 90 + angle );
                }
            }
        }
        else   // angle < 0.0
        {
            if( yTarget >= 0.0 )
            {
                if( angle < 100.0 && angle > 80.0 ) // orientacion correcta
                {
                    velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y - angle) );
                    dashParameter = (disToPoint - velocidad.x)  / ( dash_power_rate * effort );

                    if( dashParameter > 100.0)
                        dashParameter = 100.0;

                    if( y - yTarget > 0.0 )
                    {
                        command->append_dash( dashParameter );    // caso 5

                    }
                    else // yTarget < y
                    {
                        command->append_dash( -dashParameter );   // caso 6

                    }
                }
                else  // mal orientado
                {
                    command->append_turn( 90 - angle  );
                    command->append_turn_neck( angle - 90 );
                }
            }
            else  // yTarget < 0.0
            {
                if( angle > -100.0 && angle < -80.0 ) // orientacion correcta
                {
                    velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y - angle) );
                    dashParameter = (disToPoint - velocidad.x)  / ( dash_power_rate * effort );

                    if( dashParameter > 100.0)
                        dashParameter = 100.0;

                    if( y-yTarget > 0.0  )
                    {
                        command->append_dash( dashParameter );    // caso 7
                        //cout<<"caso 7"<<endl;
                    }
                    else // yTarget < y
                    {
                        command->append_dash( -dashParameter );   // caso 8
                        //cout<<"caso 8"<<endl;
                    }
                }
                else  // mal orientado
                {
                    command->append_turn( -90 - angle );
                    command->append_turn_neck( 90 + angle );
                }
            }
        }
    }
    else
    {
        // el agente llego al punto
    }

}

void centerBall( WorldModel *world,
                 AgentCommand * command)
{
    double precision = 10.0;

    if( world->bitacoraBalon.empty() )
    	return;

    if( world->bitacoraBalon.begin()->dir <=  precision &&
            world->bitacoraBalon.begin()->dir >= -precision  )
    {
        // Balon centrado en direccion al cuerpo
    }
    else
    {   // Si neck_dir es menor al maximo-ball.dir, puede girar el cuello o
        // si neck_dir es mayor al minimo-ball.dir, puede girar el cuello
        if( world->me().head_angle <  90.0 - world->bitacoraBalon.begin()->dir &&
                world->me().head_angle > -90.0 - world->bitacoraBalon.begin()->dir
                )
            command->append_turn_neck(world->bitacoraBalon.begin()->dir);
        else
        {
            // Si no, alineamos el cuello con el cuerpo y
            // luego giramos el cuerpo en ball.dir+neck_dir
            align(world->me().head_angle,command);
            command->append_turn(world->bitacoraBalon.begin()->dir +
                                 world->me().head_angle);
        }

    }
}


void GoToXY (   double   xTarget ,
		double   yTarget ,
		double   radio,
		Vector2D velocidad,
                AgentCommand * command,
                WorldModel * world
                )
{

    double  disToPoint;
    double  dirToPoint;
    double  angPermisible;
    double  turnParameter;
    double  dashParameter;
    double const dash_power_rate = 0.006, effort = 0.8;
    double const inertia_moment = 5.0;

    double   x = world->me().pos.x;
    double   y = world->me().pos.y;
    double   angle = world->me().angle;

    disToPoint = sqrt( (xTarget - x)*(xTarget - x)  + (yTarget - y)*(yTarget - y) );
    dirToPoint = Rad2Deg(atan2( yTarget - y , xTarget - x));

    if( disToPoint > radio )  // El agente no ha llegado al punto
    {

        angPermisible = Rad2Deg( atan2( radio , disToPoint ) );
        turnParameter = dirToPoint - angle;
        turnParameter = entre180( turnParameter );

        if( angPermisible < turnParameter * 0.1  ||  disToPoint > 25.0 )
            angPermisible = 25.0;

        if( fabs(turnParameter) > angPermisible )  // El agente no esta bien alineado al punto
        {
            velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y) );
            turnParameter =  turnParameter *(1.0 + (inertia_moment*velocidad.mag()));
            turnParameter = entre180(turnParameter);
            command->append_turn(turnParameter);
        }
        else
        {
            velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y - angle) );
            dashParameter = (disToPoint - velocidad.x)  / ( dash_power_rate * effort );

            if( dashParameter > 100.0)
                dashParameter = 100.0;

            command->append_dash( dashParameter );
        }
    }
    else
    {
        // El agente llego al punto
    }

}



void GoToXYSlow (   double   xTarget ,
		double   yTarget ,
		double   x,
		double   y,
		double   angle,
        double   neck_dir,
		double   radio,
		Vector2D velocidad,
		AgentCommand * command
                )
{

    double  disToPoint;
    double  dirToPoint;
    double  angPermisible;
    double  turnParameter;
    double  dashParameter;
    double const dash_power_rate = 0.006, effort = 0.8;
    double const inertia_moment = 5.0;

    //anguloCuerpo =  angle + angleCuelloCuerpo; // angleCuelloCuerpo se obtiene del sense_body
    //consideramos el angle como el angulo del cuerpo

    disToPoint = sqrt( (xTarget - x)*(xTarget - x)  + (yTarget - y)*(yTarget - y) );
    dirToPoint = Rad2Deg(atan2( yTarget - y , xTarget - x));

    if( disToPoint > radio )  // el agente no ha llegado al punto
    {

        angPermisible = Rad2Deg( atan2( radio , disToPoint ) );
        /*neck_dir?*/   turnParameter = dirToPoint - angle;// - neck_dir;
        turnParameter = entre180( turnParameter );

        if( angPermisible < turnParameter * 0.1  ||  disToPoint > 25.0 )
            angPermisible = 25.0;

        if( fabs(turnParameter) > angPermisible )  // el agente no esta bien alineado al punto
        {

            velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y) );
            turnParameter =  turnParameter *(1.0 + (inertia_moment*velocidad.mag()));
            turnParameter = entre180(turnParameter);
            //cout<<"Giro TURN"<<endl;
            command->append_turn(turnParameter);

        }
        else
        {

            velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y - angle) );
            dashParameter = (disToPoint - velocidad.x)  / ( dash_power_rate * effort );

            if( dashParameter > 100.0)
                dashParameter = 100.0;
            //cout<<"Avanza DASH"<<endl;
            command->append_dash( 70 );
        }
    }
    else
    {
        // el agente llego al punto
    }
}

void alignBodyWithNeck(WorldModel *world,
                       AgentCommand *command)
{
    if( world->me().head_angle != 0.0 )
       {
           command->append_turn(world->me().head_angle);
           command->append_turn_neck(-world->me().head_angle);
       }
}
