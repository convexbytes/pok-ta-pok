#include "worldModel.h"

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

	me.effort 	= body.effort;
	me.stamina 	= body.stamina;
	me.view_mode_q = body.view_mode_quality;
	me.view_mode_w = body.view_mode_width;
	me.head_angle = body.head_angle;
}

void WorldModelV1::updateOnSee()
{

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
