#ifndef GILAGENT_H
#define GILAGENT_H
#include "agent.h"

class GilAgent : public Agent
{
private:
    GameData        * game_data;
    AgentResponse   * agent_response;
    AgentResponse   * agent_response_commited;
    bool              my_fist_time;
public:
    GilAgent();
    virtual void do_process(GameData * game_data,
                            AgentResponse * agent_response,
                            AgentResponse * agent_response_commited);

    //play_mode functions
    void on_goalie_catch_ball_l();
    void on_goalie_catch_ball_r();
    void on_before_kick_off();
    void on_time_over();
    void on_play_on();
    void on_kick_off_l();
    void on_kick_off_r();
    void on_kick_in_l();
    void on_kick_in_r();
    void on_free_kick_l();
    void on_free_kick_r();
    void on_corner_kick_l();
    void on_corner_kick_r();
    void on_goal_kick_l();
    void on_goal_kick_r();
    void on_goal_l();
    void on_goal_r();
    void on_drop_ball();
    void on_offside_l();
    void on_offside_r();
    void on_penalty_kick_l();
    void on_penalty_kick_r();
    void on_first_half_over();
    void on_pause();
    void on_human_judge();
    void on_foul_l();
    void on_foul_r();
    void on_foul_charge_l();
    void on_foul_charge_r();
    void on_foul_push_l();
    void on_foul_push_r();
    void on_foul_multiple_attack_l();
    void on_foul_multiple_attack_r();
    void on_foul_ballout_l();
    void on_foul_ballout_r();
    void on_back_pass_l();
    void on_back_pass_r();
    void on_free_kick_fault_l();
    void on_free_kick_fault_r();
    void on_catch_fault_l();
    void on_catch_fault_r();
    void on_indirect_free_kick_l();
    void on_indirect_free_kick_r();
    void on_penalty_setup_l();
    void on_penalty_setup_r();
    void on_penalty_ready_l();
    void on_penalty_ready_r();
    void on_penalty_taken_l();
    void on_penalty_taken_r();
    void on_penalty_miss_l();
    void on_penalty_miss_r();
    void on_penalty_score_l();
    void on_penalty_score_r();
    void on_penalty_onfield_l();
    void on_penalty_onfield_r();
    void on_penalty_foul_r();
    void on_penalty_foul_l();
    void on_penalty_winner_l();
    void on_penalty_winner_r();
    void on_penalty_draw();

    void pozole_goal();

    bool search_ball(); //returns true if agent sees the ball and is aligned to it. It looks for the ball and aligns otherwise
    bool go_to_bal_kick_position(); //returns true if agent is able to kick the ball. Go into position and returns false otherwise
    void kick_to_goal();
    bool im_aligned_to_ball();
    void align_body_to_ball();
};

#endif // GILAGENT_H
