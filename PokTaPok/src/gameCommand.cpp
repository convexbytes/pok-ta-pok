#include "gameCommand.h"
#include "observation.h"

GameCommand::GameCommand()
{
    this->reset();
}


void GameCommand::reset()
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


void GameCommand::append_dash(int power)
{
    turn_set = false; /* exclusion de comandos en el mismo ciclo */
    catch_set = false;
    kick_set = false;
    move_set = false;

    dash_set = true;
    dash_power = power;
}
void GameCommand::append_turn(int angle)
{
    dash_set = false; /* exclusion de comandos en el mismo ciclo */
    catch_set = false;
    move_set = false;
    kick_set = false;

    turn_set = true;
    turn_angle = angle;
}
void GameCommand::append_attentionto( AttentionTeam team, int number )
{
    attention_to_set = true;
    attention_uniform_number = number;
    attention_team = team;
}
void GameCommand::append_turn_neck(int angle)
{
    turn_neck_set = true;
    turn_neck_angle = angle;
}
void GameCommand::append_change_view( ViewModeQuality quality, ViewModeWidth width )
{
    change_view_set = true;
    change_view_quality = quality;
    change_view_width = width;
}
void GameCommand::append_say( const char *message )
{
    say_set = true;
    say_message = message;
}
void GameCommand::append_say( string message )
{
    say_set = true;
    say_message = message;
}

void GameCommand::append_catch( int direction )
{
    dash_set = false;
    turn_set = false;
    kick_set = false;
    move_set = false;

    catch_set = true;
    catch_direction = direction;
}

void GameCommand::append_kick(int power, int direction)
{
    dash_set = false;
    turn_set = false;
    catch_set = false;
    move_set = false;

    kick_set = true;
    kick_direction = direction;
    kick_power = power;
}

void GameCommand:: append_move ( int x, int y )
{
    dash_set = false;
    turn_set = false;
    catch_set = false;
    kick_set = false;

    move_set = true;
    move_x = x;
    move_y = y;

}
