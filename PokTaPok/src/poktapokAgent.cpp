#include "poktapokAgent.h"

PoktapokAgent::PoktapokAgent()
{

}

void PoktapokAgent::do_process( GameData * game_data, AgentResponse * agent_response )
{

    agent_response->command.append_say( "SebasNoMeHaPuestoAHacerNada" );
}
