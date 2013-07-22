#include "worldModel.h"
#include "utilities.h"

AgentState::AgentState()
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

WorldModel::WorldModel( GameData * game_data )
{
	m_time 		  = 0;
	m_game_data   = game_data;
	m_play_mode   = PLAYMODE_NULL;
    m_loc_adapter = new LocalizationAdapter( game_data );
}

const GameData &
WorldModel::gameData()
{
	return *m_game_data;
}

const AgentState &
WorldModel::me() const
{
	return m_me;
}
PlayModeHearable
WorldModel::playMode() const
{
	return m_play_mode;
}
int
WorldModel::time() const
{
	return m_time;
}


void WorldModel::update( GameData * m_game_data )

{
	// Guardamos las referencias
	this->m_game_data		 = m_game_data;

	SensorHandler & sensor_h = m_game_data->sensor_handler;
	SensorType sensor_type   = sensor_h.last_sensor_type;

	// Ejecuta el filtro de partículas
	m_loc_adapter->updatePos( );

	// Actualizamos la localización
	m_me.pos.x = m_loc_adapter->getX();
	m_me.pos.y = m_loc_adapter->getY();
	m_me.angle = m_loc_adapter->getAngle();

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

void WorldModel::updateOnBody()
{
	BodySensor const & body = m_game_data->sensor_handler.last_sense;

	m_time = body.time;

	m_me.effort 		= body.effort;
	m_me.stamina 		= body.stamina;
	m_me.view_mode_q 	= body.view_mode_quality;
	m_me.view_mode_w 	= body.view_mode_width;
	m_me.head_angle 	= body.head_angle;
	m_me.speed_amount	= body.speed_amount;
	m_me.speed_dir	    = body.speed_direction;

	// Actualizamos en base a los comandos enviados
	this->updateOnCommandSent();
}

void WorldModel::updateOnSee()
{
	SeeSensor & see = m_game_data->sensor_handler.last_see;
	m_time = see.time;
	actualizarBitacora();
}

void WorldModel::updateOnInit()
{
	InitSensor const & init = m_game_data->sensor_handler.last_init;
	m_play_mode = init.play_mode;
	m_me.side = init.side;
	m_me.unum = init.unum;
}

void WorldModel::updateOnHear()
{
	SensorHandler & sensor_h = m_game_data->sensor_handler;

	m_time = sensor_h.last_hear.time;

	if( sensor_h.last_hear.sender == REFEREE )
	{
		m_play_mode = sensor_h.last_hear_referee.play_mode;
	}
}

void WorldModel::updateOnOk()
{
	SensorHandler & sensor_h = m_game_data->sensor_handler;

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
		m_me.synch_see_on = true;
		break;
	case LOOK: // Propio del coach, no lo usamos
		break;
	case START: // Propio del coach, no lo usamos
		break;
	}
}


