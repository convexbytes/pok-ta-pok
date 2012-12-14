#include "obsHandler.h"
#define PRINT_OBS 0

ObsHandler::ObsHandler()
{

}

//Funciones referentes a sense.

void ObsHandler::begin_sense( int time )
{
    last_obs_type = OBS_SENSE;
    SenseObs new_sense;
    last_sense = new_sense;
    last_sense.time = time;
}
void ObsHandler::sense_view_mode_quality( ViewModeQuality view_mode_quality) { last_sense. view_mode_quality = view_mode_quality; }
void ObsHandler::sense_view_mode_width  ( ViewModeWidth view_mode_width) { last_sense. view_mode_width = (ViewModeWidth)view_mode_width; }
void ObsHandler::sense_stamina          ( float stamina) { last_sense.stamina = stamina; }
void ObsHandler::sense_effort           ( float effort) { last_sense.effort = effort; }
void ObsHandler::sense_staminaCapacity  ( int staminaCapacity) { last_sense.stamina_capacity = staminaCapacity; }
void ObsHandler::sense_speed_amount     ( float speed_amount) { last_sense. speed_amount = speed_amount; }
void ObsHandler::sense_speed_direction      ( int speed_direction) { last_sense. speed_direction = speed_direction; }
void ObsHandler::sense_head_angle           ( int head_angle) { last_sense. head_angle = head_angle; }
void ObsHandler::sense_kick         ( int kick) { last_sense. kick = kick; }
void ObsHandler::sense_dash         ( int dash) { last_sense. dash = dash; }
void ObsHandler::sense_turn         ( int turn) { last_sense. turn = turn; }
void ObsHandler::sense_say          ( int say) { last_sense. say = say; }
void ObsHandler::sense_turn_neck    ( int turn_neck) { last_sense. turn_neck = turn_neck; }
void ObsHandler::sense_catchh       ( int catchh) { last_sense. catchh = catchh; }
void ObsHandler::sense_move         ( int move) { last_sense. move = move; }
void ObsHandler::sense_change_view      ( int change_view) { last_sense. change_view = change_view; }
void ObsHandler::sense_arm_movable      ( int arm_movable) { last_sense. arm_movable = arm_movable; }
void ObsHandler::sense_arm_expires      ( int arm_expires) { last_sense. arm_expires = arm_expires; }
void ObsHandler::sense_arm_target_1     ( int arm_target_1) { last_sense. arm_target_1 = arm_target_1; }
void ObsHandler::sense_arm_target_2     ( int arm_target_2) { last_sense. arm_target_2 = arm_target_2; }
void ObsHandler::sense_arm_count        ( int arm_count) { last_sense. arm_count = arm_count; }
void ObsHandler::sense_focus_target     ( char side, int number) { last_sense. focus_target_side = side; last_sense.focus_target_number=number; }
void ObsHandler::sense_focus_count      ( int focus_count) { last_sense. focus_count = focus_count; }
void ObsHandler::sense_tackle_expires   ( int tackle_expires) { last_sense. tackle_expires = tackle_expires; }
void ObsHandler::sense_tackle_count     ( int tackle_count) { last_sense. tackle_count = tackle_count; }
void ObsHandler::sense_collision        ( Collision collision) { last_sense. collision = collision; }
void ObsHandler::sense_foul_charged     ( int foul_charged) { last_sense. foul_charged = foul_charged; }
void ObsHandler::sense_foul_card        ( FoulCard foul_card) { last_sense. foul_card = foul_card; }


//Funciones referentes a see.
void ObsHandler::begin_see( int time )
{
    SeeObs see;
    last_obs_type = OBS_SEE;
    last_see = see;
    last_see.time = time;
}

void ObsHandler::see_player( string team, int unum,  float distance, int direction,
                                float dis_change, float dir_change, float speed_x,
                                float speed_y, int body_dir, int neck_dir)
{
    Player player;
    player.team = team;
    player.uniform_number = unum;
    player.distance = distance;
    player.direction = direction;
    player.distance_change = dis_change;
    player.direction_change = dir_change;
    player.speed_vector.x = speed_x;
    player.speed_vector.y = speed_y;
    player.neck_direction = neck_dir;   /* creo que se da en caso de que el jugador este muy cerca */
    player.body_direction = body_dir;   /* creo que se da en caso de que el jugador este muy cerca */
    last_see.add_player( player );
    if( PRINT_OBS ) player.print();
}

