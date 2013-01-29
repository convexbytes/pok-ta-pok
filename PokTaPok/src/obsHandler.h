#ifndef OBS_HANDLER_H
#define OBS_HANDLER_H
#include "gameObject.h"
#include "observation.h"
#include <vector>

class ObsHandler
{

public:
    ObsHandler();
    SeeObs           last_see;
    SenseObs         last_sense;
    HearObs          last_hear;
    HearRefereeObs   last_hear_referee;
    HearOurObs       last_hear_our;
    HearOppObs       last_hear_opp;
    HearCouchObs     last_hear_couch;
    HearSelfObs      last_hear_self;
    InitObs          last_init;
    //MsgObs           last_msg; Lo pospongo porque el único caso en que se recibe es al terminar el partido.
    ErrorObs         last_error;
    OkEar			 last_ok_ear;
    OkEye			 last_ok_eye;
    OkLook			 last_ok_look;
    OkCheckBall		 last_ok_check_ball;
    SeeGlobal		 last_see_global;
    Request          last_ok;
    ObservationType  last_obs_type;
    int				 last_obs_time;

    void init		(char side, int unum, PlayMode play_mode, int playmode_num );

    void begin_see	( int time);
    void see_player	( string team, int unum ,  float distance , int direction , float dis_change ,
                       float dir_change , float speed_x , float speed_y ,
                       int body_dir , int neck_dir );
    void see_player	( string team , int unum ,  float distance , int direction , float dis_change ,
                       float dir_change , float speed_x , float speed_y );
    void see_player	( string team , int unum ,  float distance , int direction , float dis_change ,
                       float dir_change );
    void see_player	( string team , int unum ,  float distance , int direction );
    void see_player	( string team ,  float distance , int direction );
    void see_player	( float distance , int direction );

	
    void see_flag( EFlag id , float distance, int direction , float dir_chg , float dis_chg );
    void see_flag( EFlag id , float distance, int direction );
    void see_flag( float distance, int direction );
    void see_line( ELine id, float distance , int direction , float dir_chg , float dis_chg );
    void see_line( ELine id, float distance , int direction );
    void see_ball(float distance, int direction, float dis_change, float dir_change, float speed_x, float speed_y);
    void see_ball(float distance, int direction, float dis_change, float dir_change);
    void see_ball(float distance, int direction);
    void see_finish(); // Le decimos que ya terminó para que haga algunas tareas como ordenar, pendiente a remover esta función.
    
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

    void hear_referee   ( int time, PlayMode play_mode, int num );
    void hear_referee   ( int time, PlayMode play_mode );
    void hear_self      ( int time, string msg );
    void hear_couch     ( int time, string msg );
    void hear_our       ( int time, int direction, int unum, string msg );
    void hear_opp       ( int time, int direction, int unum, string msg );

    void error          ( int time, ErrorType error );

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

#endif /* OBS_HANDLER_H */