void WorldModel::updateOnCommandSent()
{
	AgentCommand const & com = m_game_data->command_commited;
	if( com.attention_to_is_set() )
	{
		m_me.attention_unum = com.attention_uniform_number;
		m_me.attention_team = com.attention_team;
	}
	if( com.catch_is_set() )
	{

	}
	if( com.change_view_is_set() )
	{
		m_me.view_mode_q = com.change_view_quality;
		m_me.view_mode_w = com.change_view_width;
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


void WorldModel::actualizarBitacora()
{
	SeeSensor        * vision;

	SensorType sensor_type    = m_game_data->sensor_handler.last_sensor_type;
	vector <Player> & player  = m_game_data->sensor_handler.last_see.players;
	Ball 			& ball    = m_game_data->sensor_handler.last_see.ball;
	BodySensor 		& body	  = m_game_data->sensor_handler.last_sense;

    vision = & m_game_data->sensor_handler.last_see;

	ObjetoBitacora  balonAux;
	ObjetoBitacora  playerAux;
	Vector2D  coordenadas;
	Vector2D vel;
	Vector2D obj_vel;

	if( sensor_type == SENSOR_SEE )   // Se recibio sensor visual
	{
		if ( vision->ballIsVisible() )  // El balón esta visible
		    {
			 balonAux.name   = "ball";
			 balonAux.num    = 0;
			 balonAux.pos    =
			     Vector2D::fromPolar( ball.dis ,
						 	 	 	  Deg2Rad( ball.dir + m_me.angle + m_me.headAngleDeg() ) );
		     balonAux.pos.x += m_me.pos.x;                 // coordenada global x del balón
		     balonAux.pos.y += m_me.pos.y;                // coordenada global y del balón
		     balonAux.dis    = ball.dis;            // distancia al balón
		     balonAux.dir    = ball.dir;           // dirección hacia el balón
		     balonAux.dir_from_body = ball.dir + m_me.headAngleDeg();
		     balonAux.ciclo  = m_game_data->sensor_handler.last_see.time;		 // ciclo del sensado
		     if( ball.dis_chg != NDEF_NUM && ball.dir_chg != NDEF_NUM )
		     {   // Actualizamos la velocidad global en base a dir_chg y dis_chg

		    	 vel = Vector2D::fromPolar(
		    			 body.speed_amount,
		    			 Deg2Rad(body.speed_direction + m_me.angle + m_me.head_angle ) );

		    	 obj_vel = fromChgToVel( m_me.pos,
		    			 vel,
		    			 balonAux.pos,
		    			 ball.dis,
		    			 ball.dis_chg,
		    			 ball.dir_chg );
		     }
		     else
		    	 obj_vel = NDEF_NUM;
		     balonAux.vel = obj_vel;
		     if( bitacoraBalon.size() <= TAM_BITACORA )     // si el tamaño de la bitácora es menor a 10 ciclos atras
			     bitacoraBalon.push_front(balonAux);		   // introducimos los datos en la Fila.
		     else                                 // si es más de 10
	         	{
			     bitacoraBalon.pop_back();         // eliminamos el primer elemento que entro en la Fila (el más viejo)
			     bitacoraBalon.push_front(balonAux); // y luego ya introducimos los nuevos datos del balón al final
		        }
			}

		if ( player.size() > 0)    // Existe al menos un agente visible
		    {
			 for ( unsigned int i = 0; i < player.size(); i++)  // recorremos todos los jugadores visibles
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
									Deg2Rad( player[i].dir + m_me.angle + m_me.head_angle) );
							playerAux.pos.x = coordenadas.x + m_me.pos.x;
							playerAux.pos.y = coordenadas.y + m_me.pos.y;
							playerAux.dis   = player[i].dis;
							playerAux.dir   = player[i].dir;
							playerAux.dir_from_body = player[i].dir + m_me.headAngleDeg();
							playerAux.ciclo = m_game_data->sensor_handler.last_see.time;
							playerAux.goalie= player[i].is_goalie;

							if( player[i].dis_chg != NDEF_NUM )
							{   // Actualizamos la velocidad global en base a dir_chg y dis_chg


								vel = Vector2D::fromPolar(
									  body.speed_amount,
									  Deg2Rad(body.speed_direction + m_me.angle + m_me.head_angle ) );

								obj_vel = fromChgToVel( m_me.pos,
													  vel,
													  playerAux.pos,
													  player[i].dis,
													  player[i].dis_chg,
													  player[i].dir_chg );
							}
							else
								obj_vel = NDEF_NUM;
							playerAux.vel = obj_vel;

							//std::cout << "pushed teammate" << playerAux.num << " " << player[i].team << std::endl;
							if( bitacoraAmigos[playerAux.num-1].size() <= TAM_BITACORA )   // si el tamaño de la bitácora es menor a 10 ciclos atras
			                    bitacoraAmigos[playerAux.num-1].push_front( playerAux );   // introducimos los datos en la Fila.
		                    else                                 // si es más de 10
	         	               {
			                    bitacoraAmigos[playerAux.num-1].pop_back();              // eliminamos el primer elemento que entro en la Fila (el más viejo)
			                    bitacoraAmigos[playerAux.num-1].push_front( playerAux );   // y luego ya introducimos los nuevos datos del jugador al final
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
									  Deg2Rad( player[i].dir + m_me.angle + m_me.head_angle ) );
							playerAux.pos.x = coordenadas.x + m_me.pos.x;
							playerAux.pos.y = coordenadas.y + m_me.pos.y;
							playerAux.dis   = player[i].dis;
							playerAux.dir   = player[i].dir;
							playerAux.dir_from_body = player[i].dir + m_me.headAngleDeg();
							playerAux.ciclo = m_game_data->sensor_handler.last_see.time;
							playerAux.goalie= player[i].is_goalie;


							if( player[i].dis_chg != NDEF_NUM )
							{   // Actualizamos la velocidad global en base a dir_chg y dis_chg


								vel = Vector2D::fromPolar(
									  body.speed_amount,
									  Deg2Rad(body.speed_direction + m_me.angle + m_me.head_angle ) );

								obj_vel = fromChgToVel( m_me.pos,
													  vel,
													  playerAux.pos,
													  player[i].dis,
													  player[i].dis_chg,
													  player[i].dir_chg );
							}
							else
								obj_vel = NDEF_NUM;
							playerAux.vel = obj_vel;

							if( bitacoraRivales[playerAux.num-1].size() <= TAM_BITACORA )   // si el tamaño de la bitácora es menor a 10 ciclos atras
			                    bitacoraRivales[playerAux.num-1].push_front( playerAux );   // introducimos los datos en la Fila.
		                    else                                 // si es más de 10
	         	               {
			                    bitacoraRivales[playerAux.num-1].pop_back();              // eliminamos el primer elemento que entro en la Fila (el más viejo)
			                    bitacoraRivales[playerAux.num-1].push_front( playerAux );   // y luego ya introducimos los nuevos datos del jugador al final
		                       }
					       }
					    else
					       {
							// No llego el número del jugador
							// No puede almancenar la bitácora, no sabe de que jugador rival se trata
						   }
					  }
			     }	// Termina el recorrido del vector player
            }
	}
}  //end actualizarBitacora()

