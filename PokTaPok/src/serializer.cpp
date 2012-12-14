#include "serializer.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
Serializer::Serializer()
{

}

//std::string Serializer::generate_command( std::string & output )
void Serializer::generate_command( char * output, GameCommand & game_command )
{
    char command_aux[4096];
    char command_aux2[4096];
    memset(command_aux, 0, 4096);
    memset(command_aux2, 0, 4096);

    /* Genera el commando listo para enviarlo al servidor */
    if( game_command.dash_is_set() )
    {
        sprintf(command_aux2, "(dash %d)", game_command.dash_power );
        strcat(command_aux, command_aux2);
    }

    else if ( game_command.turn_is_set() )
    {
        sprintf(command_aux2, "(turn %d)", game_command.turn_angle );
        strcat(command_aux, command_aux2);
    }
    else if ( game_command.catch_is_set() )
    {
        sprintf(command_aux2, "(catch %d)", game_command.catch_direction );
        strcat(command_aux, command_aux2);
    }
    else if ( game_command.move_is_set() )
    {
        sprintf(command_aux2, "(move %d %d)", game_command.move_x, game_command.move_y );
        strcat(command_aux, command_aux2);
    }
    else if ( game_command.kick_is_set() )
    {
        sprintf(command_aux2, "(kick %d %d)", game_command.kick_power, game_command.kick_direction );
        strcat(command_aux, command_aux2);
    }
    if( game_command.turn_neck_is_set() )
    {
        sprintf(command_aux2, "(turn_neck %d)", game_command.turn_neck_angle );
        strcat(command_aux, command_aux2);
    }
    if( game_command.say_is_set() )
    {
        sprintf(command_aux2, "(say %s)", game_command.say_message.c_str() );
        strcat(command_aux, command_aux2);
    }
    if( game_command.change_view_is_set() )
    {
        switch ( game_command.change_view_quality )
        {
        case LOW:
            switch( game_command.change_view_width )
            {
            case NARROW:    sprintf(command_aux2, "(change_view narrow low)");
                break;
            case NORMAL:    sprintf(command_aux2, "(change_view normal low)");
                break;
            case WIDE:      sprintf(command_aux2, "(change_view wide low)");
                break;
            }
            break;
        case HIGH:
            switch( game_command.change_view_width )
            {
            case NARROW:    sprintf(command_aux2, "(change_view narrow high)");
                break;
            case NORMAL:    sprintf(command_aux2, "(change_view normal high)");
                break;
            case WIDE:       sprintf(command_aux2, "(change_view wide high)");
                break;
            }
            break;
        }
        strcat(command_aux, command_aux2);
    }
    if( game_command.attention_to_is_set() )
    {
        switch( game_command.attention_team )
        {
        case ATTENTION_OUR:
            sprintf( command_aux2, "(attentionto our %d)", game_command.attention_uniform_number);
            strcat(command_aux, command_aux2);
            break;
        case ATTENTION_OPP:
            sprintf( command_aux2, "(attentionto opp %d)", game_command.attention_uniform_number);
            strcat(command_aux, command_aux2);
            break;
        }
    }
    if ( game_command.synch_see_is_set() )
    {
        sprintf( command_aux2, "(synch_see)");
        strcat(command_aux, command_aux2);
    }
    strcpy( output, command_aux );
}

/*
int main()
{
	std::string result;
	Serializer serializer;
	char buffer[4096];
	serializer.append_attention_to_teammate(11);
	serializer.append_catch( 10 );
	serializer.append_catch( 10 );
	result = serializer.generate_command();
	strcpy( buffer, result.c_str() );
	printf( "%s", buffer );
	
	return 0;
	}
*/
