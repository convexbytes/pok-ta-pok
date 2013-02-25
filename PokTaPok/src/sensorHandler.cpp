#include "sensorHandler.h"
#define PRINT_SENSOR 0

SensorHandler::SensorHandler()
{
    last_sensor_type = SENSOR_NONE;
}

//Funciones referentes a sense.

void SensorHandler::begin_sense( int time )
{
    BodySensor new_sense;
    last_sensor_type =	SENSOR_SENSE;
    last_sense 	  =	new_sense;
    last_sense.time = time;
}
void SensorHandler::sense_view_mode_quality( ViewModeQuality view_mode_quality) { last_sense. view_mode_quality = view_mode_quality; }
void SensorHandler::sense_view_mode_width  ( ViewModeWidth view_mode_width) { last_sense. view_mode_width = (ViewModeWidth)view_mode_width; }
void SensorHandler::sense_stamina          ( float stamina) { last_sense.stamina = stamina; }
void SensorHandler::sense_effort           ( float effort) { last_sense.effort = effort; }
void SensorHandler::sense_staminaCapacity  ( int staminaCapacity) { last_sense.stamina_capacity = staminaCapacity; }
void SensorHandler::sense_speed_amount     ( float speed_amount) { last_sense. speed_amount = speed_amount; }
void SensorHandler::sense_speed_direction      ( int speed_direction) { last_sense. speed_direction = speed_direction; }
void SensorHandler::sense_head_angle           ( int head_angle) { last_sense. head_angle = head_angle; }
void SensorHandler::sense_kick         ( int kick) { last_sense. kick = kick; }
void SensorHandler::sense_dash         ( int dash) { last_sense. dash = dash; }
void SensorHandler::sense_turn         ( int turn) { last_sense. turn = turn; }
void SensorHandler::sense_say          ( int say) { last_sense. say = say; }
void SensorHandler::sense_turn_neck    ( int turn_neck) { last_sense. turn_neck = turn_neck; }
void SensorHandler::sense_catchh       ( int catchh) { last_sense. catchh = catchh; }
void SensorHandler::sense_move         ( int move) { last_sense. move = move; }
void SensorHandler::sense_change_view      ( int change_view) { last_sense. change_view = change_view; }
void SensorHandler::sense_arm_movable      ( int arm_movable) { last_sense. arm_movable = arm_movable; }
void SensorHandler::sense_arm_expires      ( int arm_expires) { last_sense. arm_expires = arm_expires; }
void SensorHandler::sense_arm_target_1     ( int arm_target_1) { last_sense. arm_target_1 = arm_target_1; }
void SensorHandler::sense_arm_target_2     ( int arm_target_2) { last_sense. arm_target_2 = arm_target_2; }
void SensorHandler::sense_arm_count        ( int arm_count) { last_sense. arm_count = arm_count; }
void SensorHandler::sense_focus_target     ( char side, int number) { last_sense. focus_target_side = side; last_sense.focus_target_number=number; }
void SensorHandler::sense_focus_count      ( int focus_count) { last_sense. focus_count = focus_count; }
void SensorHandler::sense_tackle_expires   ( int tackle_expires) { last_sense. tackle_expires = tackle_expires; }
void SensorHandler::sense_tackle_count     ( int tackle_count) { last_sense. tackle_count = tackle_count; }
void SensorHandler::sense_collision        ( Collision collision) { last_sense. collision = collision; }
void SensorHandler::sense_foul_charged     ( int foul_charged) { last_sense. foul_charged = foul_charged; }
void SensorHandler::sense_foul_card        ( FoulCard foul_card) { last_sense. foul_card = foul_card; }


//Funciones referentes a see.
void SensorHandler::begin_see( int time )
{
    SeeSensor see;
    last_sensor_type= SENSOR_SEE;
    last_see 		= see;
    last_see.time 	= time;
}

void SensorHandler::see_player( string team, int unum,  float distance, int direction,
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
    if( PRINT_SENSOR ) player.print();
}

