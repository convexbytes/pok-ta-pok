#ifndef GAME_PARAMETER_H
#define GAME_PARAMETER_H
#include <vector>
#include <string>
#include <iostream>

#ifndef UNDEFINED_NUMBER
#define UNDEFINED_NUMBER -9999999
#endif // undefined number


using namespace std;
class PlayerParam
{
public:
    PlayerParam();
    float allow_mult_default_type ;
    float catchable_area_l_stretch_max;
    float catchable_area_l_stretch_min;
    float dash_power_rate_delta_max;
    float dash_power_rate_delta_min;
    float effort_max_delta_factor;
    float effort_min_delta_factor;
    float extra_stamina_delta_max;
    float extra_stamina_delta_min;
    float foul_detect_probability_delta_factor;
    float inertia_moment_delta_factor;
    float kick_power_rate_delta_max;
    float kick_power_rate_delta_min;
    float kick_rand_delta_factor;
    float kickable_margin_delta_max;
    float kickable_margin_delta_min;
    float new_dash_power_rate_delta_max;
    float new_dash_power_rate_delta_min;
    float new_stamina_inc_max_delta_factor;
    float player_decay_delta_max;
    float player_decay_delta_min;
    float player_size_delta_factor;
    float player_speed_max_delta_max;
    float player_speed_max_delta_min;
    float player_types;
    float pt_max;
    float random_seed;
    float stamina_inc_max_delta_factor;
    float subs_max;
};

class ServerParam
{
public:
    ServerParam();
    void print();
    float audio_cut_dist;
    float auto_mode;
    float back_dash_rate;
    float back_passes;
    float ball_accel_max;
    float ball_decay;
    float ball_rand;
    float ball_size;
    float ball_speed_max;
    float ball_stuck_area;
    float ball_weight;
    float catch_ban_cycle;
    float catch_probability;
    float catchable_area_l;
    float catchable_area_w;
    float ckick_margin;
    float clang_advice_win;
    float clang_define_win;
    float clang_del_win;
    float clang_info_win;
    float clang_mess_delay;
    float clang_mess_per_cycle;
    float clang_meta_win;
    float clang_rule_win;
    float clang_win_size;
    float coach;
    string coach_msg_file;
    float coach_port;
    float coach_w_referee;
    float connect_wait;
    float control_radious;
    float dash_angle_step;
    float dash_power_rate;
    float drop_ball_time;
    float effort_dec;
    float effort_dec_thr;
    float effort_dec_inc;
    float effort_inc;
    float effort_inc_thr;
    float effort_init;
    float effort_min;
    float extra_half_time;
    float extra_stamina;
    float forbid_lick_off_offside;
    float foul_cycles;
    float foul_detect_probability;
    float foul_exponent;
    float free_kick_faults;
    float freeform_send_period;
    float freeform_wait_period;
    float fullstate_l;
    float fullstate_r;
    float game_log_compression;
    float game_log_dated;
    string game_log_dir;
    float game_log_fixed;
    string game_log_fixed_name;
    float game_log_version;
    float game_logging;
    float game_over_wait;
    float goal_width;
    float goalie_max_moves;
    float golden_goal;
    float half_time;
    float hear_decay;
    float hear_inc;
    float hear_max;
    float inertia_moment;
    float keepaway;
    float keepaway_length;
    float keepaway_log_dated;
    string keepaway_log_dir;
    float keepaway_log_fixed;
    string keepaway_log_fixed_name;
    float keepaway_logging;
    float keepaway_start;
    float keepaway_width;
    float kick_off_wait;
    float kick_power_rate;
    float kick_rand;
    float kick_rand_factor_l;
    float kick_rand_factor_r;
    float kickable_margin;
    string landmark_file;
    string log_date_format;
    float log_times;
    float max_back_tackle_power;
    float max_dash_angle;
    float max_dash_power;
    float max_goal_kicks;
    //float max_monitors;
    float max_tackle_power;
    float maxmoment;
    float maxneckang;
    float maxneckmoment;
    float maxpower;
    float min_dash_angle;
    float min_dash_power;
    float minmoment;
    float minneckang;
    float minneckmoment;
    float minpower;
    float nr_extra_halfs;
    float nr_normal_halfs;
    float offside_active_area_size;
    float offside_kick_margin;
    float olcoach_port;
    float old_coach_hear;
    float pen_allow_mult_kicks;
    float pen_before_setup_wait;
    float pen_coach_moves_players;
    float pen_dist_x;
    float pen_max_extra_kicks;
    float pen_max_goalie_dist_x;
    float pen_nr_kicks;
    float pen_random_winner;
    float pen_ready_wait;
    float pen_setup_wait;
    float pen_taken_wait;
    float penalty_shoot_outs;
    float player_accel_max;
    float player_decay;
    float player_rand;
    float player_size;
    float player_speed_max;
    float player_speed_max_min;
    float player_weight;
    float point_to_ban;
    float point_to_duration;
    float port;
    float prand_factor_l;
    float prand_factor_r;
    float profile;
    float proper_goal_kicks;
    float quantize_step;
    float quantize_step_l;
    float record_messages;
    float recover_dec;
    float recover_dec_thr;
    float recover_init;
    float recover_min;
    float recv_step;
    float red_card_probability;
    float say_coach_cnt_max;
    float say_coach_msg_size;
    float say_msg_size;
    float send_comms;
    float send_step;
    float send_vi_step;
    float sense_body_step;
    float side_dash_rate;
    float simulator_step;
    float slow_down_factor;
    float slowness_on_top_for_left_team;
    float slowness_on_top_for_right_team;
    float stamina_capacity;
    float stamina_inc_max;
    float stamina_max;
    float start_goal_l;
    float start_goal_r;
    float stopped_ball_vel;
    float synch_micro_sleep;
    float synch_mode;
    float synch_offset;
    float synch_see_offset;
    float tackle_back_dist;
    float tackle_cycles;
    float tackle_dist;
    float tackle_exponent;
    float tackle_power_rate;
    float tackle_rand_factor;
    float tackle_width;
    float team_actuator_noise;
    string team_l_start;
    string team_r_start;
    float text_log_compression;
    float text_log_dated;
    string text_log_dir;
    float text_log_fixed;
    string text_log_fixed_name;
    float text_logging;
    float use_offside;
    float verbose;
    float visible_angle;
    float visible_distance;
    float wind_ang;
    float wind_dir;
    float wind_force;
    float wind_none;
    float wind_rand;
    float wind_random;

};

class PlayerType
{
public:
    PlayerType();
    int id;
    float player_speed_max;
    float stamina_inc_max;
    float player_decay;
    float inertia_moment;
    float dash_power_rate;
    float player_size;
    float kickable_margin;
    float kick_rand;
    float extra_stamina;
    float effort_max;
    float effort_min;
    float kick_power_rate;
    float foul_detect_probability;
    float catchable_area_l_stretch;

    void print();
};

class GameParameter
{
public:
    PlayerParam player_param;
    ServerParam server_param;    
    vector<PlayerType> player_types;

};
#endif /* GAME_PARAMETER_H */
