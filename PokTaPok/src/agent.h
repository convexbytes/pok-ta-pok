#ifndef AGENT_H
#define AGENT_H

#include "parser.h"
#include "gameParameter.h"
#include "gameData.h"
#include "gameCommand.h"


class Agent
{

public:
	Agent( GameData * game_data,
            AgentCommand * agent_response )
		: m_game_data(game_data), m_command(agent_response)
	{	}
	~Agent()
	{
		m_game_data = 0;
		m_command   = 0;
	}
	const AgentCommand getCommand() const { return *m_command; }
    virtual void do_process( ) = 0;
protected:
    GameData 	 * m_game_data;
    AgentCommand * m_command;
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
