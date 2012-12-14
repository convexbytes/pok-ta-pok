#ifndef GAMECOMMAND_H
#define GAMECOMMAND_H
#include "observation.h"
enum AttentionTeam
{
    ATTENTION_OUR,
    ATTENTION_OPP
};

class GameCommand
{
public:
    GameCommand();
    void reset();
    void append_dash        ( int power );
    void append_turn        ( int angle );
    void append_turn_neck   ( int angle );
    void append_say         ( std::string message );
    void append_say         ( const char *message );
    void append_kick        ( int power, int direction );
    void append_catch       ( int direction );
    void append_change_view ( ViewModeQuality quality, ViewModeWidth width );
    void append_attentionto ( AttentionTeam attention_team, int uniform_number );
    void append_move        ( int x, int y );
    void append_score       ();

    bool dash_is_set ()         { return dash_set; }
    bool turn_is_set ()         { return turn_set; }
    bool turn_neck_is_set ()    { return turn_neck_set; }
    bool say_is_set ()          { return say_set; }
    bool change_view_is_set ()  { return change_view_set; }
    bool attention_to_is_set () { return attention_to_set; }
    bool synch_see_is_set()     { return synch_see_set; }
    bool catch_is_set()         { return catch_set; }
    bool kick_is_set()          { return kick_set; }
    bool move_is_set()          { return move_set; }
    bool score_is_set()         { return score_set; }

    int get_move_x          () const { return move_x; }
    int get_move_y          () const { return move_y; }
    int get_catch_direction () const { return catch_direction; }
    int get_kick_power      () const { return kick_power; }
    int get_dash_power      () const { return dash_power; }
    int move_x, move_y;
    int catch_direction;
    int kick_power, kick_direction;
    int dash_power;
    int turn_angle;
    int turn_neck_angle;
    int attention_uniform_number;
    AttentionTeam attention_team;
    std::string say_message;
    ViewModeQuality change_view_quality;
    ViewModeWidth change_view_width;

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

#endif // GAMECOMMAND_H
