#ifndef GAMECOMMAND_H
#define GAMECOMMAND_H
#include "sensor.h"
enum AttentionTeam
{
    ATTENTION_OUR,
    ATTENTION_OPP
};

class AgentCommand
{
public:
    AgentCommand();
    void reset();
    void append_dash        ( double power );
    void append_dash		( double power, double direction );
    void append_turn        ( double angle );
    void append_turn_neck   ( double angle );
    void append_say         ( std::string message );
    void append_say         ( const char *message );
    void append_kick        ( double power, double direction );
    void append_catch       ( double direction );
    void append_change_view ( ViewModeQuality quality, ViewModeWidth width );
    void append_attentionto ( AttentionTeam attention_team, int uniform_number );
    void append_move        ( double x, double y );
    void append_score       ();
    void append_synch_see   ();

    bool dash_is_set ()         const { return dash_set; }
    bool turn_is_set ()         const { return turn_set; }
    bool turn_neck_is_set ()    const { return turn_neck_set; }
    bool say_is_set ()          const { return say_set; }
    bool change_view_is_set ()  const { return change_view_set; }
    bool attention_to_is_set () const { return attention_to_set; }
    bool synch_see_is_set()     const { return synch_see_set; }
    bool catch_is_set()         const { return catch_set; }
    bool kick_is_set()          const { return kick_set; }
    bool move_is_set()          const { return move_set; }
    bool score_is_set()         const { return score_set; }

    double get_move_x          () const { return move_x; }
    double get_move_y          () const { return move_y; }
    double get_catch_direction () const { return catch_direction; }
    double get_kick_power      () const { return kick_power; }
    double get_dash_power      () const { return dash_power; }

    double move_x;
    double move_y;

    double catch_direction;

    double kick_power;
    double kick_direction;

    double dash_power;
    double dash_direction;

    double turn_angle;
    double turn_neck_angle;

    int 			attention_uniform_number;
    AttentionTeam   attention_team;

    std::string 	say_message;

    ViewModeQuality change_view_quality;
    ViewModeWidth   change_view_width;

private:
    //Las siguientes variables indican si realizaron petición a algún comando.
    bool dash_set;
    bool turn_set;
    bool turn_neck_set;
    bool say_set;
    bool change_view_set;
    bool attention_to_set;
    bool synch_see_set;
    bool catch_set;
    bool kick_set;
    bool move_set;
    bool score_set;

};


class MoveCommand
{
public:
    friend class TrainerCommand;
    //MoveCommand( );

    bool                angleIsSet() const { return angle_is_set; }
    bool                velIsSet()   const{ return vel_is_set;}

    std::string const & objectName() const{ return object_name;}
    double              posX()      const{ return x;}
    double              posY()      const{ return y;}
    double              vx()        const{ return v_x;}
    double              vy()        const { return v_y;}
    double              angle()     const{ return vdir; }

private:
    bool angle_is_set;
    bool vel_is_set;

    double x;
    double y;
    double v_x;
    double v_y;
    double vdir;
    std::string object_name;
};

class PTypeCommand
{
public:
    friend class TrainerCommand;

    string const &  team()  const { return team_name; }
    int             unum()  const { return unumber; } // uniform number
    int             pType() const { return p_type; } // player type

private:
    string  team_name;
    int     unumber;
    int     p_type;
};


class TrainerCommand
{
public:
    TrainerCommand();
    void reset();

    bool eyeIsSet() 	const { return eye_is_set; }
    bool earIsSet() 	const { return ear_is_set; }
    bool sayIsSet() 	const { return say_is_set; }
    bool moveIsSet() 	const { return move_is_set; }
    bool startIsSet() 	const { return start_is_set; }
    bool changeModeIsSet() 		const { return change_mode_is_set; }
    bool changePlayerTypeIsSet() const { return change_ptype_is_set; }


    bool                    earOn()    const { return ear_on; }
    bool                    eyeOn()    const{ return eye_on; }
    vector<string> const &  say()       const{ return says; }

    vector<MoveCommand> const & move() const{ return moves; }
    string const              & playMode() const{ return play_mode; }
    vector<PTypeCommand> const & chgPlayerTypes() const{ return ptypes; }


    void appendEar( bool ear_on );

    void appendEye( bool eye_on );

    void appendSay( std::string msg );

    void appendMoveBall( double x,
                         double y );

    void appendMoveBall( double x,
                         double y,
                         double angle );

    void appendMoveBall( double x,
                         double y,
                         double angle,
                         double vx,
                         double vy );

    void appendMovePlayer( std::string name,
                           int unum,
                           double x,
                           double y );

    void appendMovePlayer( std::string name,
                           int unum,
                           double x,
                           double y,
                           double angle );

    void appendMovePlayer( std::string name,
                           int unum,
                           double x,
                           double y,
                           double angle,
                           double vx,
                           double vy );

    void appendChgPlayMode( std::string mode );
    void appendChgPType   ( std::string team_name, int unum, int p_type );


private:
    bool eye_is_set;
    bool ear_is_set;
    bool say_is_set;
    bool move_is_set;
    bool start_is_set;
    bool change_mode_is_set; // playmode
    bool change_ptype_is_set; // player type


    bool ear_on;
    bool eye_on;
    vector<string> says;
    vector<MoveCommand> moves;
    string play_mode;
    vector<PTypeCommand> ptypes;

};

#endif // GAMECOMMAND_H