Vector2D
WorldModel::estBallVelocity() const
{
	Vector2D ball_vel;
	Vector2D p1, p2;
	int  	 t1, t2;
	int 	 time_diff;
	double const D = 0.94;

	std::deque<ObjetoBitacora>::const_iterator ball_it;
	if( bitacoraBalon.empty() )
	{
		ball_vel = 0.0;
		return ball_vel;
	}

	time_diff = this->m_time - bitacoraBalon.begin()->ciclo;

	if( time_diff == 0 )
	{
		ball_it = bitacoraBalon.begin();
		if( ball_it->vel.x != NDEF_NUM && ball_it->vel.y != NDEF_NUM )
		{
			ball_vel =  ball_it->vel;
		}
		else
		{
			if( bitacoraBalon.size() >= 2 ) // Se usan dos posiciones para estimar la velocidad
			{
				p1 = ball_it->pos;
				t1 = ball_it->ciclo;

				ball_it++;

				p2 = ball_it->pos;
				t2 = ball_it->ciclo;

				if( t1==t2 ) t1++;
				ball_vel = (p1-p2) / (double)(t1 - t2);
			}
			else
			{
				ball_vel = 0.0;
			}
		}
	}
	else	// No tenemos datos actuales de la velocidad
	{
		ball_it = bitacoraBalon.begin();
		if( ball_it->vel.x != NDEF_NUM && ball_it->vel.y != NDEF_NUM )
		{
			ball_vel =  ball_it->vel;
		}
		else
		{
			if( bitacoraBalon.size() >= 2 ) // Se usan dos posiciones para estimar la velocidad
			{
				p1 = ball_it->pos;
				t1 = ball_it->ciclo;

				ball_it++;

				p2 = ball_it->pos;
				t2 = ball_it->ciclo;

				if( t1==t2 ) t1++;
				ball_vel = (p1-p2) / (double)(t1 - t2);
			}
			else
			{
				ball_vel = 0.0;
			}
		}

		ball_vel *= std::pow(D, time_diff );
	}

	return ball_vel;

}

Vector2D
WorldModel::estBallPosition() const
{
	Vector2D b;
	if( bitacoraBalon.empty() )
		b.set(0,0);
	else
		b = bitacoraBalon.begin()->pos;
	return b;
}

Vector2D
fromChgToVel(  Vector2D const & player_pos, // Posición del jugador, absoluta.
						Vector2D const & player_vel, // Velocidad del jugador, absoluta.
						Vector2D const & obj_pos, // Posición del objeto que ve, absoluta.
						double obj_dist,
						double dist_chg,
						double dir_chg
						)
{
	Vector2D etmp = obj_pos - player_pos;
	Vector2D vtmp (0.0);

	if( obj_dist == 0.0 )
	{
		return vtmp;
	}
	etmp /= obj_dist;

	vtmp.x = dist_chg*etmp.x - obj_dist*dir_chg*etmp.y*DEG2RAD;
	vtmp.y = dist_chg*etmp.y + obj_dist*dir_chg*etmp.x*DEG2RAD;

	vtmp += player_vel;

	return vtmp;
}
