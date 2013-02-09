#ifndef AGENT_H
#define AGENT_H

#include "parser.h"
#include "gameParameter.h"
#include "obsHandler.h"
#include "gameData.h"
#include "agentResponse.h"
#include "gameCommand.h"


class Agent
{

public:

    virtual void do_process( GameData * game_data,
                             AgentCommand * agent_response,
                             AgentCommand const * agent_response_commited
                             ) = 0;

};

// Para el trainer (offline-coach)
class TrainerAgent
{
public:
    virtual void do_process( GameData * game_data,
                             TrainerCommand * trainer_command
                             ) = 0;
};
#endif /* AGENT_H */
