#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H
#include "gameObject.h"
#include "sensor.h"
#include <vector>

class SensorHandler
{
    friend class Parser;
public:
    SensorHandler();


    SeeSensor           last_see;
    BodySensor          last_sense;
    HearSensor          last_hear;
    HearRefereeSensor   last_hear_referee;
    HearOurSensor       last_hear_our;
    HearOppSensor       last_hear_opp;
    HearCouchSensor     last_hear_couch;
    HearSelfSensor      last_hear_self;
    InitSensor          last_init;
    //MsgSensor           last_msg; Lo pospongo porque el único caso en que se recibe es al terminar el partido.
    ErrorSensor         last_error;
    OkEar			 last_ok_ear;
    OkEye			 last_ok_eye;
    OkLook			 last_ok_look;
    OkCheckBall		 last_ok_check_ball;
    SeeGlobalSensor  last_see_global;
    Request          last_ok;
    SensorType       last_sensor_type; // tipo de sensor del que recibimos la última actualización


private:
    void init		(char side, int unum, PlayModeHearable play_mode, int playmode_num );

    void begin_see	( int time);


    // Comentarios tomados de la librería librcsc-4.1.0
    // " <DIST> <DIR> <DISTCH> <DIRCH> <BODY> <HEAD> <POINTDIR> <TACKLE|KICK>)" : space = 8
    // " <DIST> <DIR> <DISTCH> <DIRCH> <BODY> <HEAD> <POINTDIR>)" : space = 7
    // " <DIST> <DIR> <DISTCH> <DIRCH> <BODY> <HEAD> <TACKLE|KICK>)" : space = 7
    // " <DIST> <DIR> <DISTCH> <DIRCH> <BODY> <HEAD>)" : space = 6
    // " <DIST> <DIR> <DISTCH> <DIRCH> <BODY>)" : space = 5  only sserver-4
    // " <DIST> <DIR> <DISTCH> <DIRCH>)" : space = 4
    // " <DIST> <DIR> <POINTDIR> <TACKLE|KICK>)" : space = 4
    // " <DIST> <DIR> <POINTDIR>)" : space = 3
    // " <DIST> <DIR> <TACKLE|KICK>)" : space = 3
    // " <DIST> <DIR>)" : space = 2
    // " <DIR>)" : space = 1

    void see_player ( string team, int unum, bool is_goalie ,
    				  double dist,
    				  double dir,
    				  double distch,
    				  double dirch,
    				  double body_dir,
    				  double head_dir,
    				  double point_dir,
    				  bool	 on_tackle,
    				  bool   on_kick);

	
    void see_flag( EFlag id , float distance, int direction , float dir_chg , float dis_chg );
    void see_flag( EFlag id , float distance, int direction );
    void see_flag( float distance, int direction );
    void see_line( ELine id, float distance , int direction , float dir_chg , float dis_chg );
    void see_line( ELine id, float distance , int direction );
    void see_ball(float distance, int direction, float dis_change, float dir_change);
    void see_ball(float distance, int direction);
    
    void begin_sense(int time);
    void sense_view_mode_quality( ViewModeQuality );
    void sense_view_mode_width( ViewModeWidth );
    void sense_stamina( float );
    void sense_effort( float );
    void sense_staminaCapacity( int );
    void sense_speed_amount( float );
    void sense_speed_direction( int );
    void sense_head_angle( int );
    void sense_kick( int );
    void sense_dash( int );
    void sense_turn( int );
    void sense_say( int );
    void sense_turn_neck( int );
    void sense_catchh( int );
    void sense_move( int );
    void sense_change_view( int );
    void sense_arm_movable( int );
    void sense_arm_expires( int );
    void sense_arm_target_1( int );
    void sense_arm_target_2( int );
    void sense_arm_count( int );
    void sense_focus_target( char side, int number );
    void sense_focus_count( int );
    void sense_tackle_expires( int );
    void sense_tackle_count( int );
    void sense_collision( Collision );
    void sense_foul_charged( int );
    void sense_foul_card( FoulCard );

    void hear_referee   ( int time, PlayModeHearable play_mode, int num );
    void hear_referee   ( int time, PlayModeHearable play_mode );
    void hear_self      ( int time, string msg );
    void hear_couch     ( int time, string msg );
    void hear_our       ( int time, int direction, int unum, string msg );
    void hear_opp       ( int time, int direction, int unum, string msg );

    void error          (  ErrorType error );

    //void msg( int time, string msg );

    void ok_start 		( );
    void ok_check_ball 	( int time, 
						  CheckBallPosition bal_pos ); // Only for check_ball
    void ok_ear			( bool ear_on );
	void ok_eye			( bool eye_on );
    void ok_move		( );
	void ok_recover		( );
	void ok_change_mode ( ); 
	void ok_look_begin  ( int time ); 
	void ok_look_ball   ( double x,
						  double y,
						  double vx,
						  double vy );
	void ok_look_player ( const char * team,
						  int	unum,
						  double x,
						  double y,
						  double vx,
						  double vy,
						  int bodyAngle,
						  int neckAngle );
	void ok_synch_see	( );
	
	void see_global_begin	( int time );
	void see_global_ball  	( double x,
							  double y,
							  double vx,
							  double vy );
	void see_global_player	( const char * team,
							  int	unum,
							  double x,
							  double y,
						      double vx,
						      double vy,
						      int bodyAngle,
						      int neckAngle );
	
};

#endif // SENSOR_HANDLER_H
