#include "bitacora.h"
#include "gameData.h"
#include "gameCommand.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#ifndef TEAM_NAME
#define TEAM_NAME "PokTaPok"
#endif // TEAM_NAME

using namespace std;

Bitacora::Bitacora( GameData * game_data, WorldModelV1 * world_model )
{
	this->game_data        = game_data;
	this->command_commited = &game_data->command_commited;
	this->world_model	   = world_model;
}

void Bitacora::insertData()
{
	SeeSensor        * vision;
	int                time;

	SensorType sensor_type    = game_data->sensor_handler.last_sensor_type;
	vector <Player> & player  = game_data->sensor_handler.last_see.players;
	Ball & ball               = game_data->sensor_handler.last_see.ball;
	
    vision = & game_data->sensor_handler.last_see;
	time   = game_data->sensor_handler.last_sense.time;     
	
	objetoBitacora  balonAux;
	objetoBitacora  playerAux;
	Vector2D  coordenadas;
	
	if( sensor_type == SENSOR_SEE )   // Se recibio sensor visual
	{
		if ( vision->ballIsVisible() )  // El balón esta visible
		    {
			 balonAux.name   = "ball";
			 balonAux.num    = 0;
			 balonAux.pos    = Vector2D::fromPolar( ball.dis , ball.dir + world_model->me.angle );
		     balonAux.pos.x += world_model->me.pos.x;                 // coordenada global x del balón
		     balonAux.pos.y += world_model->me.pos.y;                // coordenada global y del balón
		     balonAux.dis    = ball.dis;            // distancia al balón
		     balonAux.dir    = ball.dir;           // dirección hacia el balón
		     balonAux.ciclo  = game_data->sensor_handler.last_see.time;		 // ciclo del sensado		     

		     if( bitacoraBalon.size() <= tamBitacora )     // si el tamaño de la bitácora es menor a 10 ciclos atras
			     bitacoraBalon.push_back(balonAux);		   // introducimos los datos en la Fila.
		     else                                 // si es más de 10
	         	{
			     bitacoraBalon.pop_front();         // eliminamos el primer elemento que entro en la Fila (el más viejo)
			     bitacoraBalon.push_back(balonAux); // y luego ya introducimos los nuevos datos del balón al final
		        }
			}
		
		if ( player.size() > 0)    // Existe al menos un agente visible
		    {
			 for ( int i = 0; i < player.size() -1 ; i++)  // recorremos todos los jugadores visibles
			     {
				   if( player[i].isMyMate( TEAM_NAME ) )  // si es de mi equipo
			          {
						if( player[i].unum != NDEF_NUM )  // si llego el número del jugador
						   {
							playerAux.name  = "friend";
							playerAux.num   = player[i].unum;
							coordenadas     = Vector2D::fromPolar(player[i].dis,player[i].dir + world_model->me.angle);
							playerAux.pos.x = coordenadas.x + world_model -> me.pos.x;
							playerAux.pos.y = coordenadas.y + world_model -> me.pos.y;
							playerAux.dis   = player[i].dis;
							playerAux.dir   = player[i].dir;
							playerAux.ciclo = game_data->sensor_handler.last_see.time;
							
							if( bitacoraAmigos[playerAux.num].size() <= tamBitacora )   // si el tamaño de la bitácora es menor a 10 ciclos atras
			                    bitacoraAmigos[playerAux.num].push_back( playerAux );   // introducimos los datos en la Fila.
		                    else                                 // si es más de 10
	         	               {
			                    bitacoraAmigos[playerAux.num].pop_front();              // eliminamos el primer elemento que entro en la Fila (el más viejo)
			                    bitacoraAmigos[playerAux.num].push_back( playerAux );   // y luego ya introducimos los nuevos datos del jugador al final
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
							coordenadas     = Vector2D::fromPolar(player[i].dis,player[i].dir + world_model->me.angle);
							playerAux.pos.x = coordenadas.x + world_model -> me.pos.x;
							playerAux.pos.y = coordenadas.y + world_model -> me.pos.y;
							playerAux.dis   = player[i].dis;
							playerAux.dir   = player[i].dir;
							playerAux.ciclo = game_data->sensor_handler.last_see.time;
							
							if( bitacoraRivales[playerAux.num].size() <= tamBitacora )   // si el tamaño de la bitácora es menor a 10 ciclos atras
			                    bitacoraRivales[playerAux.num].push_back( playerAux );   // introducimos los datos en la Fila.
		                    else                                 // si es más de 10
	         	               {
			                    bitacoraRivales[playerAux.num].pop_front();              // eliminamos el primer elemento que entro en la Fila (el más viejo)
			                    bitacoraRivales[playerAux.num].push_back( playerAux );   // y luego ya introducimos los nuevos datos del jugador al final
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
}  //end insertData()

/// predice la velocidad del balón
Vector2D predictVel( deque <objetoBitacora> const & fila )
{
	double decay_b = 0.94; // game_data->game_parameter.server_param.ball_decay;
	double decay_p = 0.4; //game_data->game_parameter.server_param.player_decay;
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
Vector2D predictPose( deque <objetoBitacora> const & fila )
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
int predictCycles( deque <objetoBitacora> const & fila , double distanciaObjetivo )
{
	double decay_b = 0.94; // game_data->game_parameter.server_param.ball_decay;
	double decay_p = 0.4; //game_data->game_parameter.server_param.player_decay;
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
double predictDistance( deque <objetoBitacora> const & fila , int n )
{
	double decay_b = 0.94; // game_data->game_parameter.server_param.ball_decay;
	double decay_p = 0.4; //game_data->game_parameter.server_param.player_decay;
	
	double distancia;
	Vector2D velocidad;

	velocidad = predictVel(fila);

    if(fila.back().name == "ball")
	  distancia = velocidad.normita() * ( ( 1.0 - pow(decay_b,n) ) / (1.0 - decay_b) );
	else
	  distancia = velocidad.normita() * ( ( 1.0 - pow(decay_p,n) ) / (1.0 - decay_p) );   

	return distancia;
}
