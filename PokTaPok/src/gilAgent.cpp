#include "gilAgent.h"
#include "gameCommand.h"
#include "gameData.h"
#include "observation.h"
#include <iostream>

GilAgent::GilAgent()
{
    my_fist_time = true;
}

void GilAgent::do_process( GameData * game_data, AgentCommand * agent_response, const AgentCommand *agent_response_commited )

{

    PlayModeHearable play_mode;
    this->game_data = game_data;
    this->agent_response = agent_response;
    this->agent_response_commited = agent_response_commited;
    play_mode = game_data->obs_handler.last_hear_referee.play_mode;

    if( game_data->obs_handler.last_obs_type == OBS_SEE )
    {
        printf( "%f\n", game_data->obs_handler.last_sense.speed_amount );
    }

    switch(play_mode)
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

        //agent_response->append_turn( 0 );
        break;
    }



}

void GilAgent::on_goalie_catch_ball_l(){}
void GilAgent::on_goalie_catch_ball_r(){}
void GilAgent::on_before_kick_off()
{
    //if( game_data->obs_handler.last_obs_type == OBS_SENSE )
    //{
    /*
        if( my_fist_time == true )
        {
            agent_response->command.append_turn( 90 );
            my_fist_time = false;
        }
        */
    //}
    agent_response->append_turn( 0 );
}
void GilAgent::on_time_over(){}
void GilAgent::on_play_on()
{
    this->pozole_goal();
}
void GilAgent::on_kick_off_l(){}
void GilAgent::on_kick_off_r(){}
void GilAgent::on_kick_in_l(){}
void GilAgent::on_kick_in_r(){}
void GilAgent::on_free_kick_l(){}
void GilAgent::on_free_kick_r(){}
void GilAgent::on_corner_kick_l(){}
void GilAgent::on_corner_kick_r(){}
void GilAgent::on_goal_kick_l(){}
void GilAgent::on_goal_kick_r(){}
void GilAgent::on_goal_l(){}
void GilAgent::on_goal_r(){}
void GilAgent::on_drop_ball()
{

}
void GilAgent::on_offside_l(){}
void GilAgent::on_offside_r(){}
void GilAgent::on_penalty_kick_l(){}
void GilAgent::on_penalty_kick_r(){}
void GilAgent::on_first_half_over(){}
void GilAgent::on_pause(){}
void GilAgent::on_human_judge(){}
void GilAgent::on_foul_l(){}
void GilAgent::on_foul_r(){}
void GilAgent::on_foul_charge_l(){}
void GilAgent::on_foul_charge_r(){}
void GilAgent::on_foul_push_l(){}
void GilAgent::on_foul_push_r(){}
void GilAgent::on_foul_multiple_attack_l(){}
void GilAgent::on_foul_multiple_attack_r(){}
void GilAgent::on_foul_ballout_l(){}
void GilAgent::on_foul_ballout_r(){}
void GilAgent::on_back_pass_l(){}
void GilAgent::on_back_pass_r(){}
void GilAgent::on_free_kick_fault_l(){}
void GilAgent::on_free_kick_fault_r(){}
void GilAgent::on_catch_fault_l(){}
void GilAgent::on_catch_fault_r(){}
void GilAgent::on_indirect_free_kick_l(){}
void GilAgent::on_indirect_free_kick_r(){}
void GilAgent::on_penalty_setup_l(){}
void GilAgent::on_penalty_setup_r(){}
void GilAgent::on_penalty_ready_l(){}
void GilAgent::on_penalty_ready_r(){}
void GilAgent::on_penalty_taken_l(){}
void GilAgent::on_penalty_taken_r(){}
void GilAgent::on_penalty_miss_l(){}
void GilAgent::on_penalty_miss_r(){}
void GilAgent::on_penalty_score_l(){}
void GilAgent::on_penalty_score_r(){}
void GilAgent::on_penalty_onfield_l(){}
void GilAgent::on_penalty_onfield_r(){}
void GilAgent::on_penalty_foul_r(){}
void GilAgent::on_penalty_foul_l(){}
void GilAgent::on_penalty_winner_l(){}
void GilAgent::on_penalty_winner_r(){}
void GilAgent::on_penalty_draw(){}


void GilAgent::pozole_goal()
{
    if( search_ball() )
        if( go_to_bal_kick_position() )
            kick_to_goal();
}

bool GilAgent::search_ball()
{
    if( !game_data->obs_handler.last_see.ball_is_visible() )
    {
        agent_response->append_turn( -35 );
        return false;
    }
    else
    {
        if( im_aligned_to_ball() )
        {
            return true;
        }
        else
        {
            align_body_to_ball();
            return false;
        }
    }
}

bool GilAgent::go_to_bal_kick_position()
{
    Ball * ball;
    WorldPlayer * me;

    double tolerance_radius = 1.0;
    ball = & game_data->obs_handler.last_see.ball;
    me = & game_data->world_state.me ;

    if( game_data->obs_handler.last_see.ball_is_visible() )
    {
        //cout << "\ball_distance " << ball->distance << "tolerance_radius " << tolerance_radius;
        if( ball->distance < tolerance_radius )
        {
            return true;
        }
        else
            agent_response->append_dash( 90 );
            return false;
    }
    else return false;

}

void GilAgent::kick_to_goal()
{
    Ball * ball;
    WorldPlayer * me;
    double goal_direction;
    Flag * goal_flag;
    vector<Flag> * flags;
    bool found;
    int i;
    ball = & game_data->obs_handler.last_see.ball;
    me = & game_data->world_state.me ;
    flags = & game_data->obs_handler.last_see.flags;
    found = false;
    for(i=0; i< flags->size() && !found; i++)
    {
        goal_flag = &(*flags)[i];

        if( goal_flag->id == GR )
        {
            found = true;
        }
    }
    if( found )
    {
        agent_response->append_kick( 100, (goal_flag->direction - game_data->obs_handler.last_sense.head_angle) );
    }
    else
    {
        agent_response->append_kick( 10, 0 );
    }

}

bool GilAgent::im_aligned_to_ball()
{
    Ball * ball;
    WorldPlayer * me;
    double tau;
    double tolerance_radius = 0.5;
    ball = & game_data->obs_handler.last_see.ball;
    me = & game_data->world_state.me ;

    tau = atan( tolerance_radius/ball->distance );
    //cout << "\n Ball direction: " << ball->direction;
    if( (ball->direction) < tau )
    {
        return true;
    }
    else
    {
        return false;
    }
}
void GilAgent::align_body_to_ball()
{
    Ball * ball;
    WorldPlayer * me;
    ball = & game_data->obs_handler.last_see.ball;
    me = & game_data->world_state.me ;

    agent_response->append_turn(  ball->direction );

}
