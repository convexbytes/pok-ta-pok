#include "agentResponse.h"
#include "gameCommand.h"
AgentResponse::AgentResponse()
{
}

void AgentResponse::flush()
{
    command.reset();
}
