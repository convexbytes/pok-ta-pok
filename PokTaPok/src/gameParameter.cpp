#include "gameParameter.h"
#include <vector>
#include <string>
#include <iostream>

using namespace std;

ServerParam::ServerParam()
{
    this->audio_cut_dist= UNDEFINED_NUMBER;
    this->auto_mode= UNDEFINED_NUMBER;
    this->back_dash_rate= UNDEFINED_NUMBER;
    this->back_passes= UNDEFINED_NUMBER;
    this->ball_accel_max= UNDEFINED_NUMBER;
    this->ball_decay= UNDEFINED_NUMBER;
    this->ball_rand= UNDEFINED_NUMBER;
    this->ball_size= UNDEFINED_NUMBER;
    this->ball_speed_max= UNDEFINED_NUMBER;
    this->ball_stuck_area= UNDEFINED_NUMBER;
    this->ball_weight= UNDEFINED_NUMBER;
    this->catch_ban_cycle= UNDEFINED_NUMBER;
    this->catch_probability= UNDEFINED_NUMBER;
    this->catchable_area_l= UNDEFINED_NUMBER;
    this->catchable_area_w= UNDEFINED_NUMBER;
    this->ckick_margin= UNDEFINED_NUMBER;
    this->clang_advice_win= UNDEFINED_NUMBER;
    this->clang_define_win= UNDEFINED_NUMBER;
    this->clang_del_win= UNDEFINED_NUMBER;
    this->clang_info_win= UNDEFINED_NUMBER;
    this->clang_mess_delay= UNDEFINED_NUMBER;
    this->clang_mess_per_cycle= UNDEFINED_NUMBER;
    this->clang_meta_win= UNDEFINED_NUMBER;
    this->clang_rule_win= UNDEFINED_NUMBER;
    this->clang_win_size= UNDEFINED_NUMBER;
    this->coach= UNDEFINED_NUMBER;
    this->coach_msg_file= "";
    this->coach_port= UNDEFINED_NUMBER;
    this->coach_w_referee= UNDEFINED_NUMBER;
    this->connect_wait= UNDEFINED_NUMBER;
    this->control_radious= UNDEFINED_NUMBER;
    this->dash_angle_step= UNDEFINED_NUMBER;
    this->dash_power_rate= UNDEFINED_NUMBER;
    this->drop_ball_time= UNDEFINED_NUMBER;
    this->effort_dec= UNDEFINED_NUMBER;
    this->effort_dec_thr= UNDEFINED_NUMBER;
    this->effort_dec_inc= UNDEFINED_NUMBER;
    this->effort_inc= UNDEFINED_NUMBER;
    this->effort_inc_thr= UNDEFINED_NUMBER;
    this->effort_init= UNDEFINED_NUMBER;
    this->effort_min= UNDEFINED_NUMBER;
    this->extra_half_time= UNDEFINED_NUMBER;
    this->extra_stamina= UNDEFINED_NUMBER;
    this->forbid_lick_off_offside= UNDEFINED_NUMBER;
    this->foul_cycles= UNDEFINED_NUMBER;
    this->foul_detect_probability= UNDEFINED_NUMBER;
    this->foul_exponent= UNDEFINED_NUMBER;
    this->free_kick_faults= UNDEFINED_NUMBER;
    this->freeform_send_period= UNDEFINED_NUMBER;
    this->freeform_wait_period= UNDEFINED_NUMBER;
    this->fullstate_l= UNDEFINED_NUMBER;
    this->fullstate_r= UNDEFINED_NUMBER;
    this->game_log_compression= UNDEFINED_NUMBER;
    this->game_log_dated= UNDEFINED_NUMBER;
    this->game_log_dir= "";
    this->game_log_fixed= UNDEFINED_NUMBER;
    this->game_log_fixed_name= "";
    this->game_log_version= UNDEFINED_NUMBER;
    this->game_logging= UNDEFINED_NUMBER;
    this->game_over_wait= UNDEFINED_NUMBER;
    this->goal_width= UNDEFINED_NUMBER;
    this->goalie_max_moves= UNDEFINED_NUMBER;
    this->golden_goal= UNDEFINED_NUMBER;
    this->half_time= UNDEFINED_NUMBER;
    this->hear_decay= UNDEFINED_NUMBER;
    this->hear_inc= UNDEFINED_NUMBER;
    this->hear_max= UNDEFINED_NUMBER;
    this->inertia_moment= UNDEFINED_NUMBER;
    this->keepaway= UNDEFINED_NUMBER;
    this->keepaway_length= UNDEFINED_NUMBER;
    this->keepaway_log_dated= UNDEFINED_NUMBER;
    this->keepaway_log_dir= "";
    this->keepaway_log_fixed= UNDEFINED_NUMBER;
    this->keepaway_log_fixed_name= "";
    this->keepaway_logging= UNDEFINED_NUMBER;
    this->keepaway_start= UNDEFINED_NUMBER;
    this->keepaway_width= UNDEFINED_NUMBER;
    this->kick_off_wait= UNDEFINED_NUMBER;
    this->kick_power_rate= UNDEFINED_NUMBER;
    this->kick_rand= UNDEFINED_NUMBER;
    this->kick_rand_factor_l= UNDEFINED_NUMBER;
    this->kick_rand_factor_r= UNDEFINED_NUMBER;
    this->kickable_margin= UNDEFINED_NUMBER;
    this->landmark_file= "";
    this->log_date_format= "";
    this->log_times= UNDEFINED_NUMBER;
    this->max_back_tackle_power= UNDEFINED_NUMBER;
    this->max_dash_angle= UNDEFINED_NUMBER;
    this->max_dash_power= UNDEFINED_NUMBER;
    this->max_goal_kicks= UNDEFINED_NUMBER;
    //this->max_monitors= UNDEFINED_NUMBER;
    this->max_tackle_power= UNDEFINED_NUMBER;
    this->maxmoment= UNDEFINED_NUMBER;
    this->maxneckang= UNDEFINED_NUMBER;
    this->maxneckmoment= UNDEFINED_NUMBER;
    this->maxpower= UNDEFINED_NUMBER;
    this->min_dash_angle= UNDEFINED_NUMBER;
    this->min_dash_power= UNDEFINED_NUMBER;
    this->minmoment= UNDEFINED_NUMBER;
    this->minneckang= UNDEFINED_NUMBER;
    this->minneckmoment= UNDEFINED_NUMBER;
    this->minpower= UNDEFINED_NUMBER;
    this->nr_extra_halfs= UNDEFINED_NUMBER;
    this->nr_normal_halfs= UNDEFINED_NUMBER;
    this->offside_active_area_size= UNDEFINED_NUMBER;
    this->offside_kick_margin= UNDEFINED_NUMBER;
    this->olcoach_port= UNDEFINED_NUMBER;
    this->old_coach_hear= UNDEFINED_NUMBER;
    this->pen_allow_mult_kicks= UNDEFINED_NUMBER;
    this->pen_before_setup_wait= UNDEFINED_NUMBER;
    this->pen_coach_moves_players= UNDEFINED_NUMBER;
    this->pen_dist_x= UNDEFINED_NUMBER;
    this->pen_max_extra_kicks= UNDEFINED_NUMBER;
    this->pen_max_goalie_dist_x= UNDEFINED_NUMBER;
    this->pen_nr_kicks= UNDEFINED_NUMBER;
    this->pen_random_winner= UNDEFINED_NUMBER;
    this->pen_ready_wait= UNDEFINED_NUMBER;
    this->pen_setup_wait= UNDEFINED_NUMBER;
    this->pen_taken_wait= UNDEFINED_NUMBER;
    this->penalty_shoot_outs= UNDEFINED_NUMBER;
    this->player_accel_max= UNDEFINED_NUMBER;
    this->player_decay= UNDEFINED_NUMBER;
    this->player_rand= UNDEFINED_NUMBER;
    this->player_size= UNDEFINED_NUMBER;
    this->player_speed_max= UNDEFINED_NUMBER;
    this->player_speed_max_min= UNDEFINED_NUMBER;
    this->player_weight= UNDEFINED_NUMBER;
    this->point_to_ban= UNDEFINED_NUMBER;
    this->point_to_duration= UNDEFINED_NUMBER;
    this->port= UNDEFINED_NUMBER;
    this->prand_factor_l= UNDEFINED_NUMBER;
    this->prand_factor_r= UNDEFINED_NUMBER;
    this->profile= UNDEFINED_NUMBER;
    this->proper_goal_kicks= UNDEFINED_NUMBER;
    this->quantize_step= UNDEFINED_NUMBER;
    this->quantize_step_l= UNDEFINED_NUMBER;
    this->record_messages= UNDEFINED_NUMBER;
    this->recover_dec= UNDEFINED_NUMBER;
    this->recover_dec_thr= UNDEFINED_NUMBER;
    this->recover_init= UNDEFINED_NUMBER;
    this->recover_min= UNDEFINED_NUMBER;
    this->recv_step= UNDEFINED_NUMBER;
    this->red_card_probability= UNDEFINED_NUMBER;
    this->say_coach_cnt_max= UNDEFINED_NUMBER;
    this->say_coach_msg_size= UNDEFINED_NUMBER;
    this->say_msg_size= UNDEFINED_NUMBER;
    this->send_comms= UNDEFINED_NUMBER;
    this->send_step= UNDEFINED_NUMBER;
    this->send_vi_step= UNDEFINED_NUMBER;
    this->sense_body_step= UNDEFINED_NUMBER;
    this->side_dash_rate= UNDEFINED_NUMBER;
    this->simulator_step= UNDEFINED_NUMBER;
    this->slow_down_factor= UNDEFINED_NUMBER;
    this->slowness_on_top_for_left_team= UNDEFINED_NUMBER;
    this->slowness_on_top_for_right_team= UNDEFINED_NUMBER;
    this->stamina_capacity= UNDEFINED_NUMBER;
    this->stamina_inc_max= UNDEFINED_NUMBER;
    this->stamina_max= UNDEFINED_NUMBER;
    this->start_goal_l= UNDEFINED_NUMBER;
    this->start_goal_r= UNDEFINED_NUMBER;
    this->stopped_ball_vel= UNDEFINED_NUMBER;
    this->synch_micro_sleep= UNDEFINED_NUMBER;
    this->synch_mode= UNDEFINED_NUMBER;
    this->synch_offset= UNDEFINED_NUMBER;
    this->synch_see_offset= UNDEFINED_NUMBER;
    this->tackle_back_dist= UNDEFINED_NUMBER;
    this->tackle_cycles= UNDEFINED_NUMBER;
    this->tackle_dist= UNDEFINED_NUMBER;
    this->tackle_exponent= UNDEFINED_NUMBER;
    this->tackle_power_rate= UNDEFINED_NUMBER;
    this->tackle_rand_factor= UNDEFINED_NUMBER;
    this->tackle_width= UNDEFINED_NUMBER;
    this->team_actuator_noise= UNDEFINED_NUMBER;
    this->team_l_start= "";
    this->team_r_start= "";
    this->text_log_compression= UNDEFINED_NUMBER;
    this->text_log_dated= UNDEFINED_NUMBER;
    this->text_log_dir= "";
    this->text_log_fixed= UNDEFINED_NUMBER;
    this->text_log_fixed_name= "";
    this->text_logging= UNDEFINED_NUMBER;
    this->use_offside= UNDEFINED_NUMBER;
    this->verbose= UNDEFINED_NUMBER;
    this->visible_angle= UNDEFINED_NUMBER;
    this->visible_distance= UNDEFINED_NUMBER;
    this->wind_ang= UNDEFINED_NUMBER;
    this->wind_dir= UNDEFINED_NUMBER;
    this->wind_force= UNDEFINED_NUMBER;
    this->wind_none= UNDEFINED_NUMBER;
    this->wind_rand= UNDEFINED_NUMBER;
    this->wind_random= UNDEFINED_NUMBER;
}
void ServerParam::print()
{
    cout << "\naudio_cut_dist: " << this->audio_cut_dist;
    cout << "\n    auto_mode: " << this->    auto_mode;
    cout << "\n    back_dash_rate: " << this->    back_dash_rate;
    cout << "\n    back_passes: " << this->    back_passes;
    cout << "\n    ball_accel_max: " << this->    ball_accel_max;
    cout << "\n    ball_decay: " << this->    ball_decay;
    cout << "\n    ball_rand: " << this->    ball_rand;
    cout << "\n    ball_size: " << this->    ball_size;
    cout << "\n    ball_speed_max: " << this->    ball_speed_max;
    cout << "\n    ball_stuck_area: " << this->    ball_stuck_area;
    cout << "\n    ball_weight: " << this->    ball_weight;
    cout << "\n    catch_ban_cycle: " << this->    catch_ban_cycle;
    cout << "\n    catch_probability: " << this->    catch_probability;
    cout << "\n    catchable_area_l: " << this->    catchable_area_l;
    cout << "\n    catchable_area_w: " << this->    catchable_area_w;
    cout << "\n    ckick_margin: " << this->    ckick_margin;
    cout << "\n    clang_advice_win: " << this->    clang_advice_win;
    cout << "\n    clang_define_win: " << this->    clang_define_win;
    cout << "\n    clang_del_win: " << this->    clang_del_win;
    cout << "\n    clang_info_win: " << this->    clang_info_win;
    cout << "\n    clang_mess_delay: " << this->    clang_mess_delay;
    cout << "\n    clang_mess_per_cycle: " << this->    clang_mess_per_cycle;
    cout << "\n    clang_meta_win: " << this->    clang_meta_win;
    cout << "\n    clang_rule_win: " << this->    clang_rule_win;
    cout << "\n    clang_win_size: " << this->    clang_win_size;
    cout << "\n    coach: " << this->    coach;
    cout << "\n    string coach_msg_file: " << this->   coach_msg_file;
    cout << "\n    coach_port: " << this->    coach_port;
    cout << "\n    coach_w_referee: " << this->    coach_w_referee;
    cout << "\n    connect_wait: " << this->    connect_wait;
    cout << "\n    control_radious: " << this->    control_radious;
    cout << "\n    dash_angle_step: " << this->    dash_angle_step;
    cout << "\n    dash_power_rate: " << this->    dash_power_rate;
    cout << "\n    drop_ball_time: " << this->    drop_ball_time;
    cout << "\n    effort_dec: " << this->    effort_dec;
    cout << "\n    effort_dec_thr: " << this->    effort_dec_thr;
    cout << "\n    effort_dec_inc: " << this->    effort_dec_inc;
    cout << "\n    effort_inc: " << this->    effort_inc;
    cout << "\n    effort_inc_thr: " << this->    effort_inc_thr;
    cout << "\n    effort_init: " << this->    effort_init;
    cout << "\n    effort_min: " << this->    effort_min;
    cout << "\n    extra_half_time: " << this->    extra_half_time;
    cout << "\n    extra_stamina: " << this->    extra_stamina;
    cout << "\n    forbid_lick_off_offside: " << this->    forbid_lick_off_offside;
    cout << "\n    foul_cycles: " << this->    foul_cycles;
    cout << "\n    foul_detect_probability: " << this->    foul_detect_probability;
    cout << "\n    foul_exponent: " << this->    foul_exponent;
    cout << "\n    free_kick_faults: " << this->    free_kick_faults;
    cout << "\n    freeform_send_period: " << this->    freeform_send_period;
    cout << "\n    freeform_wait_period: " << this->    freeform_wait_period;
    cout << "\n    fullstate_l: " << this->    fullstate_l;
    cout << "\n    fullstate_r: " << this->    fullstate_r;
    cout << "\n    game_log_compression: " << this->    game_log_compression;
    cout << "\n    game_log_dated: " << this->    game_log_dated;
    cout << "\n    string game_log_dir: " << this->     game_log_dir;
    cout << "\n    game_log_fixed: " << this->    game_log_fixed;
    cout << "\n    string game_log_fixed_name: " << this->     game_log_fixed_name;
    cout << "\n    game_log_version: " << this->    game_log_version;
    cout << "\n    game_logging: " << this->    game_logging;
    cout << "\n    game_over_wait: " << this->    game_over_wait;
    cout << "\n    goal_width: " << this->    goal_width;
    cout << "\n    goalie_max_moves: " << this->    goalie_max_moves;
    cout << "\n    golden_goal: " << this->    golden_goal;
    cout << "\n    half_time: " << this->    half_time;
    cout << "\n    hear_decay: " << this->    hear_decay;
    cout << "\n    hear_inc: " << this->    hear_inc;
    cout << "\n    hear_max: " << this->    hear_max;
    cout << "\n    inertia_moment: " << this->    inertia_moment;
    cout << "\n    keepaway: " << this->    keepaway;
    cout << "\n    keepaway_length: " << this->    keepaway_length;
    cout << "\n    keepaway_log_dated: " << this->    keepaway_log_dated;
    cout << "\n    string keepaway_log_dir: " << this->     keepaway_log_dir;
    cout << "\n    keepaway_log_fixed: " << this->    keepaway_log_fixed;
    cout << "\n    string keepaway_log_fixed_name: " << this->     keepaway_log_fixed_name;
    cout << "\n    keepaway_logging: " << this->    keepaway_logging;
    cout << "\n    keepaway_start: " << this->    keepaway_start;
    cout << "\n    keepaway_width: " << this->    keepaway_width;
    cout << "\n    kick_off_wait: " << this->    kick_off_wait;
    cout << "\n    kick_power_rate: " << this->    kick_power_rate;
    cout << "\n    kick_rand: " << this->    kick_rand;
    cout << "\n    kick_rand_factor_l: " << this->    kick_rand_factor_l;
    cout << "\n    kick_rand_factor_r: " << this->    kick_rand_factor_r;
    cout << "\n    kickable_margin: " << this->    kickable_margin;
    cout << "\n    string landmark_file: " << this->     landmark_file;
    cout << "\n    string log_date_format: " << this->     log_date_format;
    cout << "\n    log_times: " << this->    log_times;
    cout << "\n    max_back_tackle_power: " << this->    max_back_tackle_power;
    cout << "\n    max_dash_angle: " << this->    max_dash_angle;
    cout << "\n    max_dash_power: " << this->    max_dash_power;
    cout << "\n    max_goal_kicks: " << this->    max_goal_kicks;
    //cout << "\n    max_monitors: " << this->    max_monitors;
    cout << "\n    max_tackle_power: " << this->    max_tackle_power;
    cout << "\n    maxmoment: " << this->    maxmoment;
    cout << "\n    maxneckang: " << this->    maxneckang;
    cout << "\n    maxneckmoment: " << this->    maxneckmoment;
    cout << "\n    maxpower: " << this->    maxpower;
    cout << "\n    min_dash_angle: " << this->    min_dash_angle;
    cout << "\n    min_dash_power: " << this->    min_dash_power;
    cout << "\n    minmoment: " << this->    minmoment;
    cout << "\n    minneckang: " << this->    minneckang;
    cout << "\n    minneckmoment: " << this->    minneckmoment;
    cout << "\n    minpower: " << this->    minpower;
    cout << "\n    nr_extra_halfs: " << this->    nr_extra_halfs;
    cout << "\n    nr_normal_halfs: " << this->    nr_normal_halfs;
    cout << "\n    offside_active_area_size: " << this->    offside_active_area_size;
    cout << "\n    offside_kick_margin: " << this->    offside_kick_margin;
    cout << "\n    olcoach_port: " << this->    olcoach_port;
    cout << "\n    old_coach_hear: " << this->    old_coach_hear;
    cout << "\n    pen_allow_mult_kicks: " << this->    pen_allow_mult_kicks;
    cout << "\n    pen_before_setup_wait: " << this->    pen_before_setup_wait;
    cout << "\n    pen_coach_moves_players: " << this->    pen_coach_moves_players;
    cout << "\n    pen_dist_x: " << this->    pen_dist_x;
    cout << "\n    pen_max_extra_kicks: " << this->    pen_max_extra_kicks;
    cout << "\n    pen_max_goalie_dist_x: " << this->    pen_max_goalie_dist_x;
    cout << "\n    pen_nr_kicks: " << this->    pen_nr_kicks;
    cout << "\n    pen_random_winner: " << this->    pen_random_winner;
    cout << "\n    pen_ready_wait: " << this->    pen_ready_wait;
    cout << "\n    pen_setup_wait: " << this->    pen_setup_wait;
    cout << "\n    pen_taken_wait: " << this->    pen_taken_wait;
    cout << "\n    penalty_shoot_outs: " << this->    penalty_shoot_outs;
    cout << "\n    player_accel_max: " << this->    player_accel_max;
    cout << "\n    player_decay: " << this->    player_decay;
    cout << "\n    player_rand: " << this->    player_rand;
    cout << "\n    player_size: " << this->    player_size;
    cout << "\n    player_speed_max: " << this->    player_speed_max;
    cout << "\n    player_speed_max_min: " << this->    player_speed_max_min;
    cout << "\n    player_weight: " << this->    player_weight;
    cout << "\n    point_to_ban: " << this->    point_to_ban;
    cout << "\n    point_to_duration: " << this->    point_to_duration;
    cout << "\n    port: " << this->    port;
    cout << "\n    prand_factor_l: " << this->    prand_factor_l;
    cout << "\n    prand_factor_r: " << this->    prand_factor_r;
    cout << "\n    profile: " << this->    profile;
    cout << "\n    proper_goal_kicks: " << this->    proper_goal_kicks;
    cout << "\n    quantize_step: " << this->    quantize_step;
    cout << "\n    quantize_step_l: " << this->    quantize_step_l;
    cout << "\n    record_messages: " << this->    record_messages;
    cout << "\n    recover_dec: " << this->    recover_dec;
    cout << "\n    recover_dec_thr: " << this->    recover_dec_thr;
    cout << "\n    recover_init: " << this->    recover_init;
    cout << "\n    recover_min: " << this->    recover_min;
    cout << "\n    recv_step: " << this->    recv_step;
    cout << "\n    red_card_probability: " << this->    red_card_probability;
    cout << "\n    say_coach_cnt_max: " << this->    say_coach_cnt_max;
    cout << "\n    say_coach_msg_size: " << this->    say_coach_msg_size;
    cout << "\n    say_msg_size: " << this->    say_msg_size;
    cout << "\n    send_comms: " << this->    send_comms;
    cout << "\n    send_step: " << this->    send_step;
    cout << "\n    send_vi_step: " << this->    send_vi_step;
    cout << "\n    sense_body_step: " << this->    sense_body_step;
    cout << "\n    side_dash_rate: " << this->    side_dash_rate;
    cout << "\n    simulator_step: " << this->    simulator_step;
    cout << "\n    slow_down_factor: " << this->    slow_down_factor;
    cout << "\n    slowness_on_top_for_left_team: " << this->    slowness_on_top_for_left_team;
    cout << "\n    slowness_on_top_for_right_team: " << this->    slowness_on_top_for_right_team;
    cout << "\n    stamina_capacity: " << this->    stamina_capacity;
    cout << "\n    stamina_inc_max: " << this->    stamina_inc_max;
    cout << "\n    stamina_max: " << this->    stamina_max;
    cout << "\n    start_goal_l: " << this->    start_goal_l;
    cout << "\n    start_goal_r: " << this->    start_goal_r;
    cout << "\n    stopped_ball_vel: " << this->    stopped_ball_vel;
    cout << "\n    synch_micro_sleep: " << this->    synch_micro_sleep;
    cout << "\n    synch_mode: " << this->    synch_mode;
    cout << "\n    synch_offset: " << this->    synch_offset;
    cout << "\n    synch_see_offset: " << this->    synch_see_offset;
    cout << "\n    tackle_back_dist: " << this->    tackle_back_dist;
    cout << "\n    tackle_cycles: " << this->    tackle_cycles;
    cout << "\n    tackle_dist: " << this->    tackle_dist;
    cout << "\n    tackle_exponent: " << this->    tackle_exponent;
    cout << "\n    tackle_power_rate: " << this->    tackle_power_rate;
    cout << "\n    tackle_rand_factor: " << this->    tackle_rand_factor;
    cout << "\n    tackle_width: " << this->    tackle_width;
    cout << "\n    team_actuator_noise: " << this->    team_actuator_noise;
    cout << "\n    string team_l_start: " << this->     team_l_start;
    cout << "\n    string team_r_start: " << this->     team_r_start;
    cout << "\n    text_log_compression: " << this->    text_log_compression;
    cout << "\n    text_log_dated: " << this->    text_log_dated;
    cout << "\n    string text_log_dir: " << this->     text_log_dir;
    cout << "\n    text_log_fixed: " << this->    text_log_fixed;
    cout << "\n    string text_log_fixed_name: " << this->text_log_fixed_name;
    cout << "\n    text_logging: " << this->    text_logging;
    cout << "\n    use_offside: " << this->    use_offside;
    cout << "\n    verbose: " << this->    verbose;
    cout << "\n    visible_angle: " << this->    visible_angle;
    cout << "\n    visible_distance: " << this->    visible_distance;
    cout << "\n    wind_ang: " << this->    wind_ang;
    cout << "\n    wind_dir: " << this->    wind_dir;
    cout << "\n    wind_force: " << this->    wind_force;
    cout << "\n    wind_none: " << this->    wind_none;
    cout << "\n    wind_rand: " << this->    wind_rand;
    cout << "\n    wind_random: " << this->    wind_random;

}

