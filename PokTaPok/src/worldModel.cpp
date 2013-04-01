#include "worldModel.h"
#include "utilities.h"

AgentStateV1::AgentStateV1()
{
	this->angle = 0.0;
	this->pos = 0.0;
	this->side = 'l';
	this->synch_see_on = false;
	this->unum = 0;
	this->view_mode_w = NORMAL;
	this->view_mode_q = HIGH;
	this->stamina = 8000;
	this->effort = 1.0;
	this->head_angle = 0.0;
}

WorldModelV1::WorldModelV1( GameData * game_data )
{
	time = 0;
	play_mode = PLAYMODE_NULL;
    loc_engine = new LocalizationEngine( game_data );

}

void WorldModelV1::update( GameData * game_data )

{
	// Guardamos las referencias
	this->command_commited = &game_data->command_commited;
	this->game_data		 = game_data;
	SensorHandler & sensor_h = game_data->sensor_handler;
	SensorType sensor_type   = sensor_h.last_sensor_type;

	// Ejecuta el filtro de partículas
	loc_engine->updatePos( );

	// Actualizamos la localización
	me.pos.x = loc_engine->x;
	me.pos.y = loc_engine->y;
	me.angle = loc_engine->angle;

	// Actualizamos los demás datos del modelo del mundo en base
	// a la información del último sensor que recibimos
	switch( sensor_type )
	{
	case SENSOR_NONE:
		break;
	case SENSOR_BODY:
		this->updateOnBody();
		break;
	case SENSOR_HEAR:
		this->updateOnHear();
		break;
	case SENSOR_MSG:

		break;
	case SENSOR_INIT:
		this->updateOnInit();
		break;
	case SENSOR_ERROR:

		break;
	case SENSOR_OK:
		this->updateOnOk();

		break;
	case SENSOR_SEE:
		this->updateOnSee();
		break;
	case SENSOR_SEE_GLOBAL:
		// No usado por el agente
		break;
	}
}

void WorldModelV1::updateOnBody()
{
	BodySensor const & body = game_data->sensor_handler.last_sense;

	time = body.time;

	me.effort 		= body.effort;
	me.stamina 		= body.stamina;
	me.view_mode_q 	= body.view_mode_quality;
	me.view_mode_w 	= body.view_mode_width;
	me.head_angle 	= body.head_angle;


	// Actualizamos en base a los comandos enviados
	this->updateOnCommandSent();
}

void WorldModelV1::updateOnSee()
{
	SeeSensor & see = game_data->sensor_handler.last_see;
	time = see.time;
	actualizarBitacora();
}

void WorldModelV1::updateOnInit()
{
	InitSensor const & init = game_data->sensor_handler.last_init;
	this->play_mode = init.play_mode;
	me.side = init.side;
	me.unum = init.unum;


}

void WorldModelV1::updateOnHear()
{
	SensorHandler & sensor_h = game_data->sensor_handler;

	time = sensor_h.last_hear.time;

	if( sensor_h.last_hear.sender == REFEREE )
	{
		play_mode = sensor_h.last_hear_referee.play_mode;
	}
}

void WorldModelV1::updateOnOk()
{
	SensorHandler & sensor_h = game_data->sensor_handler;

	switch( sensor_h.last_ok )
	{
	case CHANGE_MODE: // Propio del coach, no lo usamos
		break;
	case MOVE:
		break;
	case RECOVER: // Propio del coach, no lo usamos
		break;
	case EAR:
		break;
	case EYE: // Propio del coach, no lo usamos
		break;
	case CHECK_BALL: // Propio del coach, no lo usamos
		break;
	case SYNCH_SEE:
		me.synch_see_on = true;
		break;
	case LOOK: // Propio del coach, no lo usamos
		break;
	case START: // Propio del coach, no lo usamos
		break;
	}
}


