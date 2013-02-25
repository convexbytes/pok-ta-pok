#ifndef SENSOR_H
#define SENSOR_H
#include "gameObject.h"
#include <vector>
#include <string>

#ifndef UNDEFINED_NUMBER
#define UNDEFINED_NUMBER -9999999
#endif

using namespace std;

enum ViewModeQuality
{
    LOW = 1,
    HIGH
};

enum ViewModeWidth
{
    NARROW = 1,
    NORMAL,
    WIDE
};

enum FoulCard
{
    FCARD_NONE = 1,
    FCARD_YELLOW,
    FCARD_RED
};

enum Collision
{
    COLLISION_NONE = 1
};

enum FocusTarget
{
    TARGET_NONE = 1
};

enum PlayModeHearable // "playmodes" que podemos escuchar del referee
{
    PLAYMODE_NULL = 0,
    GOALIE_CATCH_BALL_L,
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

enum PlayMode { // Tomado directamente del servidor version 15.1
    PM_Null,
    PM_BeforeKickOff,
    PM_TimeOver,
    PM_PlayOn,
    PM_KickOff_Left,
    PM_KickOff_Right,
    PM_KickIn_Left,
    PM_KickIn_Right,
    PM_FreeKick_Left,
    PM_FreeKick_Right,
    PM_CornerKick_Left,
    PM_CornerKick_Right,
    PM_GoalKick_Left,
    PM_GoalKick_Right,
    PM_AfterGoal_Left,
    PM_AfterGoal_Right,
    PM_Drop_Ball,
    PM_OffSide_Left,
    PM_OffSide_Right,
    // [I.Noda:00/05/13] added for 3D viewer/commentator/small league
    PM_PK_Left,
    PM_PK_Right,
    PM_FirstHalfOver,
    PM_Pause,
    PM_Human,
    PM_Foul_Charge_Left,
    PM_Foul_Charge_Right,
    PM_Foul_Push_Left,
    PM_Foul_Push_Right,
    PM_Foul_MultipleAttacker_Left,
    PM_Foul_MultipleAttacker_Right,
    PM_Foul_BallOut_Left,
    PM_Foul_BallOut_Right,
    PM_Back_Pass_Left,
    PM_Back_Pass_Right,
    PM_Free_Kick_Fault_Left,
    PM_Free_Kick_Fault_Right,
    PM_CatchFault_Left,
    PM_CatchFault_Right,
    PM_IndFreeKick_Left,
    PM_IndFreeKick_Right,
    PM_PenaltySetup_Left,
    PM_PenaltySetup_Right,
    PM_PenaltyReady_Left,
    PM_PenaltyReady_Right,
    PM_PenaltyTaken_Left,
    PM_PenaltyTaken_Right,
    PM_PenaltyMiss_Left,
    PM_PenaltyMiss_Right,
    PM_PenaltyScore_Left,
    PM_PenaltyScore_Right,
    PM_MAX
};
/* Tomado directamente del servidor version 15.1.0
 #define PLAYMODE_STRINGS {"",                   \
            "before_kick_off",                  \
            "time_over",                        \
            "play_on",                          \
            "kick_off_l",                       \
            "kick_off_r",                       \
            "kick_in_l",                        \
            "kick_in_r",                        \
            "free_kick_l",                      \
            "free_kick_r",                      \
            "corner_kick_l",                    \
            "corner_kick_r",                    \
            "goal_kick_l",                      \
            "goal_kick_r",                      \
            "goal_l",                           \
            "goal_r",                           \
            "drop_ball",                        \
            "offside_l",                        \
            "offside_r",                        \
            "penalty_kick_l",                   \
            "penalty_kick_r",                   \
            "first_half_over",                  \
            "pause",                            \
            "human_judge",                      \
            "foul_charge_l",                    \
            "foul_charge_r",                    \
            "foul_push_l",                      \
            "foul_push_r",                      \
            "foul_multiple_attack_l",           \
            "foul_multiple_attack_r",           \
            "foul_ballout_l",                   \
            "foul_ballout_r",                   \
            "back_pass_l",                      \
            "back_pass_r",                      \
            "free_kick_fault_l",                \
            "free_kick_fault_r",                \
            "catch_fault_l",                    \
            "catch_fault_r",                    \
            "indirect_free_kick_l",             \
            "indirect_free_kick_r",             \
            "penalty_setup_l",                  \
            "penalty_setup_r",                  \
            "penalty_ready_l",                  \
            "penalty_ready_r",                  \
            "penalty_taken_l",                  \
            "penalty_taken_r",                  \
            "penalty_miss_l",                   \
            "penalty_miss_r",                   \
            "penalty_score_l",                  \
            "penalty_score_r"                   \
            }
 */
enum HearSender
{
    SELF = 1,
    OUR,
    OPP,
    REFEREE,
    COUCH
};

enum SensorType
{
    SENSOR_NONE = 0,
    SENSOR_SEE,
    SENSOR_BODY,
    SENSOR_HEAR,
    SENSOR_MSG,
    SENSOR_INIT,
    SENSOR_ERROR,
    SENSOR_OK,
    SENSOR_SEE_GLOBAL
};
enum Request
{
    CHANGE_MODE = 1,
    MOVE,
    START,
    RECOVER,
    EAR,
    EYE,
    CHECK_BALL,
    SYNCH_SEE,
    LOOK
    
};
enum CheckBallPosition
{
    B_IN_FIELD = 1,
    B_GOAL_R,
    B_GOAL_L,
    B_OUT_OF_FIELD
};

enum ErrorType
{
    ILLEGAL_MODE = 1,
    ILLEGAL_OBJECT_FORM,
    ILLEGAL_COMMAND_FORM
};



class BodySensor
{
public:

