#include "poktapokAgent.h"
#include "formation.h"

PokStateV1::PokStateV1()
{
    play_mode = BEFORE_KICK_OFF;
    ball_is_visible = false;
}

void PokStateV1::update( const ObsHandler &obs,
                         const AgentCommand &command_commited)
{
    ObservationType obs_type = obs.last_obs_type;

    switch( obs_type )
    {
    case OBS_NONE:
        break;

    case OBS_SEE:
        updateOnSee( obs.last_see );
        break;

    case OBS_SENSE:
        updateOnSense( obs.last_sense );
        break;

    case OBS_HEAR:

        switch( obs.last_hear.sender )
        {
        last_upd_time = obs.last_hear.time;
        case REFEREE:
            play_mode = obs.last_hear_referee.play_mode;
            break;

        case COUCH:
            break;

        case SELF:
            break;

        case OUR:
            break;

        case OPP:
            break;
        }
        break;

    case OBS_MSG:
        break;

    case OBS_INIT:
        updateOnInit( obs.last_init );
        break;

    case OBS_ERROR:
        break;

    case OBS_OK:
        break;

    case OBS_SEE_GLOBAL:
        // No la ocupamos
        break;
    }

    updateOnCommandCommit( command_commited );
}

void PokStateV1::updateOnCommandCommit(const AgentCommand &command_commited)
{
    //
}

void PokStateV1::updateOnInit(const InitObs &init)
{
    play_mode       = init.play_mode;
    field_side      = init.side;
    uniform_number  = init.uniform_number;

}

void PokStateV1::updateOnSee(const SeeObs &see)
{
    last_upd_time = see.time;
    this->ball_is_visible = see.ball_is_visible();
}

void PokStateV1::updateOnSense(const SenseObs &sense)
{
    last_upd_time = sense.time;
}

void PokTaPokAgentV1::do_process( GameData *game_data,
                                  AgentCommand *agent_response,
                                  const AgentCommand *agent_response_commited)
{
    this->command = agent_response;
    command->reset();
    state.update( game_data->obs_handler,
                  *agent_response_commited );

    switch( state.playMode() )
    {
    case GOALIE_CATCH_BALL_L: on_goalie_catch_ball_l(); break;
    case GOALIE_CATCH_BALL_R: on_goalie_catch_ball_r(); break;
    case BEFORE_KICK_OFF: on_before_kick_off(); break;
    case TIME_OVER: on_time_over(); break;
    case PLAY_ON: on_play_on(); break;
    case KICK_OFF_L: on_kick_off_l(); break;
    case KICK_OFF_R: on_kick_off_r(); break;
    case KICK_IN_L: on_kick_in_l(); break;
    case KICK_IN_R: on_kick_in_r(); break;
    case FREE_KICK_L: on_free_kick_l(); break;
    case FREE_KICK_R: on_free_kick_r(); break;
    case CORNER_KICK_L: on_corner_kick_l(); break;
    case CORNER_KICK_R: on_corner_kick_r(); break;
    case GOAL_KICK_L: on_goal_kick_l(); break;
    case GOAL_KICK_R: on_goal_kick_r(); break;
    case GOAL_L: on_goal_l(); break;
    case GOAL_R: on_goal_r(); break;
    case DROP_BALL: on_drop_ball(); break;
    case OFFSIDE_L: on_offside_l(); break;
    case OFFSIDE_R: on_offside_r(); break;
    case PENALTY_KICK_L: on_penalty_kick_l(); break;
    case PENALTY_KICK_R: on_penalty_kick_r(); break;
    case FIRST_HALF_OVER: on_first_half_over(); break;
    case PAUSE: on_pause(); break;
    case HUMAN_JUDGE: on_human_judge(); break;
    case FOUL_L: on_foul_l(); break;
    case FOUL_R: on_foul_r(); break;
    case FOUL_CHARGE_L: on_foul_charge_l(); break;
    case FOUL_CHARGE_R: on_foul_charge_r(); break;
    case FOUL_PUSH_L: on_foul_push_l(); break;
    case FOUL_PUSH_R: on_foul_push_r(); break;
    case FOUL_MULTIPLE_ATTACK_L: on_foul_multiple_attack_l(); break;
    case FOUL_MULTIPLE_ATTACK_R: on_foul_multiple_attack_r(); break;
    case FOUL_BALLOUT_L: on_foul_ballout_l(); break;
    case FOUL_BALLOUT_R: on_foul_ballout_r(); break;
    case BACK_PASS_L: on_back_pass_l(); break;
    case BACK_PASS_R: on_back_pass_r(); break;
    case FREE_KICK_FAULT_L: on_free_kick_fault_l(); break;
    case FREE_KICK_FAULT_R: on_free_kick_fault_r(); break;
    case CATCH_FAULT_L: on_catch_fault_l(); break;
    case CATCH_FAULT_R: on_catch_fault_r(); break;
    case INDIRECT_FREE_KICK_L: on_indirect_free_kick_l(); break;
    case INDIRECT_FREE_KICK_R: on_indirect_free_kick_r(); break;
    case PENALTY_SETUP_L: on_penalty_setup_l(); break;
    case PENALTY_SETUP_R: on_penalty_setup_r(); break;
    case PENALTY_READY_L: on_penalty_ready_l(); break;
    case PENALTY_READY_R: on_penalty_ready_r(); break;
    case PENALTY_TAKEN_L: on_penalty_taken_l(); break;
    case PENALTY_TAKEN_R: on_penalty_taken_r(); break;
    case PENALTY_MISS_L: on_penalty_miss_l(); break;
    case PENALTY_MISS_R: on_penalty_miss_r(); break;
    case PENALTY_SCORE_L: on_penalty_score_l(); break;
    case PENALTY_SCORE_R: on_penalty_score_r(); break;
    case PENALTY_ONFIELD_L: on_penalty_onfield_l(); break;
    case PENALTY_ONFIELD_R: on_penalty_onfield_r(); break;
    case PENALTY_FOUL_R: on_penalty_foul_r(); break;
    case PENALTY_FOUL_L: on_penalty_foul_l(); break;
    case PENALTY_WINNER_L: on_penalty_winner_l(); break;
    case PENALTY_WINNER_R: on_penalty_winner_r(); break;
    case PENALTY_DRAW: on_penalty_draw(); break;
    default:
        break;
    }

}

