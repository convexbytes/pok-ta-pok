#include "poktapokAgent.h"
#include "formation.h"

PokTaPokAgentV1::PokTaPokAgentV1( GameData * game_data )
{
    world = new WorldModelV1( game_data );
}

void PokTaPokAgentV1::do_process( GameData *game_data,
                                  AgentCommand *agent_command )
{

    // Guardamos las referencias a los datos del juego
	this->game_data = game_data;
    this->command   = agent_command;
    // Guardamos referencias extra, para acceder más rápido a los miembros
    this->command_c = &game_data->command_commited;
    this->sensor_h  = &game_data->sensor_handler;
    this->param     = &game_data->game_parameter;

    command->reset();

    update();

    switch( state_m.state() )
    {
    case SM_INITIAL:
    	this->onInitial();
    	break;
    case SM_PREP:
    	this->onPrep();
    	break;
    case SM_PLAY:
    	this->onPlay();
    	break;
    }

}

void PokTaPokAgentV1::update()
{

	world->update( this->game_data );

	state_m.update( *this->game_data );

}

void PokTaPokAgentV1::onInitial()
{

}

void PokTaPokAgentV1::onPrep()
{

}

void PokTaPokAgentV1::onPlay()
{

}