PlayerType::PlayerType()
{
    this->id = UNDEFINED_NUMBER;
    this->player_speed_max= UNDEFINED_NUMBER;
    this->stamina_inc_max= UNDEFINED_NUMBER;
    this->player_decay= UNDEFINED_NUMBER;
    this->inertia_moment= UNDEFINED_NUMBER;
    this->dash_power_rate= UNDEFINED_NUMBER;
    this->player_size= UNDEFINED_NUMBER;
    this->kickable_margin= UNDEFINED_NUMBER;
    this->kick_rand= UNDEFINED_NUMBER;
    this->extra_stamina= UNDEFINED_NUMBER;
    this->effort_max= UNDEFINED_NUMBER;
    this->effort_min= UNDEFINED_NUMBER;
    this->kick_power_rate= UNDEFINED_NUMBER;
    this->foul_detect_probability= UNDEFINED_NUMBER;
    this->catchable_area_l_stretch= UNDEFINED_NUMBER;
}
void PlayerType::print()
{
    cout << "\nid: " << this->id;
    cout << "\nplayer_speed_max: " << this->player_speed_max;
    cout << "\nstamina_inc_max: " << this->stamina_inc_max;
    cout << "\nplayer_decay: " << this->player_decay;
    cout << "\ninertia_moment: " << this->inertia_moment;
    cout << "\ndash_power_rate: " << this->dash_power_rate;
    cout << "\nplayer_size: " << this->player_size;
    cout << "\nkickable_margin: " << this->kickable_margin;
    cout << "\nkick_rand: " << this->kick_rand;
    cout << "\nextra_stamina: " << this->extra_stamina;
    cout << "\neffort_max: " << this->effort_max;
    cout << "\neffor_min: " << this->effort_min;
    cout << "\nkick_power_rate: " << this->kick_power_rate;
    cout << "\nfoul_detect_probability: " << this->foul_detect_probability;
    cout << "\ncatchable_area_l_stretch: " << this->catchable_area_l_stretch;
}

