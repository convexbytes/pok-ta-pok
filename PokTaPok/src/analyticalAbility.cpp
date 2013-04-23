#include "analyticalAbility.h"
#include "gameData.h"
#include "worldModel.h"

#include <cstring>

#ifndef NDEF_NUM
#define NDEF_NUM -999999.0 // Fuera del rango de cualquier información provista por el servidor.
#endif

Vector2D
DePrimera( Vector2D const & vt1, // Velocidad deseada en el siguiente ciclo
		   Vector2D const & vt0, // velocidad del balón en el ciclo actual
		   double ball_dis, // Distancia al balón
		   double ball_dir_deg, //  Dirección del balón con respecto al cuerpo
		   double kickable_margin = 0.7,
		   double kick_power_rate = 0.027,
		   double ball_decay = 0.94)

{
	// kick_vector.x = power, kick_vector.y = direction
	Vector2D kick_vector;

	double dir_diff = std::fabs( ball_dir_deg );

	double f = (1 - 0.25*(dir_diff/180.0) -0.25*(ball_dis/kickable_margin) );

	double pow_needed;
	double angle_needed;

	if( vt1.x == 0.0 && vt1.y == 0.0 && vt0.x == 0.0 && vt0.y == 0.0)
	{
		pow_needed = 0.0;
		angle_needed = 0.0;
	}
	else
	{
		pow_needed = (vt1/ball_decay - vt0).normita() / ( kick_power_rate * f ) ;
		angle_needed = std::atan2( vt1.y/ball_decay - vt0.y, vt1.x/ball_decay - vt0.x );
	}

	kick_vector.x = pow_needed;
	kick_vector.y = Rad2Deg( angle_needed );

	return kick_vector;

}

Vector2D
velToInterceptBall( Vector2D const & b, // Posición del balón
		Vector2D const & p, // Posición del jugador
		Vector2D const & v, // Velocidad del balón
		int t,
		double ball_decay = 0.94 // constante de decaimiento
)
{
	// Regresa la velocidad en x y en y necesarias para interceptar el baĺón en el tiempo t

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
	double kick_power_rate = 0.027,kickable_margin = 0.7;
	double act_kpr = kick_power_rate * (( 1.0 - ((0.25 * dir) / 180.0)) - ((0.25 * dist) / kickable_margin));

	return act_kpr;
}

double KickPowerForSpeed(double speed,double actkpr)
{
	double kickPower = speed / actkpr;

	return kickPower;
}

double KickSpeedToTravel(double d,double e)
{
	if( e == 0 )
	{
		e = 0.01;
	}
	double ball_decay = 0.94;
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
            turnParameter =  turnParameter *(1.0 + (inertia_moment*velocidad.normita()));
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
                  WorldModelV1 * world,
                  AgentCommand * command
                  )
{
    double angToPoint;
    double auxX,auxY;
    double x,y,angle,neck_dir;
    Vector2D Kick;

    x=world->me.pos.x;
    y=world->me.pos.y;
    angle=world->me.angle;
    neck_dir = world->me.head_angle;


    if (disBall <= radio)
    {
        angToPoint = Rad2Deg( atan2(  yTarget   - y,   xTarget   - x) )- angle;

        /*Este 2.0 es similar a lo que queremos que adelante la pelota
        para valores grandes la adelanta mucho*/

        auxX = 2.0 * cos( Deg2Rad( angToPoint ) );
        auxY = 2.0 * sin( Deg2Rad( angToPoint ) );

        Kick = PasePunto(x+auxX,y+auxY,x,y,angle,0.0, xBall , yBall);

        Kick.y = entre180(angToPoint);

        //// cout<<"Adelanto KICK"<<endl;
        command->append_kick(Kick.x,Kick.y);
    }
    else
    {
        GoToXY(xBall,yBall,x,y,angle,neck_dir,radio,velocidad,command );  // vamos por la pelota
    }
}


bool  isZoneShoot( double x, double y , double radio)
{
	double aux;
	bool   regreso;

	aux = sqrt(((x - 52.0)*(x - 52.0)) + (y*y));

	if( aux <= radio )
	{
		regreso = true;
	}
	else
		regreso = false;

	return regreso;
}