void WorldModelV1::updateOnCommandSent()
{
	AgentCommand const & com = game_data->command_commited;
	if( com.attention_to_is_set() )
	{

	}
	if( com.catch_is_set() )
	{

	}
	if( com.change_view_is_set() )
	{
		me.view_mode_q = com.change_view_quality;
		me.view_mode_w = com.change_view_width;
	}
	if( com.dash_is_set() )
	{

	}
	if( com.kick_is_set() )
	{

	}
	if( com.move_is_set() )
	{

	}
	if( com.say_is_set() )
	{

	}
	if( com.score_is_set() )
	{

	}
	if( com.synch_see_is_set() )
	{

	}
	if( com.turn_is_set() )
	{

	}
	if( com.turn_neck_is_set() )
	{

	}

}


Vector2D
WorldModelV1::fromChgToVel(  Vector2D const & player_pos, // Posición del jugador, absoluta.
						Vector2D const & player_vel, // Velocidad del jugador, absoluta.
						Vector2D const & obj_pos, // Posición del objeto que ve, absoluta.
						double obj_dist,
						double dist_chg,
						double dir_chg
						)
{
	Vector2D etmp = obj_pos - player_pos;
	Vector2D vtmp (0.0);

	if( obj_dist == 0 )
	{
		std::cerr << "Warning. fromChgToVel(): zero division." << std::endl;
		return vtmp;
	}
	etmp /= obj_dist;

	vtmp.x = dist_chg*etmp.x - obj_dist*dir_chg*etmp.y*DEG2RAD;
	vtmp.y = dist_chg*etmp.y + obj_dist*dir_chg*etmp.x*DEG2RAD;

	vtmp += player_vel;

	return vtmp;
}

