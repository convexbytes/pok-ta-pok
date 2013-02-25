#include "poktapokAgent.h"
#include "formation.h"

void PokTaPokAgentV1::do_process( GameData *game_data,
                                  AgentCommand *agent_command,
                                  const AgentCommand *agent_command_commited)
{

    // Guardamos las referencias a los datos del juego

    this->command   = agent_command;
    this->command_c = agent_command_commited;
    this->sensor_h  = &game_data->sensor_handler;
    this->param     = &game_data->game_parameter;

    command->reset();
    /*
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
    */
}


void PokTaPokAgentV1::on_before_kick_off()
{
    /*
    Vector2D const * position;
    static Vector2D auxPos( 0.0, 0.0 );
        switch( state.unum() )
        {
        case 1:
            position = &Formation::BeforeKickOff::P1;
            break;
        case 2:
            position = &Formation::BeforeKickOff::P2;
            break;
        case 3:
            position = &Formation::BeforeKickOff::P3;
            break;
        case 4:
            position = &Formation::BeforeKickOff::P4;
            break;
        case 5:
            position = &Formation::BeforeKickOff::P5;
            break;
        case 6:
            position = &Formation::BeforeKickOff::P6;
            break;
        case 7:
            position = &Formation::BeforeKickOff::P7;
            break;
        case 8:
            position = &Formation::BeforeKickOff::P8;
            break;
        case 9:
            position = &Formation::BeforeKickOff::P9;
            break;
        case 10:
            position = &Formation::BeforeKickOff::P10;
            break;
        case 11:
            position = &Formation::BeforeKickOff::P11;
            break;
        default:
            position = &auxPos;
            break;
        }

    if( state.side() == 'l' )
        command->append_move( position->x,
                              position->y );
    else
        command->append_move( - position->x,
                              position->y );
*/
}

void PokTaPokAgentV1::on_play_on()
{

}

void PokTaPokAgentV1::on_time_over(){}

void PokTaPokAgentV1::on_back_pass_l(){}

void PokTaPokAgentV1::on_back_pass_r(){}

void PokTaPokAgentV1::on_catch_fault_l(){}

void PokTaPokAgentV1::on_catch_fault_r(){}

void PokTaPokAgentV1::on_corner_kick_l(){}

void PokTaPokAgentV1::on_corner_kick_r(){}

void PokTaPokAgentV1::on_drop_ball(){}

void PokTaPokAgentV1::on_first_half_over(){}

void PokTaPokAgentV1::on_foul_ballout_l(){}

void PokTaPokAgentV1::on_foul_ballout_r(){}

void PokTaPokAgentV1::on_foul_charge_l(){}

void PokTaPokAgentV1::on_foul_charge_r(){}

void PokTaPokAgentV1::on_foul_l(){}

void PokTaPokAgentV1::on_foul_r(){}

void PokTaPokAgentV1::on_foul_multiple_attack_l(){}

void PokTaPokAgentV1::on_foul_multiple_attack_r(){}

void PokTaPokAgentV1::on_foul_push_l(){}

void PokTaPokAgentV1::on_foul_push_r(){}

void PokTaPokAgentV1::on_free_kick_fault_l(){}

void PokTaPokAgentV1::on_free_kick_fault_r(){}

void PokTaPokAgentV1::on_free_kick_l(){}

void PokTaPokAgentV1::on_free_kick_r(){}

void PokTaPokAgentV1::on_goalie_catch_ball_l(){}

void PokTaPokAgentV1::on_goalie_catch_ball_r(){}

void PokTaPokAgentV1::on_goal_kick_l(){}

void PokTaPokAgentV1::on_goal_kick_r(){}

void PokTaPokAgentV1::on_goal_l(){}

void PokTaPokAgentV1::on_goal_r(){}

void PokTaPokAgentV1::on_human_judge(){}

void PokTaPokAgentV1::on_indirect_free_kick_l(){}

void PokTaPokAgentV1::on_indirect_free_kick_r(){}

void PokTaPokAgentV1::on_kick_in_l(){}

void PokTaPokAgentV1::on_kick_in_r(){}

void PokTaPokAgentV1::on_kick_off_l(){}

void PokTaPokAgentV1::on_kick_off_r(){}

void PokTaPokAgentV1::on_offside_l(){}

void PokTaPokAgentV1::on_offside_r(){}

void PokTaPokAgentV1::on_pause(){}

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