PlayerParam::PlayerParam()
{
    this->allow_mult_default_type = UNDEFINED_NUMBER;
    this->catchable_area_l_stretch_max = UNDEFINED_NUMBER;
    this->catchable_area_l_stretch_min = UNDEFINED_NUMBER;
    this->dash_power_rate_delta_max = UNDEFINED_NUMBER;
    this->dash_power_rate_delta_min = UNDEFINED_NUMBER;
    this->effort_max_delta_factor = UNDEFINED_NUMBER;
    this->effort_min_delta_factor = UNDEFINED_NUMBER;
    this->extra_stamina_delta_max = UNDEFINED_NUMBER;
    this->extra_stamina_delta_min = UNDEFINED_NUMBER;
    this->foul_detect_probability_delta_factor = UNDEFINED_NUMBER;
    this->inertia_moment_delta_factor = UNDEFINED_NUMBER;
    this->kick_power_rate_delta_max = UNDEFINED_NUMBER;
    this->kick_power_rate_delta_min = UNDEFINED_NUMBER;
    this->kick_rand_delta_factor = UNDEFINED_NUMBER;
    this->kickable_margin_delta_max = UNDEFINED_NUMBER;
    this->kickable_margin_delta_min = UNDEFINED_NUMBER;
    this->new_dash_power_rate_delta_max = UNDEFINED_NUMBER;
    this->new_dash_power_rate_delta_min = UNDEFINED_NUMBER;
    this->new_stamina_inc_max_delta_factor = UNDEFINED_NUMBER;
    this->player_decay_delta_max = UNDEFINED_NUMBER;
    this->player_decay_delta_min = UNDEFINED_NUMBER;
    this->player_size_delta_factor = UNDEFINED_NUMBER;
    this->player_speed_max_delta_max = UNDEFINED_NUMBER;
    this->player_speed_max_delta_min = UNDEFINED_NUMBER;
    this->player_types = UNDEFINED_NUMBER;
    this->pt_max = UNDEFINED_NUMBER;
    this->random_seed = UNDEFINED_NUMBER;
    this->stamina_inc_max_delta_factor = UNDEFINED_NUMBER;
    this->subs_max = UNDEFINED_NUMBER;
}