void WorldModelV1::actualizarBitacora()
{
	SeeSensor        * vision;

	SensorType sensor_type    = game_data->sensor_handler.last_sensor_type;
	vector <Player> & player  = game_data->sensor_handler.last_see.players;
	Ball 			& ball    = game_data->sensor_handler.last_see.ball;
	BodySensor 		& body	  = game_data->sensor_handler.last_sense;

    vision = & game_data->sensor_handler.last_see;

	objetoBitacora  balonAux;
	objetoBitacora  playerAux;
	Vector2D  coordenadas;
	Vector2D vel;
	Vector2D obj_vel;

	if( sensor_type == SENSOR_SEE )   // Se recibio sensor visual
	{
		if ( vision->ballIsVisible() )  // El balón esta visible
		    {
			 balonAux.name   = "ball";
			 balonAux.num    = 0;
			 balonAux.pos    = // No incluimos el ángulo del cuello
			     Vector2D::fromPolar( ball.dis ,
						 	 	 	  Deg2Rad( ball.dir + me.angle ) );
		     balonAux.pos.x += me.pos.x;                 // coordenada global x del balón
		     balonAux.pos.y += me.pos.y;                // coordenada global y del balón
		     balonAux.dis    = ball.dis;            // distancia al balón
		     balonAux.dir    = ball.dir;           // dirección hacia el balón
		     balonAux.ciclo  = game_data->sensor_handler.last_see.time;		 // ciclo del sensado
		     if( ball.dis_chg != NDEF_NUM && ball.dir_chg != NDEF_NUM )
		     {   // Actualizamos la velocidad global en base a dir_chg y dis_chg
		    	 // No incluimos el ángulo del cuello
		    	 vel = Vector2D::fromPolar(
		    			 body.speed_amount,
		    			 Deg2Rad(body.speed_direction + me.angle /*+ me.head_angle*/ ) );

		    	 obj_vel = fromChgToVel( me.pos,
		    			 vel,
		    			 balonAux.pos,
		    			 ball.dis,
		    			 ball.dis_chg,
		    			 ball.dir_chg );
		     }
		     else
		    	 obj_vel = NDEF_NUM;
		     balonAux.vel = obj_vel;
		     if( bitacoraBalon.size() <= tamBitacora )     // si el tamaño de la bitácora es menor a 10 ciclos atras
			     bitacoraBalon.push_front(balonAux);		   // introducimos los datos en la Fila.
		     else                                 // si es más de 10
	         	{
			     bitacoraBalon.pop_back();         // eliminamos el primer elemento que entro en la Fila (el más viejo)
			     bitacoraBalon.push_front(balonAux); // y luego ya introducimos los nuevos datos del balón al final
		        }
			}

		if ( player.size() > 0)    // Existe al menos un agente visible
		    {
			 for ( unsigned int i = 0; i < player.size() -1 ; i++)  // recorremos todos los jugadores visibles
			     {
				   if( player[i].isMyMate( TEAM_NAME ) )  // si es de mi equipo
			          {
						if( player[i].unum != NDEF_NUM )  // si llego el número del jugador
						   {
							playerAux.name  = "friend";
							playerAux.num   = player[i].unum;
							// No incluimos el ángulo del cuello
							coordenadas     =
							Vector2D::fromPolar(
									player[i].dis,
									Deg2Rad( player[i].dir + me.angle /*+ me.head_angle*/) );
							playerAux.pos.x = coordenadas.x + me.pos.x;
							playerAux.pos.y = coordenadas.y + me.pos.y;
							playerAux.dis   = player[i].dis;
							playerAux.dir   = player[i].dir;
							playerAux.ciclo = game_data->sensor_handler.last_see.time;

							if( player[i].dis_chg != NDEF_NUM )
							{   // Actualizamos la velocidad global en base a dir_chg y dis_chg


								vel = Vector2D::fromPolar(
									  body.speed_amount,
									  Deg2Rad(body.speed_direction + me.angle /*+ me.head_angle*/ ) );

								obj_vel = fromChgToVel( me.pos,
													  vel,
													  playerAux.pos,
													  player[i].dis,
													  player[i].dis_chg,
													  player[i].dir_chg );
							}
							else
								obj_vel = NDEF_NUM;
							playerAux.vel = obj_vel;
							if( bitacoraAmigos[playerAux.num].size() <= tamBitacora )   // si el tamaño de la bitácora es menor a 10 ciclos atras
			                    bitacoraAmigos[playerAux.num].push_front( playerAux );   // introducimos los datos en la Fila.
		                    else                                 // si es más de 10
	         	               {
			                    bitacoraAmigos[playerAux.num].pop_back();              // eliminamos el primer elemento que entro en la Fila (el más viejo)
			                    bitacoraAmigos[playerAux.num].push_front( playerAux );   // y luego ya introducimos los nuevos datos del jugador al final
		                       }
					       }
					    else
					       {
							// No llego el número del jugador
							// No puede almancenar la bitácora, no se sabe de que jugador se trata
						   }
					  }
				   else   // si no es de mi equipo (es rival)
				      {
						if( player[i].unum != NDEF_NUM )  // si llego el número del jugador
						   {
							playerAux.name  = "enemy";
							playerAux.num   = player[i].unum;

							coordenadas     =
							Vector2D::fromPolar(
									  player[i].dis,
									  Deg2Rad( player[i].dir + me.angle /*+ me.head_angle*/ ) );
							playerAux.pos.x = coordenadas.x + me.pos.x;
							playerAux.pos.y = coordenadas.y + me.pos.y;
							playerAux.dis   = player[i].dis;
							playerAux.dir   = player[i].dir;
							playerAux.ciclo = game_data->sensor_handler.last_see.time;
							if( player[i].dis_chg != NDEF_NUM )
							{   // Actualizamos la velocidad global en base a dir_chg y dis_chg


								vel = Vector2D::fromPolar(
									  body.speed_amount,
									  Deg2Rad(body.speed_direction + me.angle/* + me.head_angle*/ ) );

								obj_vel = fromChgToVel( me.pos,
													  vel,
													  playerAux.pos,
													  player[i].dis,
													  player[i].dis_chg,
													  player[i].dir_chg );
							}
							else
								obj_vel = NDEF_NUM;
							playerAux.vel = obj_vel;

							if( bitacoraRivales[playerAux.num].size() <= tamBitacora )   // si el tamaño de la bitácora es menor a 10 ciclos atras
			                    bitacoraRivales[playerAux.num].push_front( playerAux );   // introducimos los datos en la Fila.
		                    else                                 // si es más de 10
	         	               {
			                    bitacoraRivales[playerAux.num].pop_back();              // eliminamos el primer elemento que entro en la Fila (el más viejo)
			                    bitacoraRivales[playerAux.num].push_front( playerAux );   // y luego ya introducimos los nuevos datos del jugador al final
		                       }
					       }
					    else
					       {
							// No llego el número del jugador
							// No puede almancenar la bitácora, no sabe de que jugador rival se trata
						   }
					  }
			     }	// termina el recorrido del vector player
            }
	}
}  //end actualizarBitacora()