void SensorHandler::see_player(float distance, int direction)
{
    this->see_player("", UNDEFINED_NUMBER, distance, direction, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER,UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}

void SensorHandler::see_player(string team, float distance, int direction)
{
    this->see_player(team, UNDEFINED_NUMBER, distance, direction, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER,UNDEFINED_NUMBER,UNDEFINED_NUMBER);
}
void SensorHandler::see_player(string team, int unum, float distance, int direction)
{
    this->see_player(team, unum, distance, direction, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER,UNDEFINED_NUMBER,UNDEFINED_NUMBER);
}
void SensorHandler::see_player(string team, int unum, float distance, int direction, float dis_change, float dir_change)
{
    this->see_player(team, unum, distance, direction, dis_change, dir_change, UNDEFINED_NUMBER, UNDEFINED_NUMBER,UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}

void SensorHandler::see_player(string team, int unum, float distance, int direction, float dis_change, float dir_change, float speed_x, float speed_y)
{
 this->see_player(team, unum, distance, direction, dis_change, dir_change, speed_x, speed_y,UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}


void SensorHandler::see_flag(EFlag id, float distance, int direction, float dis_chg, float dir_chg)
{
    Flag flag;
    flag.direction = direction;
    flag.distance = distance;
    flag.direction_change = dir_chg;
    flag.distance_change = dis_chg;
    flag.id = id;
    last_see.add_flag( flag );
    if( PRINT_SENSOR ) flag.print();
}
void SensorHandler::see_flag(float distance, int direction)
{
    this->see_flag(FLAG_UNKNOWN, distance, direction, UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}

void SensorHandler::see_flag(EFlag id, float distance, int direction)
{
    this->see_flag(id, distance, direction, UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}


void SensorHandler::see_line(ELine id, float distance, int direction, float dir_chg, float dis_chg)
{
    Line line;
    line.direction = direction;
    line.distance = distance;
    line.direction_change = dir_chg;
    line.distance_change = dis_chg;
    line.id = id;
    last_see.add_line( line );
    if( PRINT_SENSOR ) line.print();
}

void SensorHandler::see_line(ELine id, float distance, int direction)
{
    this->see_line(id, distance, direction, UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}


void SensorHandler::see_ball( float distance, int direction, float dis_change,
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
    if( PRINT_SENSOR ) ball.print();
}

void SensorHandler::see_ball(float distance, int direction)
{
    this->see_ball( distance, direction, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}

void SensorHandler::see_ball(float distance, int direction, float dis_change, float dir_change)
{
    this->see_ball( distance, direction, dis_change, dir_change, UNDEFINED_NUMBER, UNDEFINED_NUMBER);
}

void SensorHandler::see_finish()
{
    int i;
    int num_banderas;
    num_banderas = last_see.flags.size();
    last_see.recognized_flags.clear();
    for( i=0; i<num_banderas; i++ )
    {
        if( last_see.flags[i].id != FLAG_UNKNOWN )
        {
            last_see.recognized_flags.push_back( last_see.flags[i] );
        }
    }
}

//Funciones referentes a hear.
void SensorHandler::hear_couch(int time, string msg)
{
    last_sensor_type = SENSOR_HEAR;
    last_hear.time = last_hear_couch.time = time;
    last_hear.sender = COUCH;
    last_hear_couch.message = msg;

}

void SensorHandler::hear_our(int time, int direction, int unum, string msg)
{
    last_sensor_type = SENSOR_HEAR;
    last_hear.time = last_hear_our.time = time;
    last_hear.sender = OUR;
    last_hear_our.direction = direction;
    last_hear_our.uniform_number = unum;
    last_hear_our.message = msg;
}

void SensorHandler::hear_opp(int time, int direction, int unum, string msg)
{
    last_sensor_type = SENSOR_HEAR;
    last_hear.time = last_hear_our.time = time;
    last_hear.sender = OPP;
    last_hear_opp.direction = direction;
    last_hear_opp.uniform_number = unum;
    last_hear_opp.message = msg;
}

void SensorHandler::hear_referee(int time, PlayModeHearable play_mode, int num )
{
    last_sensor_type = SENSOR_HEAR;
    last_hear_referee.time = last_hear.time = time;
    last_hear.sender = REFEREE;
    last_hear_referee.play_mode = play_mode;
    last_hear_referee.num = num;
}

void SensorHandler::hear_referee(int time, PlayModeHearable play_mode)
{
    hear_referee(time, play_mode, UNDEFINED_NUMBER );
}

void SensorHandler::hear_self(int time, string msg)
{
    last_sensor_type = SENSOR_HEAR;
    last_hear_self.time = last_hear.time = time;
    last_hear.sender = SELF;
    last_hear_self.message = msg;
}
//Funciones referentes a init.
void SensorHandler::init(char side, int unum, PlayModeHearable play_mode, int playmode_num)
{
    //playmode_num es el número incrustado en los modos de juego, por ejemplo, 2 en goal_r_2.
    last_sensor_type = SENSOR_INIT;
    last_init.side = side;
    last_init.uniform_number = unum;
    last_init.play_mode = play_mode;
    last_init.playmode_num = playmode_num;
}

//Funciones referentes a error.
void SensorHandler::error(ErrorType error)
{
    last_sensor_type = SENSOR_ERROR;
    last_error.error = error;
}

//Funciones referentes a ok.

void SensorHandler::ok_start		( )
{
    last_sensor_type = SENSOR_OK;
	last_ok = START;
}
void SensorHandler::ok_check_ball	( int time,
								  CheckBallPosition ball_pos )
{
    last_sensor_type = SENSOR_OK;
	last_ok = CHECK_BALL;
    last_ok_check_ball.time = time;
    last_ok_check_ball.position = ball_pos;
}
void SensorHandler::ok_ear			( bool ear_on )
{
    last_sensor_type = SENSOR_OK;
	last_ok = EAR;
	last_ok_ear.ear_on = ear_on;
}
void SensorHandler::ok_eye			( bool eye_on )
{
    last_sensor_type = SENSOR_OK;
	last_ok = EYE;
	last_ok_eye.eye_on = eye_on;
}
void SensorHandler::ok_move		( )
{
    last_sensor_type = SENSOR_OK;
	last_ok	 = MOVE;
}
void SensorHandler::ok_recover		( )
{
    last_sensor_type = SENSOR_OK;
	last_ok = RECOVER;
}

void SensorHandler::ok_change_mode ( )
{
    last_sensor_type = SENSOR_OK;
	last_ok = CHANGE_MODE;
}

void SensorHandler::ok_look_begin	( int time )
{
    last_sensor_type = SENSOR_OK;
	last_ok = LOOK;
    last_ok_look.time = time;
}

void SensorHandler::ok_look_ball	( double x,
								  double y,
								  double vx,
								  double vy )
{
	last_ok_look.ball.x = x;
	last_ok_look.ball.y = y;
	last_ok_look.ball.vx = vx;
	last_ok_look.ball.vy = vy;
}

void SensorHandler::ok_look_player ( const char * team,
								  int unum,
								  double x,
								  double y,
								  double vx,
								  double vy,
								  int bodyAngle,
								  int neckAngle )
{
	static AbsPlayer new_player;
	new_player.team.assign( team );
	new_player.unum = unum;
	new_player.x = x;
	new_player.y = y;
	new_player.vx = vx;
	new_player.vy = vy;
	new_player.body_angle = bodyAngle;
	new_player.neck_angle = neckAngle;
	
	last_ok_look.players.push_back( new_player );
}

void SensorHandler::ok_synch_see	( )
{
    last_sensor_type = SENSOR_OK;
	last_ok = SYNCH_SEE;
}
	
void SensorHandler::see_global_begin	( int time )
{
    last_sensor_type = SENSOR_SEE_GLOBAL;
    last_see_global.time = time;
    last_see_global.players.clear();
}
void SensorHandler::see_global_ball  	( double x,
							  double y,
							  double vx,
							  double vy )
{
	last_see_global.ball.x = x;
	last_see_global.ball.y = y;
	last_see_global.ball.vx = vx;
	last_see_global.ball.vy = vy;
}
								  
							  
void SensorHandler::see_global_player	( const char * team,
							  int	unum,
							  double x,
							  double y,
						      double vx,
						      double vy,
						      int bodyAngle,
						      int neckAngle )
{
    AbsPlayer new_player;
	new_player.team.assign( team );
	new_player.unum = unum;
	new_player.x = x;
	new_player.y = y;
	new_player.vx = vx;
	new_player.vy = vy;
	new_player.body_angle = bodyAngle;
	new_player.neck_angle = neckAngle;
	
	last_see_global.players.push_back( new_player );
}

