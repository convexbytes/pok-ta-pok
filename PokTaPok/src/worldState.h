
#ifndef WORLD_STATE_H
#define WORLD_STATE_H
#include "obsHandler.h"

class AgentStateV1
{
public:
    AgentStateV1();
    bool synchSeeOn() const { return synch_see_on; }

private:
    bool synch_see_on;
};

class WorldState
{
public:
    virtual void updateWorld( ObsHandler const & obs_h ) = 0;
};

class WorldStateV1 : public WorldState
{
public:

    virtual void updateWorld( ObsHandler const & obs_h);

    AgentStateV1 me;
};



#endif // WORLD_STATE_H