void PokTaPokAgentV1::on_before_kick_off()
{
    Vector2D const * position;
    if( state.side() == 'l' )
    {
        switch( state.unum() )
        {
        case 1:
            position = &Formation::BeforeKickOff::L1;
            break;
        case 2:
            position = &Formation::BeforeKickOff::L2;
            break;
        case 3:
            position = &Formation::BeforeKickOff::L3;
            break;
        case 4:
            position = &Formation::BeforeKickOff::L4;
            break;
        case 5:
            position = &Formation::BeforeKickOff::L5;
            break;
        case 6:
            position = &Formation::BeforeKickOff::L6;
            break;
        case 7:
            position = &Formation::BeforeKickOff::L7;
            break;
        case 8:
            position = &Formation::BeforeKickOff::L8;
            break;
        case 9:
            position = &Formation::BeforeKickOff::L9;
            break;
        case 10:
            position = &Formation::BeforeKickOff::L10;
            break;
        case 11:
            position = &Formation::BeforeKickOff::L11;
            break;
        }
    }
    else
    {
        switch( state.unum() )
        {
        case 1:
            position = &Formation::BeforeKickOff::R1;
            break;
        case 2:
            position = &Formation::BeforeKickOff::R2;
            break;
        case 3:
            position = &Formation::BeforeKickOff::R3;
            break;
        case 4:
            position = &Formation::BeforeKickOff::R4;
            break;
        case 5:
            position = &Formation::BeforeKickOff::R5;
            break;
        case 6:
            position = &Formation::BeforeKickOff::R6;
            break;
        case 7:
            position = &Formation::BeforeKickOff::R7;
            break;
        case 8:
            position = &Formation::BeforeKickOff::R8;
            break;
        case 9:
            position = &Formation::BeforeKickOff::R9;
            break;
        case 10:
            position = &Formation::BeforeKickOff::R10;
            break;
        case 11:
            position = &Formation::BeforeKickOff::R11;
            break;
        }
    }

    command->append_move( position->x,
                          position->y );

}