    BodySensor();
    ViewModeQuality view_mode_quality; //correspondiente a high o low, se recibe en forma de cadena
    ViewModeWidth   view_mode_width; //correspondiente a narrow, normal o wide, se recibe en forma de cadena
    int     time;
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

class SeeSensor
{
public:
    SeeSensor();
    bool    ballIsVisible     () const { return ball_is_visible; }

    void    add_player          ( Player player);
    void    add_flag            ( Flag flag);
    void    add_line            ( Line line);
    void    add_ball            ( Ball ball);
    vector<Player> players;
    vector<Flag> flags;
    vector<Line> lines;
    int 		 time;
    Ball 	     ball;
    bool 		 ball_is_visible;
};

class HearSensor
{
public:
    int time;
    HearSender sender;
    /* en caso de que el sender sea referee */
};

class HearRefereeSensor : public HearSensor
{
public:
    HearRefereeSensor() { sender = REFEREE;  }
    PlayModeHearable play_mode;
    int num; //Número correspondente a número de jugador, número de goles, etc; dependiendo el play_mode.
};

class HearOurSensor : public HearSensor
{
public:
    HearOurSensor() { sender = OUR; }
    int uniform_number;
    string message;
    int direction;
};

class HearOppSensor : public HearSensor
{
public:
    HearOppSensor() { sender = OUR; }
    int uniform_number;
    string message;
    int direction;
};
class HearCouchSensor : public HearSensor
{
public:
    HearCouchSensor() { sender = COUCH; }
    string message;
};

class HearSelfSensor : public HearSensor
{
public:
    HearSelfSensor() { sender = SELF; }
    string message;
};

class InitSensor
{
public:
    int 		uniform_number;
    char 		side; // 'l' | 'r'
    PlayModeHearable 	play_mode;
    int 		playmode_num;
};


class ErrorSensor
{
public:
    ErrorType error;

};

class SeeGlobalSensor
{
public:
	// Omito (g r) y (g l), ya que son siempre constantes: ((g r) 52.5 0) ((g l) -52.5 0)
	vector<AbsPlayer> 	players;
	AbsBall 			ball;
	int					time;
};

class OkLook // Tiene lo mismo que SeeGlobal hasta la versión 15.1.0 del servidor
{
public:
	// Omito (g r) y (g l), ya que son siempre constantes: ((g r) 52.5 0) ((g l) -52.5 0)
	vector<AbsPlayer> 	players;
	AbsBall 			ball;
	int					time;
};

class OkEar
{
public:
	bool ear_on;
};

class OkEye
{
public:
	bool eye_on;
};

class OkCheckBall
{
public:
	int 			  time;
	CheckBallPosition position;	
};

#endif // SENSOR_H