// predice la velocidad del balón
Vector2D
WorldModelV1::predictVel( deque <objetoBitacora> const & fila )
{
	double decay_b = game_data->game_parameter.server_param.ball_decay;
	double decay_p = game_data->game_parameter.server_param.player_decay;
	objetoBitacora ultimoElemento, pUltimoElemento;
	Vector2D velocidad;

    ultimoElemento  = fila[ fila.size() -1 ];
    pUltimoElemento = fila[ fila.size() -2 ];

    if( ultimoElemento.name == "ball" )
	   {
		velocidad.x = ( ultimoElemento.pos.x - pUltimoElemento.pos.x ) * decay_b;   // aproximamos la velocidad en x
  	    velocidad.y = ( ultimoElemento.pos.y - pUltimoElemento.pos.y ) * decay_b;   // aproximamos la velocidad en y
  	   }
  	else
  	   {
     	velocidad.x = ( ultimoElemento.pos.x - pUltimoElemento.pos.x  ) * decay_p;   // aproximamos la velocidad en x
	    velocidad.y = ( ultimoElemento.pos.y - pUltimoElemento.pos.y ) * decay_p;   // aproximamos la velocidad en y
	   }

	return velocidad;
}

/// predice la la posición de la pelota
/// después del último ciclo de sensado
Vector2D
WorldModelV1::predictPose( deque <objetoBitacora> const & fila )
{
	double         coorx,coory;
	Vector2D       velocidad,futurePose;
	objetoBitacora ultimoElemento;

	velocidad = predictVel(fila);

	ultimoElemento = fila[ fila.size()-1 ]; // obtenemos el ultimo elemento (el más reciente)

	coorx = ultimoElemento.pos.x;           // obtenemos coordenada x del balón
	coory = ultimoElemento.pos.y;           // obtenemos coordenada y del balón

	futurePose.x = velocidad.x + coorx;
	futurePose.y = velocidad.y + coory;

	return futurePose;
}

/// predice el número de ciclos que
/// tarda el balón en recorrer cierta distancia
int
WorldModelV1::predictCycles( deque <objetoBitacora> const & fila , double distanciaObjetivo )
{
	double decay_b = game_data->game_parameter.server_param.ball_decay;
	double decay_p = game_data->game_parameter.server_param.player_decay;
	Vector2D velocidad;
	int n;

	velocidad = predictVel(fila);
    if( fila.back().name == "ball" )
	   n = log( 1.0 - ( distanciaObjetivo * ( 1.0 - (decay_b) ) / velocidad.normita() ) ) / log(decay_b);
	else
	   n = log( 1.0 - ( distanciaObjetivo * ( 1.0 - (decay_p) ) / velocidad.normita() ) ) / log(decay_p);

	return n;
}

/// predice la distancia que recorrera con cierta velocidad
/// en determinado numero de ciclos
double
WorldModelV1::predictDistance( deque <objetoBitacora> const & fila , int n )
{
	double decay_b =  game_data->game_parameter.server_param.ball_decay;
	double decay_p =  game_data->game_parameter.server_param.player_decay;

	double distancia;
	Vector2D velocidad;

	velocidad = predictVel(fila);

    if(fila.back().name == "ball")
	  distancia = velocidad.normita() * ( ( 1.0 - pow(decay_b,n) ) / (1.0 - decay_b) );
	else
	  distancia = velocidad.normita() * ( ( 1.0 - pow(decay_p,n) ) / (1.0 - decay_p) );

	return distancia;
}

