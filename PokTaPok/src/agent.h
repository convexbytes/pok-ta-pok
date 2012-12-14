#ifndef AGENT_H
#define AGENT_H
#include "parser.h"
#include "gameParameter.h"
#include "obsHandler.h"
#include "gameData.h"
#include "LocalizationAlgorithm.h"
#include "localizationEngine.h"
#include "agentResponse.h"

class Agent
{

public:
    //Agent( GameData * game_data );

    virtual void do_process( GameData * game_data,
                             AgentResponse * agent_response,
                             AgentResponse * agent_response_commited
                             ) = 0;

};

#endif /* AGENT_H */
