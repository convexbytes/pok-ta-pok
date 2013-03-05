#include "worldModel.h"
#include "client.h"

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
	me.pos.x = loc_engine->x();
	me.pos.y = loc_engine->y();
	me.angle = loc_engine->angle();


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

}

void WorldModelV1::updateOnSee()
{

}

void WorldModelV1::updateOnInit()
{
	InitSensor & init = game_data->sensor_handler.last_init;
	this->play_mode = init.play_mode;
	me.side = init.side;
	me.unum = init.unum;

}

void WorldModelV1::updateOnHear()
{
	SensorHandler & sensor_h = game_data->sensor_handler;
	HearSender sender = sensor_h.last_hear.sender;

	time = sensor_h.last_hear.time;

	if( sender == REFEREE )
	{
		play_mode = sensor_h.last_hear_referee.play_mode;
	}
}
