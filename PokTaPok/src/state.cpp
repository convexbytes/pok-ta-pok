#include "state.h"

StateMachine::StateMachine()
{
	M_state = SM_INITIAL;
}

void StateMachine::update( GameData const & game_data )
{
	SensorHandler const & sensor_h = game_data.sensor_handler;
	PlayModeHearable pm;
	if( sensor_h.last_sensor_type == SENSOR_HEAR )
	{
		if( sensor_h.last_hear.sender == REFEREE )
		{
			pm = sensor_h.last_hear_referee.play_mode;
			if( pm == BEFORE_KICK_OFF )
				this->eRefereeBefKickOff();
			else if( pm == KICK_OFF_L || pm == KICK_OFF_R )
				this->eRefereeKickOff();
			else if( pm == PLAY_ON )
				this->eRefereePlayOn();
			else if( pm == KICK_IN_L || pm == KICK_IN_R )
				this->eRefereeKickIn();
			else if( pm == FREE_KICK_L || pm == FREE_KICK_R )
				this->eRefereeFreeKick();
			else if( pm == CORNER_KICK_L || pm == CORNER_KICK_R )
				this->eRefereeCornerKick();
			else if( pm == GOALIE_CATCH_BALL_L || pm == GOALIE_CATCH_BALL_R )
				this->eRefereeGoalieCatch();
			else if( pm == GOAL_KICK_L || pm == GOAL_KICK_R )
				this->eRefereeGoalKick();
			else if( pm == INDIRECT_FREE_KICK_L || pm == INDIRECT_FREE_KICK_R )
				this->eRefereeIndirectFreeKick();
			else if( pm == PLAY_ON )
				this->eRefereePlayOn();
			else if( pm == GOAL_L || pm == GOAL_R )
				this->eRefereeGoal();
			else if( pm == TIME_OVER )
				this->eRefereeTimeOver();
			else if( pm == BACK_PASS_L || pm == BACK_PASS_R )
				this->eRefereeBackPass();
			else if( pm == FOUL_CHARGE_L || pm == FOUL_CHARGE_R )
				this->eRefereeFoulCharge();
		}
	}
}

void StateMachine::eRefereeBackPass()
{
	switch (M_state)
	{
	case SM_INITIAL:
		M_state = SM_PREP;
		break;
	case SM_PREP:

		break;
	case SM_PLAY:
		M_state = SM_PREP;
		break;
	}
}

void StateMachine::eRefereeBefKickOff()
{
	switch (M_state)
	{
	case SM_INITIAL:

		break;
	case SM_PREP:
		M_state = SM_INITIAL;
		break;
	case SM_PLAY:
		M_state = SM_INITIAL;
		break;
	}
}

void StateMachine::eRefereeCornerKick()
{
	switch (M_state)
	{
	case SM_INITIAL:
		M_state = SM_PREP;
		break;
	case SM_PREP:
		break;
	case SM_PLAY:
		M_state = SM_PREP;
		break;
	}
}

void StateMachine::eRefereeFoulCharge()
{
	switch (M_state)
	{
	case SM_INITIAL:
		M_state = SM_PREP;
		break;
	case SM_PREP:
		break;
	case SM_PLAY:
		M_state = SM_PREP;
		break;
	}
}

void StateMachine::eRefereeFreeKick()
{
	switch (M_state)
	{
	case SM_INITIAL:
		M_state = SM_PREP;
		break;
	case SM_PREP:
		break;
	case SM_PLAY:
		M_state = SM_PREP;
		break;
	}
}

void StateMachine::eRefereeGoal()
{
	switch (M_state)
	{
	case SM_INITIAL:
		break;
	case SM_PREP:
		M_state = SM_INITIAL;
		break;
	case SM_PLAY:
		M_state = SM_INITIAL;
		break;
	}
}

void StateMachine::eRefereeGoalKick()
{
	switch (M_state)
	{
	case SM_INITIAL:
		M_state = SM_PREP;
		break;
	case SM_PREP:
		break;
	case SM_PLAY:
		M_state = SM_PREP;
		break;
	}
}

void StateMachine::eRefereeGoalieCatch()
{
	switch (M_state)
	{
	case SM_INITIAL:
		break;
	case SM_PREP:
		M_state = SM_INITIAL;
		break;
	case SM_PLAY:
		M_state = SM_PREP;
		break;
	}
}

void StateMachine::eRefereeIndirectFreeKick()
{
	switch (M_state)
	{
	case SM_INITIAL:
		M_state = SM_PREP;
		break;
	case SM_PREP:
		break;
	case SM_PLAY:
		M_state = SM_PREP;
		break;
	}
}

void StateMachine::eRefereeKickIn()
{
	switch (M_state)
	{
	case SM_INITIAL:
		M_state = SM_PREP;
		break;
	case SM_PREP:
		break;
	case SM_PLAY:
		M_state = SM_PREP;
		break;
	}
}

void StateMachine::eRefereeKickOff()
{
	switch (M_state)
	{
	case SM_INITIAL:
		M_state = SM_PREP;
		break;
	case SM_PREP:
		break;
	case SM_PLAY:
		M_state = SM_PREP;
		break;
	}
}

void StateMachine::eRefereePlayOn()
{
	switch (M_state)
	{
	case SM_INITIAL:
		M_state = SM_PLAY;
		break;
	case SM_PREP:
		M_state = SM_PLAY;
		break;
	case SM_PLAY:
		break;
	}
}

void StateMachine::eRefereeTimeOver()
{
	switch (M_state)
	{
	case SM_INITIAL:

		break;
	case SM_PREP:
		M_state = SM_INITIAL;
		break;
	case SM_PLAY:
		M_state = SM_INITIAL;
		break;
	}
}
