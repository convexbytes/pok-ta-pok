#ifndef AGENTRESPONSE_H
#define AGENTRESPONSE_H
#include "gameCommand.h"

class AgentResponse
{
public:
    AgentResponse();
    GameCommand command;
    void flush(); //resetea todos los datos del response
};

#endif // AGENTRESPONSE_H
