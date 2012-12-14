#ifndef POKTAPOKAGENT_H
#define POKTAPOKAGENT_H
#include "agent.h"
class PoktapokAgent : public Agent
{
public:
    PoktapokAgent();
    virtual void do_process(GameData * game_data, AgentResponse * agent_response);
};

#endif // POKTAPOKAGENT_H
