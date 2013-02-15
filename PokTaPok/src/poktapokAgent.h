#ifndef POKTAPOK_AGENT_H
#define POKTAPOK_AGENT_H
#include "agent.h"
#include "attractorField.h"
#include "obsHandler.h"


#define TEAM_NAME "PokTaPok"


class Jugada
{
public:
    //virtual void execute() = 0;

    bool   M_on;
    int    M_start_time;
    int    M_expire_time;

};

class GoToPoint : public Jugada
{
public:
    //virtual void execute();
    Vector2D M_point;
};

class SearchBall : public Jugada
{
public:
    //virtual void execute();
    double M_turn_body_angle;
    double M_turn_neck_angle;
};

class GoToBall : public Jugada
{

};

class SearchMarkFoe : public Jugada
{

};

class PokStateV1
{
public:
    PokStateV1();

    void update( ObsHandler const & obs,
                 AgentCommand const & command_commited );

    PlayModeHearable playMode() const { return play_mode; }

    // Referentes al agente
    int  time()             const { return last_upd_time; }
    char side()             const { return field_side; }
    int  unum()             const { return uniform_number; }
    Vector2D const & pos()  const { return M_pos; }
    double bodyAngle(  )    const { return M_angle; }
    double neckDirGrad()    const { return M_neck_dir_grad; }


    // Referentes al balón
    Vector2D const & ballPos() const { return M_ball_pos; }
    bool ballIsVisible()    const { return ball_is_visible; }
    int ballLastSeenTime()     const { return M_ball_last_seen_time; }


private:
    void updateOnSense  ( SenseObs const & sense );
    void updateOnInit   ( InitObs  const & init );
    void updateOnSee    ( SeeObs   const & see );

    void updateOnCommandCommit( AgentCommand const & command_commited );

    int uniform_number;
    char field_side;

    Vector2D M_pos;
    double M_angle;


    PlayModeHearable play_mode;
    int last_upd_time;

    // Datos del balón

    Ball M_last_ball;
    Ball M_llast_ball;
    Vector2D M_ball_pos;
    bool M_ball_last_seen_time;
    bool ball_is_visible;
    bool ball_is_kickable;



    // Datos del sense
    double M_neck_dir_grad;

};


class PokTaPokAgentV1 : public Agent
{
public:
    virtual void do_process( GameData *game_data,
                             AgentCommand *agent_response,
                             const AgentCommand *agent_response_commited );



    // Por ahora las haré públicas porque no tengo tiempo de ponerle sus
    // geters y seters
public:

        // Datos de acciones
        GoToPoint  M_go_to_point;
        GoToBall  M_go_to_ball;
        SearchBall M_Search_Ball;

private:
    PokStateV1 state;

    AgentCommand    * command;
    AgentCommand const * command_c;
    GameParameter   * param;
    ObsHandler      * obs_h;

    bool iHaveTheBall();
    bool myTeamHasTheBall();
    bool ballIsKickable();

    // Estrategias

    void attackBallStrategy();
    void attackNoBallStrategy();
    void defendStrategy();


    // Algunas "acciones"
    void searchBall();
    void goToPoint();
    void goToBall();
    void followBall(); // Generalizar a "follow object"

    // Ejecutor de acciones
    void executeActions();


    // Modos de juego
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
};

#endif // POKTAPOK_AGENT_H
