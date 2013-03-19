#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "gameData.h"

enum SM_State
{
	SM_INITIAL,
	SM_PREP, // Preparación
	SM_PLAY
};

class StateMachine
{

public:
	StateMachine();
	SM_State state() const { return M_state; }
	void update( GameData const & game_data );


	// Eventos, se cubren los principales, incluidos a los que se pueden cambiar desde el monitor
	void eRefereeBackPass();
	void eRefereeBefKickOff();
	void eRefereeKickOff();
	void eRefereePlayOn();
	void eRefereeKickIn();
	void eRefereeCornerKick();
	void eRefereeGoalKick();
	void eRefereeGoalieCatch();
	void eRefereeGoal();
	void eRefereeFreeKick();
	void eRefereeIndirectFreeKick();
	void eRefereeTimeOver();
	void eRefereeFoulCharge();

	//void eRefereeDropBall();
	//void eRefereeFirstHOver();


private:
	SM_State M_state;

};

#endif // STATE_H
