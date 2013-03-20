#include "bitacoraBall.h"
#include "gameData.h"
#include "gameCommand.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <math.h>

using namespace std;

BitacoraBall::BitacoraBall( GameData * game_data, WorldModelV1 * world_model )
{
	this->game_data        = game_data;
	this->command_commited = &game_data->command_commited;
	this->world_model	   = world_model;
}

void BitacoraBall::insertData()
{
	SeeSensor        * vision;
	int                time;

	objetoBalon balonAux;
	Ball & ball = game_data->sensor_handler.last_see.ball;

	vision = & game_data->sensor_handler.last_see;
	SensorType sensor_type = game_data->sensor_handler.last_sensor_type;

	time = game_data->sensor_handler.last_sense.time;     
	
	if( sensor_type == SENSOR_SEE && game_data->sensor_handler.last_see.ballIsVisible() )
	{
		balonAux.pos    = Vector2D::fromPolar( ball.dis , ball.dir + world_model->me.angle );
		balonAux.pos.x += world_model->me.pos.x;                 // coordenada global x del balón
		balonAux.pos.y += world_model->me.pos.y;                // coordenada global y del balón
		balonAux.dis    = ball.dis;            // distancia al balón
		balonAux.dir    = ball.dir;           // dirección hacia el balón
		balonAux.ciclo  = game_data->sensor_handler.last_see.time;		 // ciclo del sensado

		if( bitacoraBalon.size() <= tamBitacora )             // si el tamaño de la bitácora es menor a 10 ciclos atras
			bitacoraBalon.push_back(balonAux);		         // introducimos los datos en la Fila.
		else                               // si es más de 10
		{
			bitacoraBalon.pop_front();         // eliminamos el primer elemento que entro en la Fila (el más viejo)
			bitacoraBalon.push_back(balonAux); // y luego ya introducimos los nuevos datos del balón al final
		}
	}
}

/// predice la la posición de la pelota 
/// después del último ciclo de sensado
Vector2D BitacoraBall::predictPose()
{
	double      coorx,coory;
	Vector2D    velocidad,futurePose;
	objetoBalon ultimoElemento, pUltimoElemento;

	velocidad = predictVel(); // #gil_mark La función debería recibir argumentos supongo
	                          // ya corregi para que lleve argumentos ;-) jaja

	ultimoElemento = bitacoraBalon[ bitacoraBalon.size()-1 ];  // obtenemos el ultimo elemento (el más reciente)	
	
	coorx = ultimoElemento.pos.x;           // obtenemos coordenada x del balón
	coory = ultimoElemento.pos.y;           // obtenemos coordenada y del balón
	
	pUltimoElemento = bitacoraBalon[ bitacoraBalon.size()-2]; // obtenemos el penultimo elemento (el segundo más reciente)

	futurePose.x = velocidad.x + coorx;     
	futurePose.y = velocidad.y + coory; 
    
	return futurePose;
}

/// predice la velocidad del balón
Vector2D BitacoraBall::predictVel()
{
	double decay = game_data->game_parameter.server_param.ball_decay;
	objetoBalon ultimoElemento, pUltimoElemento;
	Vector2D velocidad;

    ultimoElemento  = bitacoraBalon[ bitacoraBalon.size() -1 ];
    pUltimoElemento = bitacoraBalon[ bitacoraBalon.size() -2 ];
 
	velocidad.x = ( ultimoElemento.pos.x - pUltimoElemento.pos.x ) * decay;   // aproximamos la velocidad en x
	velocidad.y = ( ultimoElemento.pos.y - pUltimoElemento.pos.y ) * decay;   // aproximamos la velocidad en y

	return velocidad;
}


/// predice el número de ciclos que 
/// tarda el balón en recorrer cierta distancia
int BitacoraBall::predictCycles()
{
	double decay = game_data->game_parameter.server_param.ball_decay;
	Vector2D velocidad;
	int n;

	Ball & ball = game_data->sensor_handler.last_see.ball;

	velocidad = predictVel();

	n = log(1-(ball.dis*(1-(decay))/velocidad.normita())) / log(decay);

	return n;
}

/// predice la distancia que recorrera con cierta velocidad
/// en determinado numero de ciclos
double BitacoraBall::predictDistance(int n)
{
	double decay = game_data->game_parameter.server_param.ball_decay;
	double distancia;
	Vector2D velocidad;

	velocidad = predictVel();

	distancia = velocidad.normita() * ((1-pow(decay,n))/(1-decay));

	return distancia;
}





