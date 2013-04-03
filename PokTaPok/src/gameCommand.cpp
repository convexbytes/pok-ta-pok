#include<cstdio>

#include "gameCommand.h"

AgentCommand::AgentCommand()
{
    this->reset();
}


void AgentCommand::reset()
{
    dash_power = 0;
    turn_angle = 0;
    turn_neck_angle = 0;
    catch_direction = 0;
    kick_power = 0;
    kick_direction = 0;
    say_message = "";
    change_view_quality = HIGH;
    change_view_width = NORMAL;
    move_x = 0;
    move_y = 0;

    dash_set = false;
    turn_set = false;
    turn_neck_set = false;
    say_set = false;
    change_view_set = false;
    attention_to_set = false;
    synch_see_set = false;
    kick_set = false;
    catch_set = false;
    move_set = false;
}


void AgentCommand::append_dash(double power)
{
    append_dash( power, 0.0 );
}
void AgentCommand::append_dash( double power, double direction )
{
	turn_set  = false; // exclusion de comandos en el mismo ciclo
	catch_set = false;
	kick_set  = false;
	move_set  = false;

	dash_set = true;

	dash_power 		= power;
	dash_direction  = direction;
}
void AgentCommand::append_turn(double angle)
{
    dash_set = false; // exclusion de comandos en el mismo ciclo
    catch_set = false;
    move_set = false;
    kick_set = false;

    turn_set = true;
    turn_angle = angle;
}
void AgentCommand::append_attentionto( AttentionTeam team, int number )
{
    attention_to_set = true;
    attention_uniform_number = number;
    attention_team = team;
}
void AgentCommand::append_turn_neck(double angle)
{
    turn_neck_set = true;
    turn_neck_angle = angle;
}
void AgentCommand::append_change_view( ViewModeQuality quality, ViewModeWidth width )
{
    change_view_set = true;
    change_view_quality = quality;
    change_view_width = width;
}
void AgentCommand::append_say( const char *message )
{
    say_set = true;
    say_message = message;
}
void AgentCommand::append_say( string message )
{
    say_set = true;
    say_message = message;
}

void AgentCommand::append_catch( double direction )
{
    dash_set = false;
    turn_set = false;
    kick_set = false;
    move_set = false;

    catch_set = true;
    catch_direction = direction;
}

void AgentCommand::append_kick(double power, double direction)
{
    dash_set = false;
    turn_set = false;
    catch_set = false;
    move_set = false;

    kick_set = true;
    kick_direction = direction;
    kick_power = power;
}

void AgentCommand:: append_move ( double x, double y )
{
    dash_set = false;
    turn_set = false;
    catch_set = false;
    kick_set = false;

    move_set = true;
    move_x = x;
    move_y = y;

}

void AgentCommand::append_synch_see()
{
    synch_see_set = true;
}



///////////////// TrainerCommand /////////////


TrainerCommand::TrainerCommand()
{
    this->reset();
}

void TrainerCommand::reset()
{
    eye_is_set =
    ear_is_set =
    say_is_set =
    move_is_set =
    start_is_set =
    change_mode_is_set =
    change_ptype_is_set = false;
    says.clear();
    moves.clear();
    ptypes.clear();

}

void TrainerCommand::appendEar(bool ear_on)
{
    this->ear_is_set = true;
    this->ear_on = ear_on;

}

void TrainerCommand::appendEye(bool eye_on)
{
    this->eye_is_set = true;
    this->eye_on = eye_on;
}

void TrainerCommand::appendMoveBall(double x,
                               double y)
{
    static MoveCommand move;
    move.angle_is_set = false;
    move.vel_is_set = false;
    move.object_name.assign( "(ball)" );
    move.x = x;
    move.y = y;
    this->move_is_set = true;
    this->moves.push_back( move );

}

void TrainerCommand::appendMoveBall(double x,
                               double y,
                               double angle)
{
    static MoveCommand move;
    move.angle_is_set = true;
    move.vel_is_set = false;
    move.object_name.assign( "(ball)" );
    move.x = x;
    move.y = y;
    move.vdir = angle;
    this->move_is_set = true;
    this->moves.push_back( move );

}

void TrainerCommand::appendMoveBall(double x,
                               double y,
                               double angle,
                               double vx,
                               double vy)
{
    static MoveCommand move;
    move.angle_is_set = true;
    move.vel_is_set = true;
    move.object_name.assign( "(ball)" );
    move.x = x;
    move.y = y;
    move.vdir = angle;
    move.v_x = vx;
    move.v_y = vy;
    this->move_is_set = true;
    this->moves.push_back( move );
}

void TrainerCommand::appendMovePlayer(std::string name,
                                 int unum,
                                 double x,
                                 double y)
{
    static  MoveCommand move;
    char    obj_name[128];
    move.angle_is_set = false;
    move.vel_is_set = false;

    sprintf(obj_name, "(player %s %d)", name.c_str(), unum );
    move.object_name.assign( obj_name );
    move.x = x;
    move.y = y;
    this->move_is_set = true;
    this->moves.push_back( move );
}

void TrainerCommand::appendMovePlayer(std::string name,
                                 int unum,
                                 double x,
                                 double y,
                                 double angle)
{
    static  MoveCommand move;
    char    obj_name[128];
    move.angle_is_set = true;
    move.vel_is_set = false;

    sprintf(obj_name, "(player %s %d)", name.c_str(), unum );
    move.object_name.assign( obj_name );
    move.x = x;
    move.y = y;
    move.vdir = angle;
    this->move_is_set = true;
    this->moves.push_back( move );
}

void TrainerCommand::appendMovePlayer(std::string name,
                                 int unum,
                                 double x,
                                 double y,
                                 double angle,
                                 double vx,
                                 double vy)
{
    static  MoveCommand move;
    char    obj_name[128];
    move.angle_is_set = true;
    move.vel_is_set = true;

    sprintf(obj_name, "(player %s %d)", name.c_str(), unum );
    move.object_name.assign( obj_name );
    move.x = x;
    move.y = y;
    move.vdir = angle;
    move.v_x = vx;
    move.v_y = vy;
    this->move_is_set = true;
    this->moves.push_back( move );
}

void TrainerCommand::appendChgPlayMode(std::string mode)
{
    this->change_mode_is_set = true;
    play_mode.assign( mode );
}

void TrainerCommand::appendChgPType(std::string team_name, int unum, int p_type)
{
    static PTypeCommand chgtype;

    chgtype.team_name.assign( team_name );
    chgtype.unumber = unum;
    chgtype.p_type = p_type;

    this->change_ptype_is_set = true;
    this->ptypes.push_back( chgtype );
}

void TrainerCommand::appendSay(std::string msg)
{
    // formato del mensaje para la version 15.1.0
    // #define SAY_MESSAGE_SCAN_FORMAT	" ( %s %[-0-9a-zA-Z ().+*/?<>_]"
    this->say_is_set = true;
    this->says.push_back( msg );
}
