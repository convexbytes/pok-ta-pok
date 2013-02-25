#include <cstring>
#include <string>
#include <cstdio>
#include <iostream>

#include "serializer.h"

void Serializer::generate_command( char * output, AgentCommand &game_command )
{
    char command_aux[4096];
    char command_aux2[4096];
    memset(command_aux, 0, 4096);
    memset(command_aux2, 0, 4096);

    // Genera el commando listo para enviarlo al servidor //
    if( game_command.dash_is_set() )
    {
        sprintf(command_aux2, "(dash %lf)", game_command.dash_power );
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
        sprintf(command_aux2, "(move %lf %lf)", game_command.move_x, game_command.move_y );
        strcat(command_aux, command_aux2);
    }
    else if ( game_command.kick_is_set() )
    {
        sprintf(command_aux2, "(kick %lf %lf)", game_command.kick_power, game_command.kick_direction );
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

void Serializer::serializeAgentCommands(const AgentCommand &command, vector<string> *container)
{
    char command_aux2[512];
    memset(command_aux2, 0, 512);
    string new_command;

    /* Genera el commando listo para enviarlo al servidor */
    if( command.dash_is_set() )
    {
        sprintf(command_aux2, "(dash %lf)", command.dash_power );
        new_command.assign( command_aux2 );
        container->push_back( new_command );

    }

    else if ( command.turn_is_set() )
    {
        sprintf(command_aux2, "(turn %d)", command.turn_angle );
        new_command.assign( command_aux2 );
        container->push_back( new_command );
    }
    else if ( command.catch_is_set() )
    {
        sprintf(command_aux2, "(catch %d)", command.catch_direction );
        new_command.assign( command_aux2 );
        container->push_back( new_command );
    }
    else if ( command.move_is_set() )
    {
        sprintf(command_aux2, "(move %lf %lf)", command.move_x, command.move_y );
        new_command.assign( command_aux2 );
        container->push_back( new_command );
    }
    else if ( command.kick_is_set() )
    {
        sprintf(command_aux2, "(kick %lf %lf)", command.kick_power, command.kick_direction );
        new_command.assign( command_aux2 );
        container->push_back( new_command );
    }
    if( command.turn_neck_is_set() )
    {
        sprintf(command_aux2, "(turn_neck %d)", command.turn_neck_angle );
        new_command.assign( command_aux2 );
        container->push_back( new_command );
    }
    if( command.say_is_set() )
    {
        sprintf(command_aux2, "(say %s)", command.say_message.c_str() );
        new_command.assign( command_aux2 );
        container->push_back( new_command );
    }
    if( command.change_view_is_set() )
    {
        switch ( command.change_view_quality )
        {
        case LOW:
            switch( command.change_view_width )
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
            switch( command.change_view_width )
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
        new_command.assign( command_aux2 );
        container->push_back( new_command );
    }
    if( command.attention_to_is_set() )
    {
        switch( command.attention_team )
        {
        case ATTENTION_OUR:
            sprintf( command_aux2, "(attentionto our %d)", command.attention_uniform_number);
            new_command.assign( command_aux2 );
            container->push_back( new_command );
            break;
        case ATTENTION_OPP:
            sprintf( command_aux2, "(attentionto opp %d)", command.attention_uniform_number);
            new_command.assign( command_aux2 );
            container->push_back( new_command );
            break;
        }
    }
    if ( command.synch_see_is_set() )
    {
        sprintf( command_aux2, "(synch_see)");
        new_command.assign( command_aux2 );
        container->push_back( new_command );
    }
}


void Serializer::serializeTrainerCommands( const TrainerCommand & command,
                                          vector<string> * container)
{
    char aux_command[256];
    string new_command;
    unsigned int i;
    if( command.earIsSet() )
    {
        if( command.earOn() )
            sprintf( aux_command, "(ear on)");
        else
            sprintf( aux_command, "(ear on)");
        new_command.assign( aux_command );
        container->push_back( new_command );
    }

    if( command.changeModeIsSet() )
    {
        sprintf( aux_command, "(change_mode %s)", command.playMode().c_str() );
        new_command.assign( aux_command );
        container->push_back( new_command );
    }

    if( command.changePlayerTypeIsSet() )
    {
        for( i=0; i < command.chgPlayerTypes().size() ; i++ )
        {
            sprintf( aux_command, "(change_player_type %s %d %d)",
                     command.chgPlayerTypes().at(i).team().c_str(),
                     command.chgPlayerTypes().at(i).unum(),
                     command.chgPlayerTypes().at(i).pType() );

            new_command.assign( aux_command );
            container->push_back( new_command );
        }
    }

    if( command.eyeIsSet() )
    {
        if( command.eyeOn() )
            sprintf( aux_command, "(eye on)" );
        else
            sprintf( aux_command, "(eye off)" );
        new_command.assign( aux_command );
        container->push_back( new_command );

    }

    if( command.moveIsSet() )
    {
        for( i=0; i < command.move().size() ; i++ )
        {
            if( command.move().at(i).angleIsSet() )
            {
                if( command.move().at(i).velIsSet() )
                {
                    sprintf( aux_command, "(move %s %lf %lf %lf %lf %lf)",
                             command.move().at(i).objectName().c_str(),
                             command.move().at(i).posX(),
                             command.move().at(i).posY(),
                             command.move().at(i).angle(),
                             command.move().at(i).vx(),
                             command.move().at(i).vy()
                             );
                }
                else
                {
                    sprintf( aux_command, "(move %s %lf %lf %lf)",
                             command.move().at(i).objectName().c_str(),
                             command.move().at(i).posX(),
                             command.move().at(i).posY(),
                             command.move().at(i).angle()
                             );
                }
            }
            else
            {
                if( command.move().at(i).velIsSet() )
                {
                    // No es posible que le especifiquemos la velocidad sin el angulo del cuerpo
                    // ignoramos la velocidad y tomamos solo x y y
                    sprintf( aux_command, "(move %s %lf %lf)",
                             command.move().at(i).objectName().c_str(),
                             command.move().at(i).posX(),
                             command.move().at(i).posY()
                             );
                }
                else
                {
                    sprintf( aux_command, "(move %s %lf %lf)",
                             command.move().at(i).objectName().c_str(),
                             command.move().at(i).posX(),
                             command.move().at(i).posY()

                             );
                }
            }
            new_command.assign( aux_command );
            container->push_back( new_command );
        }
    }

    if( command.sayIsSet() )
    {
        for( i=0; i < command.say().size(); i++ )
        {
            sprintf( aux_command, "(say \"%s\")", command.say().at(i).c_str() );
            new_command.assign( aux_command );
            container->push_back( new_command );
        }

    }

    if( command.startIsSet() )
    {
        sprintf( aux_command, "(start)");
        new_command.assign( aux_command );
        container->push_back( new_command );
    }
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
