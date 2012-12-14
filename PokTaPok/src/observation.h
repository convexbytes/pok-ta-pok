#ifndef OBSERVATION_H
#define OBSERVATION_H
#include "gameObject.h"
#include <vector>
#include <string>

#ifndef UNDEFINED_NUMBER
#define UNDEFINED_NUMBER -9999999
#endif

using namespace std;

enum ViewModeQuality
{
    LOW,
    HIGH
};

enum ViewModeWidth
{
    NARROW,
    NORMAL,
    WIDE
};

enum FoulCard
{
    FCARD_NONE,
    FCARD_YELLOW,
    FCARD_RED
};

enum Collision
{
    COLLISION_NONE
};

enum FocusTarget
{
    TARGET_NONE
};

enum PlayMode
{
    GOALIE_CATCH_BALL_L = 1,
    GOALIE_CATCH_BALL_R,
    BEFORE_KICK_OFF,
    TIME_OVER,
    PLAY_ON,
    KICK_OFF_L,
    KICK_OFF_R,
    KICK_IN_L,
    KICK_IN_R,
    FREE_KICK_L,
    FREE_KICK_R,
    CORNER_KICK_L,
    CORNER_KICK_R,
    GOAL_KICK_L,
    GOAL_KICK_R,
    GOAL_L,
    GOAL_R,
    DROP_BALL,
    OFFSIDE_L,
    OFFSIDE_R,
    PENALTY_KICK_L,
    PENALTY_KICK_R,
    FIRST_HALF_OVER,
    PAUSE,
    HUMAN_JUDGE,
    FOUL_L,
    FOUL_R,
    FOUL_CHARGE_L,
    FOUL_CHARGE_R,
    FOUL_PUSH_L,
    FOUL_PUSH_R,
    FOUL_MULTIPLE_ATTACK_L,
    FOUL_MULTIPLE_ATTACK_R,
    FOUL_BALLOUT_L,
    FOUL_BALLOUT_R,
    BACK_PASS_L,
    BACK_PASS_R,
    FREE_KICK_FAULT_L,
    FREE_KICK_FAULT_R,
    CATCH_FAULT_L,
    CATCH_FAULT_R,
    INDIRECT_FREE_KICK_L,
    INDIRECT_FREE_KICK_R,
    PENALTY_SETUP_L,
    PENALTY_SETUP_R,
    PENALTY_READY_L,
    PENALTY_READY_R,
    PENALTY_TAKEN_L,
    PENALTY_TAKEN_R,
    PENALTY_MISS_L,
    PENALTY_MISS_R,
    PENALTY_SCORE_L,
    PENALTY_SCORE_R,
    PENALTY_ONFIELD_L,
    PENALTY_ONFIELD_R,
    PENALTY_FOUL_R,
    PENALTY_FOUL_L,
    PENALTY_WINNER_L,
    PENALTY_WINNER_R,
    PENALTY_DRAW,
    YELLOW_CARD_L,
    YELLOW_CARD_R,
    RED_CARD_R,
    RED_CARD_L
};
enum HearSender
{
    SELF,
    OUR,
    OPP,
    REFEREE,
    COUCH
};

enum ObservationType
{
    OBS_SEE,
    OBS_SENSE,
    OBS_HEAR,
    OBS_MSG,
    OBS_INIT,
    OBS_ERROR,
    OBS_OK
};
enum CouchRequest
{
    CHANGE_MODE,
    MOVE,
    START,
    RECOVER,
    EAR,
    CHECK_BALL
};
enum CheckBallPosition
{
    B_IN_FIELD,
    B_GOAL_R,
    B_GOAL_L,
    B_OUT_OF_FIELD
};

enum ErrorType
{
    ILLEGAL_MODE,
    ILLEGAL_OBJECT_FORM,
    ILLEGAL_COMMAND_FORM
};

class Observation
{
public:
    int time;
    ObservationType type;
};

class SenseObs : public Observation
{
public:

    SenseObs();
    ViewModeQuality view_mode_quality; //correspondiente a high o low, se recibe en forma de cadena
    ViewModeWidth   view_mode_width; //correspondiente a narrow, normal o wide, se recibe en forma de cadena
    float   stamina;
    float   effort ;
    int     stamina_capacity;
    float   speed_amount;
    float   speed_direction;
    int     head_angle;
    int     kick; //contador
    int     dash; //contador
    int     turn; //contador
    int     say; //contador
    int     turn_neck; //contador
    int     catchh; //contador
    int     move; //contador
    int     change_view; //contador
    int     arm_movable;
    int     arm_expires;
    int     arm_target_1; //armTargetDistance
    int     arm_target_2; //armTargetDirection
    int     arm_count;   //pointToCount
    char    focus_target_side; /* puede tomar 'n' (none), 'l' (left) o 'r' (right) */
    int     focus_target_number;
    int     focus_count;
    int     tackle_expires;
    int     tackle_count;
    Collision collision; //se recibe en forma de cadena
    int     foul_charged;
    FoulCard foul_card; //se recibe en forma de cadena

    void print();
};

class SeeObs : public Observation
{
public:
    SeeObs();
    bool    ball_is_visible     ();
    int     num_recognized_players  ();
    int     num_recognized_flags    ();
    int     num_recognized_lines    ();
    void    add_player          ( Player player);
    void    add_flag            ( Flag flag);
    void    add_line            ( Line line);
    void    add_ball            ( Ball ball);
    vector<Player> players;
    vector<Flag> flags;
    vector<Flag> recognized_flags;
    vector<Line> lines;
    Ball ball;
    bool ball_visible;
};

class HearObs : public Observation
{
public:
    HearSender sender;
    /* en caso de que el sender sea referee */
};

class HearRefereeObs : public HearObs
{
public:
    HearRefereeObs() { sender = REFEREE;  }
    PlayMode play_mode;
    int num; //Número correspondente a número de jugador, número de goles, etc; dependiendo el play_mode.
};

class HearOurObs : public HearObs
{
public:
    HearOurObs() { sender = OUR; }
    int uniform_number;
    string message;
    int direction;
};

class HearOppObs : public HearObs
{
public:
    HearOppObs() { sender = OUR; }
    int uniform_number;
    string message;
    int direction;
};
class HearCouchObs : public HearObs
{
public:
    HearCouchObs() { sender = COUCH; }
    string message;
};

class HearSelfObs : public HearObs
{
public:
    HearSelfObs() { sender = SELF; }
    string message;
};

class InitObs : public Observation
{
public:
    int uniform_number;
    char side; // 'l' | 'r'
    PlayMode play_mode;
    int playmode_num;
};


class ErrorObs : public Observation
{
public:
    ErrorType error;

};

class OkObs : public Observation
{
public:
    CouchRequest request_type;
    bool ear_on;
    CheckBallPosition ball_pos;
    int ball_time;
};

#endif /* OBSERVATION_H */