bool posteVisible ( vector<Flag> *banderas )
{
	bool regreso;
	int  i;

	for( i=0; i<banderas->size(); i++ )
	{
		if( banderas->at(i).id == 40 || banderas->at(i).id == 42  ) //id's de los postes
		{
			regreso = true;
			i=banderas->size();
		}
		else
		{
			if( i == banderas->size()-1 )
				regreso = false;
		}
	}
	return regreso;
}

double shootAtGoal(double x, double y, double angle, vector<Flag> *banderas )
{
	double regreso  ;
	int    i;
	bool   post1,post2;
	Vector2D disToPosts;  //disToPosts.x = distancia a grt
	//disToPosts.y = distancia a grb
	Vector2D dirToPosts;  //dirToPosts.x = dirección a grt
	//dirToPosts.y = dirección a grb

	post1 = post2 = false;
	for( i=0; i< banderas->size(); i++ )
	{
		if( banderas->at(i).id == 40 )   // es bandera GRT
		{
			disToPosts.x = banderas->at(i).dis;
			dirToPosts.x = banderas->at(i).dir;
			post1       = true;
		}

		if( banderas->at(i).id == 42 )   // es bandera GRB
		{
			disToPosts.y = banderas->at(i).dis;
			dirToPosts.y = banderas->at(i).dir;
			post2        = true;
		}
	}

	if( post1 == true && post2== true ) // ve los dos postes
	{
		if( disToPosts.x > disToPosts.y ) // compara que poste esta más cerca
		{
			regreso = dirToPosts.y - 2.0;          // poste 1 más lejos
		}
		else
		{
			regreso = dirToPosts.x + 2.0;          // poste 2 más lejos
		}
	}
	else                              // ve solo un poste
	{
		if( post1 == true)             // ve el poste 1
		{
			regreso = dirToPosts.x + 2.0;
		}
		else                           // ve el poste 2
		{
			regreso = dirToPosts.y -2.0;
		}
	}
	return regreso;
}

BallInterception::BallInterception( GameData 	   * game_data,
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
	this->last_call_time= NDEF_NUM;
	time_to_reach_s = NDEF_NUM;
	reset();
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
	if( world->time - last_call_time > 1 )
		state = START;

	last_call_time = world->time;
	//
	// Espacio para considerar el evento
	// on pasive interception position
	//
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
	case PASIVE_INTERCEPTION:
		pasiveInterception();
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
		command->append_turn( 60 );
	}
}