void ObsHandler::see_player(float distance, int direction)
{
    this->see_player("", UNDEFINED_NUMBER, distance, direction, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER,UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}

void ObsHandler::see_player(string team, float distance, int direction)
{
    this->see_player(team, UNDEFINED_NUMBER, distance, direction, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER,UNDEFINED_NUMBER,UNDEFINED_NUMBER);
}
void ObsHandler::see_player(string team, int unum, float distance, int direction)
{
    this->see_player(team, unum, distance, direction, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER,UNDEFINED_NUMBER,UNDEFINED_NUMBER);
}
void ObsHandler::see_player(string team, int unum, float distance, int direction, float dis_change, float dir_change)
{
    this->see_player(team, unum, distance, direction, dis_change, dir_change, UNDEFINED_NUMBER, UNDEFINED_NUMBER,UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}

void ObsHandler::see_player(string team, int unum, float distance, int direction, float dis_change, float dir_change, float speed_x, float speed_y)
{
 this->see_player(team, unum, distance, direction, dis_change, dir_change, speed_x, speed_y,UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}


void ObsHandler::see_flag(EFlag id, float distance, int direction, float dis_chg, float dir_chg)
{
    Flag flag;
    flag.direction = direction;
    flag.distance = distance;
    flag.direction_change = dir_chg;
    flag.distance_change = dis_chg;
    flag.id = id;
    last_see.add_flag( flag );
    if( PRINT_OBS ) flag.print();
}
void ObsHandler::see_flag(float distance, int direction)
{
    this->see_flag(FLAG_UNKNOWN, distance, direction, UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}

void ObsHandler::see_flag(EFlag id, float distance, int direction)
{
    this->see_flag(id, distance, direction, UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}


void ObsHandler::see_line(ELine id, float distance, int direction, float dir_chg, float dis_chg)
{
    Line line;
    line.direction = direction;
    line.distance = distance;
    line.direction_change = dir_chg;
    line.distance_change = dis_chg;
    line.id = id;
    last_see.add_line( line );
    if( PRINT_OBS ) line.print();
}

void ObsHandler::see_line(ELine id, float distance, int direction)
{
    this->see_line(id, distance, direction, UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}


void ObsHandler::see_ball( float distance, int direction, float dis_change,
                             float dir_change , float speed_x, float speed_y)
{
    Ball ball;
    ball.distance = distance;
    ball.direction = direction;
    ball.distance_change = dis_change;
    ball.direction_change = dir_change;
    ball.speed_vector.x = speed_x;
    ball.speed_vector.y = speed_y;
    last_see.add_ball( ball );
    if( PRINT_OBS ) ball.print();
}

void ObsHandler::see_ball(float distance, int direction)
{
    this->see_ball( distance, direction, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}

void ObsHandler::see_ball(float distance, int direction, float dis_change, float dir_change)
{
    this->see_ball( distance, direction, dis_change, dir_change, UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}

void ObsHandler::see_finish()
{
    int i;
    int num_banderas;
    num_banderas = last_see.flags.size();

    for( i=0; i<num_banderas; i++ )
    {
        if( last_see.flags[i].id != FLAG_UNKNOWN )
        {
            last_see.recognized_flags.push_back( last_see.flags[i] );
        }
    }
}

//Funciones referentes a hear.
void ObsHandler::hear_couch(int time, string msg)
{
    last_obs_type = OBS_HEAR;
    last_hear.time = last_hear_couch.time = time;
    last_hear.sender = COUCH;
    last_hear_couch.message = msg;

}

void ObsHandler::hear_our(int time, int direction, int unum, string msg)
{
    last_obs_type = OBS_HEAR;
    last_hear.time = last_hear_our.time = time;
    last_hear.sender = OUR;
    last_hear_our.direction = direction;
    last_hear_our.uniform_number = unum;
    last_hear_our.message = msg;
}

void ObsHandler::hear_opp(int time, int direction, int unum, string msg)
{
    last_obs_type = OBS_HEAR;
    last_hear.time = last_hear_our.time = time;
    last_hear.sender = OPP;
    last_hear_opp.direction = direction;
    last_hear_opp.uniform_number = unum;
    last_hear_opp.message = msg;
}

void ObsHandler::hear_referee(int time, PlayMode play_mode, int num )
{
    last_obs_type = OBS_HEAR;
    last_hear_referee.time = last_hear.time = time;
    last_hear.sender = REFEREE;
    last_hear_referee.play_mode = play_mode;
    last_hear_referee.num = num;
}

void ObsHandler::hear_referee(int time, PlayMode play_mode)
{
    hear_referee(time, play_mode, UNDEFINED_NUMBER );
}

void ObsHandler::hear_self(int time, string msg)
{
    last_obs_type = OBS_HEAR;
    last_hear_self.time = last_hear.time = time;
    last_hear.sender = SELF;
    last_hear_self.message = msg;
}
//Funciones referentes a init.
void ObsHandler::init(char side, int unum, PlayMode play_mode, int playmode_num)
{
    //playmode_num es el número incrustado en los modos de juego, por ejemplo, 2 en goal_r_2.
    last_obs_type = OBS_INIT;
    last_init.side = side;
    last_init.uniform_number = unum;
    last_init.play_mode = play_mode;
    last_init.playmode_num = playmode_num;
}

//Funciones referentes a error.
void ObsHandler::error(int time, ErrorType error)
{
    last_obs_type = OBS_ERROR;
    last_error.time = time;
    last_error.error = error;
}
//Funciones referentes a ok.
void ObsHandler::ok(CouchRequest r_type)
{
    if( r_type == CHECK_BALL || r_type == EAR) return;
    last_obs_type = OBS_OK;
    last_ok.request_type = r_type;
}

void ObsHandler::ok(CouchRequest r_type, bool ear_on)
{
    if( r_type != EAR ) return;
    last_obs_type = OBS_OK;
    last_ok.request_type = r_type;
    last_ok.ear_on = ear_on;
}

void ObsHandler::ok(CouchRequest r_type, int time, CheckBallPosition bal_pos)
{
    if( r_type != CHECK_BALL ) return;
    last_obs_type = OBS_OK;
    last_ok.request_type = r_type;
    last_ok.ball_time = time;
    last_ok.ball_pos = bal_pos;
}