void PokTaPokAgentV1::on_play_on()
{

}

void PokTaPokAgentV1::on_time_over(){}

void PokTaPokAgentV1::on_back_pass_l()
{

}

void PokTaPokAgentV1::on_back_pass_r()
{

}

void PokTaPokAgentV1::on_catch_fault_l()
{

}

void PokTaPokAgentV1::on_catch_fault_r()
{

}

void PokTaPokAgentV1::on_corner_kick_l()
{

}

void PokTaPokAgentV1::on_corner_kick_r()
{

}

void PokTaPokAgentV1::on_drop_ball()
{

}

void PokTaPokAgentV1::on_first_half_over()
{

}

void PokTaPokAgentV1::on_foul_ballout_l()
{

}

void PokTaPokAgentV1::on_foul_ballout_r()
{

}

void PokTaPokAgentV1::on_foul_charge_l()
{

}

void PokTaPokAgentV1::on_foul_charge_r()
{

}

void PokTaPokAgentV1::on_foul_l()
{

}

void PokTaPokAgentV1::on_foul_r()
{

}

void PokTaPokAgentV1::on_foul_multiple_attack_l()
{

}

void PokTaPokAgentV1::on_foul_multiple_attack_r()
{

}

void PokTaPokAgentV1::on_foul_push_l()
{

}

void PokTaPokAgentV1::on_foul_push_r()
{

}

void PokTaPokAgentV1::on_free_kick_fault_l()
{

}

void PokTaPokAgentV1::on_free_kick_fault_r()
{

}

void PokTaPokAgentV1::on_free_kick_l()
{

}

void PokTaPokAgentV1::on_free_kick_r()
{

}

void PokTaPokAgentV1::on_goalie_catch_ball_l()
{

}

void PokTaPokAgentV1::on_goalie_catch_ball_r()
{

}

void PokTaPokAgentV1::on_goal_kick_l()
{

}

void PokTaPokAgentV1::on_goal_kick_r()
{

}

void PokTaPokAgentV1::on_goal_l()
{

}

void PokTaPokAgentV1::on_goal_r()
{

}

void PokTaPokAgentV1::on_human_judge()
{

}

void PokTaPokAgentV1::on_indirect_free_kick_l()
{

}

void PokTaPokAgentV1::on_indirect_free_kick_r()
{

}

void PokTaPokAgentV1::on_kick_in_l()
{

}

void PokTaPokAgentV1::on_kick_in_r()
{

}

void PokTaPokAgentV1::on_kick_off_l()
{

}

void PokTaPokAgentV1::on_kick_off_r()
{

}

void PokTaPokAgentV1::on_offside_l()
{

}

void PokTaPokAgentV1::on_offside_r()
{

}

void PokTaPokAgentV1::on_pause()
{

}

void PokTaPokAgentV1::on_penalty_setup_l(){}
void PokTaPokAgentV1::on_penalty_setup_r(){}
void PokTaPokAgentV1::on_penalty_ready_l(){}
void PokTaPokAgentV1::on_penalty_ready_r(){}
void PokTaPokAgentV1::on_penalty_taken_l(){}
void PokTaPokAgentV1::on_penalty_taken_r(){}
void PokTaPokAgentV1::on_penalty_miss_l(){}
void PokTaPokAgentV1::on_penalty_miss_r(){}
void PokTaPokAgentV1::on_penalty_score_l(){}
void PokTaPokAgentV1::on_penalty_score_r(){}
void PokTaPokAgentV1::on_penalty_onfield_l(){}
void PokTaPokAgentV1::on_penalty_onfield_r(){}
void PokTaPokAgentV1::on_penalty_foul_r(){}
void PokTaPokAgentV1::on_penalty_foul_l(){}
void PokTaPokAgentV1::on_penalty_winner_l(){}
void PokTaPokAgentV1::on_penalty_winner_r(){}
void PokTaPokAgentV1::on_penalty_draw(){}
void PokTaPokAgentV1::on_penalty_kick_l(){}
void PokTaPokAgentV1::on_penalty_kick_r(){}
