#include "analyticalAbility.h"
#include "gameData.h"
#include "worldModel.h"

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

        if( angPermisible < turnParameter * 0.1  &&  disToPoint > 15.0 )
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
                  double   x,
                  double   y,
                  double   angle,
                  double   xBall,
                  double   yBall,
                  double   disBall,
                  Vector2D velocidad,
                  double   neck_dir,
                  double   radio,
                  AgentCommand * command
                  )
{
    double angToPoint;
    double auxX,auxY;
    Vector2D Kick;


    if (disBall <= radio)
    {
        angToPoint = Rad2Deg( atan2(  yTarget   - y,   xTarget   - x) )- angle;

        /*Este 2.0 es similar a lo que queremos que adelante la pelota
        para valores grandes la adelanta mucho*/

        auxX = 2.0 * cos( Deg2Rad( angToPoint ) );
        auxY = 2.0 * sin( Deg2Rad( angToPoint ) );

        Kick = PasePunto(x+auxX,y+auxY,x,y,angle,0.0, xBall , yBall );

        Kick.y = entre180(angToPoint);

        //cout<<"Adelanto KICK"<<endl;
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

		command->append_turn( turn_param );

		turn_requested = true;
	}
}

void
BallInterception::go()
{
	state = GO;
	bool point_reached =
			(world->me.pos - p_mid_s).normita() <= 0.7;
	if( visual->ball_is_visible && visual->ball.dis < 1)
	{
		freeze();
	}
	else if(    (world->time - start_time_s-2 > time_to_reach_s)
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

PossessionBall whoHasTheBall(bool   ballVisible,
		double ballDis,
		double ballDir,
		double x ,
		double y ,
		double angle ,
		double neck_dir,
		vector<Player> *agentes)
{
	int            i;
	bool           posesion;
	double         disToBall;
	double         disPermisible;
	Vector2D       balon;
	Vector2D       poseAgente;

	PossessionBall regreso;

	posesion      = false;
	disPermisible = 1.0;

	if( ballVisible )
	{
		if( ballDis <= disPermisible )
		{
			// cout<<"PROPIA....*****"<<endl;
			regreso = PROPIA;
		}
		else
		{
			//Obtenemos las coordenadas del balon
			balon = Vector2D::fromPolar(ballDis,Deg2Rad(ballDir + angle + neck_dir));

			balon.x += x;
			balon.y += y;

			//Obtenemos las coordenadas de todos los agentes visibles
			for( i=0; i< agentes->size();i++ )
			{
				poseAgente = Vector2D::fromPolar(agentes->at(i).dis,
						Deg2Rad(agentes->at(i).dir
								+angle
								+neck_dir)
				);
				poseAgente.x += x;
				poseAgente.y += y;

				disToBall = sqrt((balon.x-poseAgente.x)*(balon.x-poseAgente.x) +
						(balon.y-poseAgente.y)*(balon.y-poseAgente.y)
				);

				if( agentes->at(i).team.compare("PokTaPok")==0 && disToBall<=disPermisible)
				{   // cout<<"EQUIPO"<<endl;
				regreso =  EQUIPO;
				i = agentes->size();
				posesion = true;
				}
				else
					if( agentes->at(i).team.compare("")==0 && disToBall<=disPermisible)
					{   // cout<<"DESCONOCIDA"<<endl;
					regreso =  DESCONOCIDA;
					posesion = true;
					i = agentes->size();
					}
					else
						if( agentes->at(i).team.compare("PokTaPok")!=0 && disToBall<=disPermisible)
						{   // cout<<"RIVAL"<<endl;
							regreso = RIVAL;
							i = agentes->size();
							posesion = true;
						}
			}

			if( posesion == false )
			{// cout<<"SUELTA"<<endl;
				regreso = SUELTA;
			}
		}
	}
	else
	{   // cout<<"PERDIDA"<<endl;
		regreso = PERDIDA;
	}

	return regreso;
}

void searchBall(double lastDirection,
               int idCono,
               AgentCommand * command )
{
   double cono;

   switch(idCono)
   {
   case NARROW: cono = 60.0;  break;
   case NORMAL: cono = 90.0; break;
   case WIDE:   cono = 180.0; break;
   default:     cono = 0.0;   break; // incorrecto, error
   }

   if( lastDirection < 0.0 )
       command->append_turn(-cono);
   else
       command->append_turn(cono);
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

bool amTheClosest( double ballDis,
                   double ballDir,
                   double x ,
                   double y ,
                   double angle ,
                   double neck_dir,
                   vector<Player> *agentes)
{
    int      i,k,friends;
    bool     regreso;
    double   disToBall;
    Vector2D balon;
    Vector2D poseAgente;


    balon = Vector2D::fromPolar(ballDis,Deg2Rad(ballDir + angle + neck_dir));

    balon.x += x;
    balon.y += y;

    k=0;
    friends =0;

    for( i=0;i < agentes->size();i++ )
           if( agentes->at(i).team.compare("PokTaPok")==0 )
              friends++;

    for( i=0;i < agentes->size();i++ )
        {
         poseAgente = Vector2D::fromPolar(agentes->at(i).dis,
                                          Deg2Rad(agentes->at(i).dir
                                                  +angle
                                                  +neck_dir)
                                          );
         poseAgente.x += x;
         poseAgente.y += y;

         disToBall = sqrt((balon.x-poseAgente.x)*(balon.x-poseAgente.x) +
                          (balon.y-poseAgente.y)*(balon.y-poseAgente.y)
                         );

         if( agentes->at(i).team.compare("PokTaPok")==0 )
         {
        	 if( disToBall < ballDis )
        		 i=agentes->size();
        	 else
        		 k++;
         }

        }

    if( agentes->size() == 0 || friends == 0 || k == friends )
         regreso = true;
      else
         regreso = false;

    return regreso;
}

void radar( double neck_dir,
		    AgentCommand *command )
{
	static bool aux;
	double      aumento = 45.0;

	if( neck_dir + aumento > 90.0 )
	   aux = true;

	if( neck_dir - aumento < -90.0 )
	   aux = false;

	if( neck_dir < 90.0 && aux == false )
       command->append_turn_neck(aumento);

	if( neck_dir >-90.0 && aux == true)
	   command->append_turn_neck(-aumento);
}