void
BallInterception::computePointTurn()
{
	state = COMPUTE_POINT_TURN;
	double const PERMISIVE_ANGLE = 10;

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

		ball_pos_s 	   = world->me.pos
				+ Vector2D::fromPolar( visual->ball.dis,
						Deg2Rad( visual->ball.dir + world->me.angleDeg() + world->me.headAngleDeg() ) );
		player_pos_s   = world->me.pos;

		start_time_s   = world->time;

		player_vel_s   = Vector2D::fromPolar(  body->speed_amount,
				Deg2Rad(body->speed_direction + world->me.angle + world->me.head_angle ) );

		if( visual->ball.dir_chg != NDEF_NUM && visual->ball.dis_chg != NDEF_NUM )
		{
			ball_vel_s = fromChgToVel(  player_pos_s, // Posición del jugador, absoluta.
					player_vel_s, // Velocidad del jugador, absoluta.
					ball_pos_s, // Posición del objeto que ve, absoluta.
					visual->ball.dis,
					visual->ball.dis_chg,
					visual->ball.dir_chg// + world->me.headAngleDeg()
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
		while( vel_needed_s.normita() >= AVERAGE_VEL );

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
				entre180( angle_deg - world->me.angleDeg() /* - world->me.headAngleDeg() */)
				* (1.0 + param->inertia_moment * body->speed_amount ) ;

		// if param too slow.....
		command->append_turn( turn_param );

		turn_requested = true;
	}
}

void
BallInterception::go()
{
	state = GO;
	bool point_reached =
			(world->me.pos - p_mid_s).normita() <= 1.0;
	double const DIR_CHANGE_THRESHOLD = 5;
	if( visual->ball_is_visible && visual->ball.dis < 1)
	{
		freeze();
	}
	else if(    (world->time - start_time_s) > time_to_reach_s
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

void BallInterception::pasiveInterception()
{
	state = PASIVE_INTERCEPTION;
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

PossessionBall whoHasTheBall(WorldModelV1 *world)
{
    int            i;
    bool           posesion=false;
    double         x,y,angle,neck_dir;
    double         disToBall;
    double         disPermisible;

    Vector2D       poseAgente,balon;

    PossessionBall regreso;

    x = world->me.pos.x;
    y = world->me.pos.y;
    angle = world->me.angle;
    neck_dir = world->me.head_angle;

    disPermisible = 2.0;

    if( world->bitacoraBalon.size() != 0         &&
            (world->time - world->bitacoraBalon.begin()->ciclo) <= 5 )
    {
        balon.x = world->bitacoraBalon.begin()->pos.x;
        balon.y = world->bitacoraBalon.begin()->pos.y;

        if(  world->bitacoraBalon.begin()->dis <= disPermisible )
        {
            regreso = PROPIA;
            posesion = true;
            //cout<<world->time<<" PROPIA"<<endl;
        }

        //pruebo para los amigos
        for( i=0 ; i<11 && posesion == false ; i++ )
        {
            if( world->bitacoraAmigos[i].size()!= 0 )
            {
                poseAgente.x = world->bitacoraAmigos[i].begin()->pos.x;
                poseAgente.y = world->bitacoraAmigos[i].begin()->pos.y;

                disToBall = sqrt((balon.x-poseAgente.x)*(balon.x-poseAgente.x) +
                                 (balon.y-poseAgente.y)*(balon.y-poseAgente.y)
                                 );

                if( disToBall <= disPermisible )
                {
                    regreso = EQUIPO;
                    posesion = true;
                    //cout<<world->time<<" EQUIPO "<<endl;
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
                    //cout<<world->time<<" RIVAL "<<endl;
                    i=11;
                }
            }
        }

        if( posesion == false )
        {
            regreso = SUELTA;
            //cout<<world->time<<" SUELTA "<<endl;
        }
    }
    else
    {
        regreso = PERDIDA;
        //cout<<world->time<<" PERDIDA "<<endl;
    }
    return regreso;
}


void searchBall( AgentCommand * command,
                 WorldModelV1 * world,
                 SensorType sensor_type)
{
    double lastDirection = world->bitacoraBalon.begin()->dir;
    int    idCono        = world->me.view_mode_w;

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
            command->append_turn(- (cono-1.0 ));
            //command->append_turn(-15);
        }
        else
        {
            command->append_turn(cono - 1.0);
            //command->append_turn( 60 );
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

bool amTheClosest( WorldModelV1 *world )

{
    int      i,k,friends;
    bool     regreso;
    double   disToBall;
    Vector2D balon;
    Vector2D poseAgente;

    double x =  world->me.pos.x;
    double y =  world->me.pos.y;
    double angle =  world->me.angle;
    double neck_dir =  world->me.head_angle;

    balon.x = world->bitacoraBalon.begin()->pos.x;
    balon.y = world->bitacoraBalon.begin()->pos.y;

    k=0;
    friends = 0;

    for( i = 1; i<11; i++ ) // no toma en cuenta al portero
    {
        if( world->bitacoraAmigos[i].size() != 0 )
        {
            friends++;

            poseAgente.x = world->bitacoraAmigos[i].begin()->pos.x;
            poseAgente.y = world->bitacoraAmigos[i].begin()->pos.y;

            disToBall = sqrt((poseAgente.x-balon.x)*(poseAgente.x-balon.x) +
                             (poseAgente.y-balon.y)*(poseAgente.y-balon.y)
                             );

            if( (world->time - world->bitacoraAmigos[i].begin()->ciclo) >= 10 ||
                    world->bitacoraBalon.begin()->dis < disToBall)
                k++;
        }
    }

    if( friends == 0 || ( friends!=0 && friends==k  )  )
    {
        regreso = true;
    }
    else
        regreso = false;

    return regreso;
}

void radar(double neck_dir,
           AgentCommand * command,
           SensorType sensor_type)
{
    static bool aux;
    double aumento = 40.0;

    if( neck_dir == 90  || (neck_dir + aumento) >= 90.0 )
        aux=true;

    if( neck_dir == -90 || (neck_dir - aumento) <= -90.0 )
        aux=false;


    if( neck_dir < 90.0 && aux == false && sensor_type== SENSOR_SEE )
    {
        command->append_turn_neck( aumento );
    }

    if( neck_dir >-90.0 && aux == true && sensor_type== SENSOR_SEE )
    {
        command->append_turn_neck( -aumento );
    }
}


int aQuienPasar(WorldModelV1 *world)
{
    int i;
    int indice;
    double min = 9999.9999;

    indice = -1;

    for(i=0;i<11;i++)
    {
        if( world->bitacoraAmigos[i].size()!=0 &&
           (world->time - world->bitacoraAmigos[i].begin()->ciclo) < 3 &&
           (world->me.unum - 1) != i
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


void porteroLine( double xTarget,
		          Vector2D velocidad,
		          SensorType sensor_type,
		          WorldModelV1 *world,
		          AgentCommand *command)
{
    Vector2D puntoReferencia;  //Coordenadas de la bandera del centro de la porteria
    double yTarget;

    if( world->me.side == 'l')
        puntoReferencia.x =-52.5;   // coordenadas de la bandera del centro de la porteria
    else
    	puntoReferencia.x = 52.5;

    puntoReferencia.y = 0.0;

    if(  (world->time - world->bitacoraBalon.begin()->ciclo) < 3 ) // si esta reciente los datos del balon
    {

        //xTarget = -48.0; // Linea donde quieres que se desplace el portero

        if( world->bitacoraBalon.begin()->pos.x < puntoReferencia.x + 0.5 &&
            world->bitacoraBalon.begin()->pos.x > puntoReferencia.x - 0.5 ||
            world->bitacoraBalon.begin()->pos.x < -53.0 ||
            world->bitacoraBalon.begin()->pos.x > 53.0 )
           yTarget=0.0;
        else
        	yTarget = ( (world->bitacoraBalon.begin()->pos.y - puntoReferencia.y) /
        	                        (world->bitacoraBalon.begin()->pos.x - puntoReferencia.x)
        	                        ) * (xTarget - puntoReferencia.x);



        if(yTarget >= 7.0 )
            yTarget = 7.0;

        if(yTarget <= -7.0)
            yTarget = -7.0;

        if( (world->me.pos.y > (yTarget - 0.5)) && (world->me.pos.y < (yTarget + 0.5)) )
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
                 WorldModelV1 * world
                 )
{

    double  disToPoint;
    double  dirToPoint;
    double  dashParameter;
    double const dash_power_rate = 0.006, effort = 0.8;
    //double const inertia_moment = 5.0;

    double   x = world->me.pos.x;
    double   y = world->me.pos.y;
    double   angle = world->me.angle;
    //double   neck_dir = world->me.head_angle;

    //anguloCuerpo =  angle + angleCuelloCuerpo; // angleCuelloCuerpo se obtiene del sense_body
    //consideramos el angle como el angulo del cuerpo

    disToPoint = sqrt( (xTarget - x)*(xTarget - x)  + (yTarget - y)*(yTarget - y) );
    dirToPoint = Rad2Deg(atan2( yTarget - y , xTarget - x));


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

// centra el balón en el cono de visión utilizando el turn
void centerBall(bool   ballVisible,
                double ballDir,
                double lastAngleBall,
                double neckDir,
                ViewModeWidth cono,
                AgentCommand * command)
{
           if( ballDir >= -10.0 && ballDir <= 10.0 )
              {
                  // Balon esta centrado en el cono de vision
              }
           else
              {
                  align(neckDir,command);
                  command->append_turn(ballDir);
              }

}
// centra el balon en el cono de vision utilizando el turn_neck
void centerBall( WorldModelV1 *world,
                 AgentCommand * command)
{
    double precision = 10.0;

    if( world->bitacoraBalon.begin()->dir <=  precision &&
            world->bitacoraBalon.begin()->dir >= -precision  )
    {
        // Balon centrado en direccion al cuerpo
    }
    else
    {   // Si neck_dir es menor al maximo-ball.dir, puede girar el cuello o
        // si neck_dir es mayor al minimo-ball.dir, puede girar el cuello
        if( world->me.head_angle <  90.0 - world->bitacoraBalon.begin()->dir &&
                world->me.head_angle > -90.0 - world->bitacoraBalon.begin()->dir
                )
            command->append_turn_neck(world->bitacoraBalon.begin()->dir);
        else
        {
            // Si no, alineamos el cuello con el cuerpo y
            // luego giramos el cuerpo en ball.dir+neck_dir
            align(world->me.head_angle,command);
            command->append_turn(world->bitacoraBalon.begin()->dir +
                                 world->me.head_angle);
        }

    }
}


void GoToXY (   double   xTarget ,
		double   yTarget ,
		double   radio,
		Vector2D velocidad,
                AgentCommand * command,
                WorldModelV1 * world
                )
{

    double  disToPoint;
    double  dirToPoint;
    double  angPermisible;
    double  turnParameter;
    double  dashParameter;
    double const dash_power_rate = 0.006, effort = 0.8;
    double const inertia_moment = 5.0;

    double   x = world->me.pos.x;
    double   y = world->me.pos.y;
    double   angle = world->me.angle;
    double   neck_dir = world->me.head_angle;

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
            turnParameter =  turnParameter *(1.0 + (inertia_moment*velocidad.normita()));
            turnParameter = entre180(turnParameter);
            //// cout<<"Giro TURN"<<endl;
            command->append_turn(turnParameter);

        }
        else
        {

            velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y - angle) );
            dashParameter = (disToPoint - velocidad.x)  / ( dash_power_rate * effort );

            if( dashParameter > 100.0)
                dashParameter = 100.0;
            //// cout<<"Avanza DASH"<<endl;
            command->append_dash( dashParameter );
        }
    }
    else
    {
        // el agente llego al punto
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
            turnParameter =  turnParameter *(1.0 + (inertia_moment*velocidad.normita()));
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



// Convierte a numeros dobles el mensaje recibido, es decir mensaje = x,y,prob
float * convertToDouble( string mensaje )
{
    const char   *subcadena;
    int x,y;
    float prob;
    bool error;

    static float arreglo[3];

    subcadena = mensaje.c_str();

    if( *subcadena == '+' )
    {
        subcadena++;
        sscanf(subcadena,"%d",&x);   //obtenemos el 1er dato (+,x)

        if( isdigit(*(subcadena+1) ) )
            subcadena+=2;
        else
            subcadena++;

        if( *subcadena == '+' )
        {
            subcadena++;
            sscanf(subcadena,"%d",&y);   //obtenemos el 2do dato (+,y)

            if( isdigit(*(subcadena+1) ) )
                subcadena+=2;
            else
                subcadena++;

            if(*subcadena == '.')
               sscanf(subcadena,"%f",&prob);  //obtenemos el 3er dato (prob)
            else
                error = true;
        }
        else
        {
            if( *subcadena == '-')
            {
                sscanf(subcadena,"%d",&y); //obtenemos el 2do dato (-,y)
                subcadena++;  // brinco el signo

                if( isdigit(*(subcadena+1) ) )
                    subcadena+=2;
                else
                    subcadena++;

                if(*subcadena == '.')
                    sscanf(subcadena,"%f",&prob);  //obtenemos el 3er dato (prob)
                else
                    error = true;
            }
            else
            {
                // ERROR, mensaje fuera de formato
                error = true;
            }
        }
    }
    else  // el primer dato no fue positivo
    {
        if( *subcadena == '-')
        {
            sscanf(subcadena,"%d",&x);         //obtenemos el 1er dato (-,x)

            subcadena++;  // me brinco el signo

            if( isdigit(*(subcadena+1) ) )
                subcadena+=2;
            else
                subcadena++;

            if( *subcadena == '+' )
            {
                subcadena++;
                sscanf(subcadena,"%d",&y);   //obtenemos el 2do dato (+,y)

                if( isdigit(*(subcadena+1) ) )
                    subcadena+=2;
                else
                    subcadena++;

                if(*subcadena == '.')
                    sscanf(subcadena,"%f",&prob);  //obtenemos el 3er dato (prob)
                else
                    error = true;
            }
            else
            {
                if( *subcadena == '-')
                {
                    sscanf(subcadena,"%d",&y); //obtenemos el 2do dato (-,y)

                    subcadena++;  // me brinco el signo

                    if( isdigit(*(subcadena+1) ) )
                        subcadena+=2;
                    else
                        subcadena++;

                    if(*subcadena == '.')
                        sscanf(subcadena,"%f",&prob);  //obtenemos el 3er dato (prob)
                    else
                        error = true;
                }
                else
                {
                    // ERROR, mensaje fuera de formato
                    error = true;
                }
            }
        }
        else
        {
            // ERROR, mensaje fuera de formato
            error = true;
        }
    }

    if(error != true )
    {
        arreglo[0] = x;
        arreglo[1] = y;
        arreglo[2] = prob;
        cout<<"Convertimos a valores: "<<x<<" "<<y<<" "<<prob<<endl;
        cout<<"Enviados en el arreglo: "<<arreglo[0]<<" "<<arreglo[1]<<" "<<arreglo[2]<<endl;
    }

    return arreglo;
}

// Convierte las coordenadas del punto objetivo y la probabilidad en cadena
string convertToString(Vector2D pointTarget , float prob)
{
    int x,y;
    char cad1[5],cad2[5],cad3[5],cad4[5],cad5[5];
    char mensaje[10];

    for(int i=0 ; i<10 ; i++)
    {
        mensaje[i]='\0';
    }

    x= (int)pointTarget.x;
    y= (int)pointTarget.y;

    if( x<0 )
    {
        sprintf(cad1,"%d",x);  // obtengo el primer dato negativo

        if( isdigit(cad1[2]) )  // veo si es de uno o dos digitos
            cad1[3]='\0';
        else
            cad1[2]='\0';

        if( y<0 )  // Y negativo
        {
            sprintf(cad2,"%d",y);  // obtengo el segundo dato

            if( isdigit(cad2[2]) )  // veo si es de uno o dos digitos
                cad2[3]='\0';
            else
                cad2[2]='\0';

            sprintf(cad3,"%f",prob);   // obtengo la probabilidad

            cad3[0]=cad3[1];
            cad3[1]=cad3[2];
            cad3[2]=cad3[3];
            cad3[3]=cad3[4];
            cad3[4]='\0';                // quito el caracter 0.

            strcat(mensaje,cad1);
            strcat(mensaje,cad2);
            strncat(mensaje,cad3,10);
        }  // termina el if del segundo dato negativo
        else
        {
            if( y < 0 )
            {
                cad2[0]='+';
                cad2[1]='\0';
                sprintf(cad3,"%d",y);

                if( isdigit(cad3[2]) )
                    cad3[3]='\0';
                else
                    cad3[2]='\0';

                sprintf(cad4,"%f",prob);

                cad4[0]=cad4[1];
                cad4[1]=cad4[2];
                cad4[2]=cad4[3];
                cad4[3]=cad4[4];
                cad4[4]='\0';

                strcat(mensaje,cad1);
                strcat(mensaje,cad2);
                strcat(mensaje,cad3);
                strncat(mensaje,cad4,10);
            }  // termina else del segundo dato positivo
            else  // Y = 0
            {
                strcpy(cad2,"+00");
                sprintf(cad3,"%f",prob);

                cad3[0]=cad3[1];
                cad3[1]=cad3[2];
                cad3[2]=cad3[3];
                cad3[3]=cad3[4];
                cad3[4]='\0';

                strcat(mensaje,cad1);
                strcat(mensaje,cad2);
                strcat(mensaje,cad3);
            }  // termina else del segundo dato = 0
        }
    }// termina el if del primer dato positivo
    else  // X > 0   o   X = 0
    {
        if( x>0 )
        {
            cad1[0] ='+';
            cad1[1] ='\0';

            sprintf(cad2,"%d",x);

            if( isdigit(cad2[1]) )
                cad2[2]='\0';
            else
                cad2[1]='\0';

            if( y < 0 )
            {
                sprintf(cad3,"%d",y);

                if( isdigit(cad3[2]) )
                    cad3[3]='\0';
                else
                    cad3[2]='\0';

                sprintf(cad4,"%f",prob);

                cad4[0]=cad4[1];
                cad4[1]=cad4[2];
                cad4[2]=cad4[3];
                cad4[3]=cad4[4];
                cad4[4]='\0';

                strcat(mensaje,cad1);
                strcat(mensaje,cad2);
                strcat(mensaje,cad3);
                strncat(mensaje,cad4,10);
            }
            else  // Y > 0
            {
                if( y > 0)
                {
                    cad3[0]='+';
                    cad3[1]='\0';

                    sprintf(cad4,"%d",y);

                    if( isdigit(cad4[1]) )
                        cad4[2]='\0';
                    else
                        cad4[1]='\0';

                    sprintf(cad5,"%f",prob);

                    cad5[0]=cad5[1];
                    cad5[1]=cad5[2];
                    cad5[2]=cad5[3];
                    cad5[3]=cad5[4];
                    cad5[4]='\0';

                    strcat(mensaje,cad1);
                    strcat(mensaje,cad2);
                    strcat(mensaje,cad3);
                    strcat(mensaje,cad4);
                    strncat(mensaje,cad5,10);
                }
                else // Y = 0
                {
                    strcpy(cad3,"+00");
                    sprintf(cad4,"%f",prob);

                    cad4[0]=cad4[1];
                    cad4[1]=cad4[2];
                    cad4[2]=cad4[3];
                    cad4[3]=cad4[4];
                    cad4[4]='\0';

                    strcat(mensaje,cad1);
                    strcat(mensaje,cad2);
                    strcat(mensaje,cad3);
                    strncat(mensaje,cad4,10);

                }
            }
        }
        else  // X = 0
        {
            strcpy(cad1,"+00");
            if( y<0 )
            {
                sprintf(cad2,"%d",y);

                if( isdigit(cad2[2]) )
                    cad2[3]='\0';
                else
                    cad2[2]='\0';

                sprintf(cad3,"%f",prob);

                cad3[0]=cad3[1];
                cad3[1]=cad3[2];
                cad3[2]=cad3[3];
                cad3[3]=cad3[4];
                cad3[4]='\0';

                strcat(mensaje,cad1);
                strcat(mensaje,cad2);
                strncat(mensaje,cad3,10);

            }
            else
            {
                if( y>0 )
                {
                    cad2[0]='+';
                    cad2[1]='\0';

                    sprintf(cad3,"%d",y);

                    if( isdigit(cad3[1]) )
                        cad3[2]='\0';
                    else
                        cad3[1]='\0';

                    sprintf(cad4,"%f",prob);

                    cad4[0]=cad4[1];
                    cad4[1]=cad4[2];
                    cad4[2]=cad4[3];
                    cad4[3]=cad4[4];
                    cad4[4]='\0';

                    strcat(mensaje,cad1);
                    strcat(mensaje,cad2);
                    strcat(mensaje,cad3);
                    strncat(mensaje,cad4,10);

                }
                else // Y = 0
                {
                    strcpy(cad2,"+00");
                    sprintf(cad3,"%f",prob);

                    cad3[0]=cad3[1];
                    cad3[1]=cad3[2];
                    cad3[2]=cad3[3];
                    cad3[3]=cad3[4];
                    cad3[4]='\0';

                    strcat(mensaje,cad1);
                    strcat(mensaje,cad2);
                    strncat(mensaje,cad3,10);
                }
            }
        }
    }

    cout<<"Mensaje Final: "<<mensaje<<endl;
    return mensaje;
}

// Devuelve el unum del agente amigo al cual debe de oir
// Elige al agente más cercano al objetivo que tiene destinado
int chooseFriendToHear(Vector2D pointTarget,WorldModelV1 * world)
{
    double dist_min;
    double distToObjetive;
    int    closestAgent,i;

    closestAgent = -1;
    dist_min = 9999.9999;

    for( i=0 ; i<11 ; i++ )
    {
        if(  world->bitacoraAmigos[i].size()!= 0 &&
            (world->time - world->bitacoraAmigos[i].begin()->ciclo) < 2 )
        {
            distToObjetive = sqrt( (pointTarget.x - world->bitacoraAmigos[i].begin()->pos.x)*(pointTarget.x - world->bitacoraAmigos[i].begin()->pos.x) +
                                   (pointTarget.y - world->bitacoraAmigos[i].begin()->pos.y)*(pointTarget.y - world->bitacoraAmigos[i].begin()->pos.y)
                                   );

            if( distToObjetive < dist_min )
            {
                closestAgent = i+1;
                dist_min = distToObjetive;
            }
        }
    }

    return closestAgent;
}

// Recibe como parametros las coordenadas de su objetivo y la probabilidad de
// realizarlo, convierte a cadena esos datos y comunica el mansaje con el comando SAY
void comunicarObjetivo(Vector2D pointTarget,
                       float   prob,
                       WorldModelV1 *world,
                       AgentCommand *command )
{
    string mensaje;
    mensaje = convertToString(pointTarget,prob);
    command->append_say(mensaje);
    cout<<"Grito la cadena: "<<mensaje<<endl;
}

// Elige que agente escuchar, y aplica el comando attentionto para oir el mensaje
bool escucharObjetivo ( GameData 	   * game_data,
                       Vector2D pointTarget,
                       WorldModelV1 *world,
                       AgentCommand *command)
{
    int amigo;
    bool regreso;

    amigo = chooseFriendToHear(pointTarget,world);

    if( amigo != -1 )
    {
        command->append_attentionto( ATTENTION_OUR , amigo );
        regreso = true;
        cout<<"Escucho al agente con el numero: "<<amigo<<endl;
        cout<<"Que envio: "<< game_data->sensor_handler.last_hear_our.message<<endl;
    }
    else
    {
        cout<<"No ve ningun agente...."<<endl;
        regreso = false;
    }
    return regreso;
}

// Regresa un false, si el objetivo es similar al del agente amigo y si su probabilidad es menor
bool compararObjetivo( GameData * game_data,
		               Vector2D miPunto, float miProb, WorldModelV1 *world)
{
    float * p;
    float x,y,prob;
    bool   regreso;


    p = convertToDouble( game_data->sensor_handler.last_hear_our.message );

    x    = p[0];
    y    = p[1];
    prob = p[2];

    cout<<"Comparo el objetivo mio con: "<<x<<" "<<y<<" "<<prob<<endl;

    if( (((x + 1.0) <= (miPunto.x + 1.0) && (x + 1.0) >= (miPunto.x - 1.0)) ||
         ((x - 1.0) <= (miPunto.x + 1.0) && (x - 1.0) >= (miPunto.x - 1.0)) )
            &&
            (((y + 1.0) <= (miPunto.y + 1.0) && (y + 1.0) >= (miPunto.y - 1.0)) ||
             ((y - 1.0) <= (miPunto.y + 1.0) && (y - 1.0) >= (miPunto.y - 1.0)) )
            &&
            miProb < prob )

    {
        // Las zonas son similares y es mas probable que cumpla el objetivo el
        // agente amigo
        regreso = false;
    }
    else
    {
        // miPunto es bueno, eligo realizar objetivo
        regreso = true;
    }

    return regreso;
}


bool amStayInLine( WorldModelV1 * world )
{
    bool regreso;
    double xTarget;

    regreso = false;

    if( world->me.side == 'l' )
        xTarget = -48.0;
    else
        xTarget = 48.0;

    if(world->me.pos.x < xTarget + 1.0 && world->me.pos.x > xTarget - 1.0)
        regreso = true;

    return regreso;
}

void alignBodyWithNeck(WorldModelV1 *world,
                       AgentCommand *command)
{
    if( world->me.head_angle != 0.0 )
       {
           command->append_turn(world->me.head_angle);
           command->append_turn_neck(-world->me.head_angle);
       }
}

bool balonEnAreaGrande( WorldModelV1 *world )
{
    bool regreso;

    regreso = false;

    if( world->me.side == 'l')
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