bool
WorldModelV1::predictBallCurrentVel( Vector2D * v )
{
	// Usamos información disponible en los ciclos anteriores y en el actual
	// para obtener un aproximado de la velocidad del balón
	int   diferencia_tiempo;
//	bool  impulso_detectado = false;
//	double rmax;

	double ball_decay = game_data->game_parameter.server_param.ball_decay;
	Vector2D vtmp, vtmp_2;
	*v = 0.0;

	if( this->bitacoraBalon.size() < 1 )
		return false;

	if( bitacoraBalon.size() == 1 )
	{
		if(    bitacoraBalon.begin()->vel.x == NDEF_NUM
			|| bitacoraBalon.begin()->vel.y == NDEF_NUM )
			// No tenemos información para predecir la velocidad
			return false;
		else
		{
			// Usamos la última velocidad y le aplicamos el decay
			*v = bitacoraBalon.begin()->vel;
			diferencia_tiempo =  time - bitacoraBalon.begin()->ciclo;
			*v *= std::pow( ball_decay, diferencia_tiempo );
			return true;
		}
	}
	else // ( bitacoraBalon.size() > 1)
	{	// Aproximamos la velocidad con las dos anteriores
		deque<objetoBitacora>::const_iterator it = bitacoraBalon.begin();
		deque<objetoBitacora>::const_iterator it_2 = it+1;
		vtmp = 0.0;

		if(    it->vel.x != NDEF_NUM && it->vel.y != NDEF_NUM )
		{
			if ( it_2->vel.x != NDEF_NUM && it_2->vel.y != NDEF_NUM )
			{
				diferencia_tiempo =  time - it->ciclo;
				vtmp = it->vel * std::pow( ball_decay, diferencia_tiempo );
				diferencia_tiempo =  time - it_2->ciclo;
				vtmp += it_2->vel * std::pow( ball_decay, diferencia_tiempo );
				*v = vtmp/2.0;
				return true;
			}
			else // Sólo tenemos disponible la primera velocidad,
			{
				vtmp_2 	= it->pos - it_2->pos;
				vtmp_2 /= (double)(it->ciclo - it_2->ciclo);
				diferencia_tiempo =  time - it->ciclo;
				vtmp_2 *= std::pow( ball_decay, diferencia_tiempo);

				vtmp = it->vel * std::pow( ball_decay, diferencia_tiempo );
				*v = vtmp + vtmp_2;
				*v /= 2.0;
				return true;
			}
		}
		else // No tenemos la primera velocidad
		{
			if ( it_2->vel.x != NDEF_NUM && it_2->vel.y != NDEF_NUM )
			{
				vtmp_2 	= it->pos - it_2->pos;
				vtmp_2 /= (double)(it->ciclo - it_2->ciclo);
				diferencia_tiempo =  time - it->ciclo;
				vtmp_2 *= std::pow( ball_decay, diferencia_tiempo);

				vtmp = it_2->vel;
				vtmp *= std::pow( ball_decay, time - it_2->ciclo );

				*v = vtmp_2 + vtmp;
				*v /= 2.0;
				return true;
			}
			else // No tenemos ninguna velocidad
			{ // Aproximamos la velocidad con la diferencia de posiciones
				vtmp_2 	= it->pos - it_2->pos;
				vtmp_2 /= (double)(it->ciclo - it_2->ciclo);
				diferencia_tiempo =  time - it->ciclo;
				vtmp_2 *= std::pow( ball_decay, diferencia_tiempo);
				*v = vtmp_2;
				return true;
			}
		}
		//do
		//{		}
		//while( it_2 != bitacoraBalon.end() && impulso_detectado == false );

		return true;
	}

}
