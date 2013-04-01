#include "parser.h"
#include "microParser.h"
#include "gameData.h"
#include "sensorHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>


Parser::Parser(GameData *game_data)
{
    this->game_data = game_data;

}

ParserMsgType Parser::parse(char *message )
{
    if( strncmp( message, "(server_param", 13) == 0 )
    {
        parse_server_param( message );
        return P_SERVER_PARAM;
    }

    else if( strncmp(message, "(player_param", 13) == 0 )
    {
        parse_player_param( message );
        return P_PLAYER_PARAM;
    }
    else if( strncmp(message, "(player_type", 12) == 0 )
    {
        parse_player_type( message );
        return P_PLAYER_TYPE;
    }
    else if( strncmp( message, "(see_global", 11) == 0 )
    {
        parse_see_global( message );
        return P_SEE_GLOBAL;
    }
    else if( strncmp(message, "(see", 4) == 0 )
    {
        parse_see( message );
        return P_SEE;
    }
    else if( strncmp(message, "(sense", 6) == 0 )
    {
        parse_sense( message );
        return P_SENSE;
    }
    else if( strncmp(message, "(hear", 5) == 0 )
    {
        parse_hear( message );
        return P_HEAR;
    }
    else if( strncmp(message, "(msg", 4) == 0 )
    {
        //parse_msg( message );
        return P_MSG;
    }
    else if( strncmp(message, "(init", 5) == 0 )
    {
        parse_init( message );
        return P_INIT;
    }
    else if( strncmp(message, "(ok", 3) == 0 )
    {
        parse_ok( message );
        return P_OK;
    }
    else if( strncmp(message, "(error", 6) == 0 )
    {
        parse_error( message );
        return P_ERROR;
    }
    else
    {
        return P_PARSING_ERROR;
    }
}

void Parser::parse_error( char *message)
{
        char 	str1[256];
        std::cout << message << std::endl;
        if ( strncmp( ("(error"), message, 6 ) == 0 )
        {

        	message = move_to_next_word( message );

            sscanf( message+1,"%[^)]", str1 ); // Lee todos los caracteres menos ')'

            if( strcmp( str1, "illegal_mode" )==0 )
            {
                game_data->sensor_handler.error( ILLEGAL_MODE );
            }

            else if ( strcmp(str1, "illegal_object_form" )==0 )
            {
                game_data->sensor_handler.error( ILLEGAL_OBJECT_FORM );
            }

            else if ( strcmp(str1, "illegal_command_form" )==0 )
            {
                game_data->sensor_handler.error( ILLEGAL_COMMAND_FORM );
            }

            // Aún faltan más mensajes de error
        }
}

void Parser::parse_hear( char *message )
{
    char  * subcad = NULL;
    int 	time;
    int 	aux_num;
    //int 	angle;
    char 	aux_str[256];
    PlayModeHearable play_mode; // "playmode" que podemos escuchar del referee
    
    subcad = strstr( message,"(hear" ); //Buscamos el inicio del hear
    if( subcad == NULL ) return; //Si no encuentra algún hear, salimos
    subcad = move_to_next_word( subcad );
    sscanf(subcad, "%d", &time );

    subcad = move_to_next_word( subcad );
    if( isdigit( *subcad ) || *subcad == '-' )
    {
        // Aquí hay un problema de parseo, segmentation fault
        //Si nos metemos aquí, quiere decir que recibimos un ángulo y el sender es: our | opp
        /*
        sscanf( subcad, "%d", &angle );
        subcad = move_to_next_word( subcad );
        strncpy(aux_str, subcad, 3);
        if( strcmp( aux_str, "our" ) )
        {
            subcad = move_to_next_word( subcad );
            sscanf( subcad, "%d", &aux_num ); //obtenemos el unum
            subcad = move_to_next_word( subcad );
            sscanf( subcad, "%s", aux_str );	//extraemos el mensaje antes del paréntesis que cierra, incluyendo las comillas
            aux_str[ strlen( aux_str) -1 ] = '\0'; //quitamos el paréntesis
            game_data->sensor_handler.hear_our(time, angle, aux_num, aux_str);

        }
        else if( strcmp( aux_str, "opp" ) )
        {*/
            /*
            subcad = move_to_next_word( subcad );
            sscanf( subcad, "%d", &aux_num ); //obtenemos el unum
            subcad = move_to_next_word( subcad );
            sscanf( subcad, "%s", aux_str );	//extraemos el mensaje antes del paréntesis que cierra, incluyendo las comillas
            aux_str[ strlen( aux_str) -1 ] = '\0'; //quitamos el paréntesis
            game_data->sensor_handler.hear_opp(time, angle, aux_num, aux_str);
            */
        //}
        //else
//            return;
    }
    else
    {
        //Si nos metemos aquí, el sender puede ser: self | couch | referee
        switch( *subcad )
        {
            case 's': //El sender es self, recibimos solamente una cadena
                subcad = move_to_next_word( subcad );
                sscanf( subcad, "%s", aux_str );	//extraemos el mensaje antes del paréntesis que cierra, incluyendo las comillas
                aux_str[ strlen( aux_str) -1 ] = '\0'; //quitamos el paréntesis
                game_data->sensor_handler.hear_self(time, aux_str );


            break;
            case 'c': //El sender es couch, recibimos solamente una cadena
                subcad = move_to_next_word( subcad );
                sscanf( subcad, "%s", aux_str );	//extraemos el mensaje antes del paréntesis que cierra, incluyendo las comillas
                aux_str[ strlen( aux_str) -1 ] = '\0';//quitamos el paréntesis
                game_data->sensor_handler.hear_couch(time, aux_str );

            break;
            case 'r': //El sender es referee, recibimos el play_mode
                subcad = move_to_next_word( subcad ); //#gil_mark
                sscanf( subcad, "%s", aux_str );	//extraemos el mensaje antes del paréntesis que cierra
                aux_str[ strlen( aux_str) -1 ] = '\0';//quitamos el paréntesis
                //obtenemos el playmode
                play_mode = this->parse_play_mode(aux_str, aux_num );
                game_data->sensor_handler.hear_referee( time, play_mode );

            break;
            default:
                return;
            break;
        }
    }

}

void Parser::parse_init( char *message )
{
    char * subcad;
    int unum;
    char side;
    char str[256];
    int aux_num;
    PlayModeHearable play_mode;
    subcad = strstr(message, "(init" );
    if( subcad == NULL ) return;


    if( strncmp( "(init ok)", message, 9 ) == 0 )
    {
        game_data->sensor_handler.init( 'c', 0, (PlayModeHearable)0, 0 );
    }
    else
    {
        sscanf(subcad, "(init %c %d %s", &side, &unum, str );
        str[ strlen(str) -1 ] = '\0'; //quitamos el paréntesis que cierra
        play_mode = parse_play_mode( str, aux_num );
        if( play_mode == 0 ) //Cuando play_mode es igual a cero, hubo un error en el parseo
            return;
        game_data->sensor_handler.init(side, unum, play_mode, aux_num );
    }


}

void Parser::parse_ok( char *message )
{
        char str1[256];
        int time;
        char *subcad = message;
        subcad = move_to_next_word( subcad );
        sscanf( subcad,"%[^)]", str1 );
        if  	(strcmp(str1, "change_mode")==0)
        {
            game_data->sensor_handler.ok_change_mode(  );
        }
        else if (strcmp(str1, "move")==0)
        {
            game_data->sensor_handler.ok_move(  );
        }
        else if (strcmp(str1, "start")==0)
        {
            game_data->sensor_handler.ok_start(  );
        }
        else if (strcmp(str1, "recover")==0)
        {
            game_data->sensor_handler.ok_recover(  );
        }
        else if (strcmp(str1, "ear")==0)
        {
        	subcad = move_to_next_word( subcad );
            sscanf( subcad, "%s", str1 );
            if( str1[ strlen( str1 ) -1 ] == ')' ) str1[ strlen( str1 ) -1 ] = '\0'; // Quitamos el último paréntesis
            if( strcmp(str1, "on") == 0 )
            {
                game_data->sensor_handler.ok_ear( true );
            }
            else if (strcmp(str1, "off") == 0)
            {
                game_data->sensor_handler.ok_ear( false );
            }
        }
		else if ( strcmp( str1, "eye" ) == 0 )
        {
			subcad = move_to_next_word( subcad );
			sscanf( subcad, "%s", str1 );
            if( str1[ strlen( str1 ) -1 ] == ')' ) str1[ strlen( str1 ) -1 ] = '\0'; // Quitamos el último paréntesis
            if( strcmp(str1, "on") == 0 )
            {
                game_data->sensor_handler.ok_eye( true );
            }
            else if (strcmp(str1, "off") == 0)
            {
                game_data->sensor_handler.ok_eye( false );
            }
		}
        
        else if (strcmp(str1, "check_ball")==0)
        {
        	subcad = move_to_next_word( subcad );
            sscanf(subcad, "%d", &time ); // Obtenemos tiempo
            subcad = move_to_next_word( subcad );
            sscanf( subcad, "%s", str1 ); // Obtenemos in_field, goal_l, etc...
            if( str1[ strlen( str1 ) -1 ] == ')' ) str1[ strlen( str1 ) -1 ] = '\0';
            if ( strcmp( str1, "in_field" ) == 0 )
            {
                game_data->sensor_handler.ok_check_ball( time, B_IN_FIELD );
            }
            else if ( strcmp( str1, "goal_l" ) == 0 )
            {
                game_data->sensor_handler.ok_check_ball( time, B_GOAL_L );
            }
            else if ( strcmp( str1, "goal_r" ) == 0 )
            {
                game_data->sensor_handler.ok_check_ball( time, B_GOAL_R );
            }
            else if ( strcmp( str1, "out_of_field") == 0 )
            {
                game_data->sensor_handler.ok_check_ball( time, B_OUT_OF_FIELD );
            }
        }
        else if ( strcmp( str1, "look" ) == 0 )
        {
			double 	x, y, vx, vy, body_angle, neck_angle;
			int		unum, n_readed;
			char	team[56];
			
			message += 9; // Recorremos 9 espacios: (ok look 
			// Datos del tiempo y la portería derecha
			sscanf( message, "%d ( ( g r ) %lf %lf ) %n", &time, &x, &y, &n_readed );
			message += n_readed;
			
            game_data->sensor_handler.ok_look_begin( time );
			
			// Datos de la portería izquierda
			sscanf( message, "( ( g l ) %lf %lf ) %n", &x, &y, &n_readed );
			message += n_readed;
			
			// Datos del balón
			sscanf( message, "( (b) %lf %lf %lf %lf ) %n", &x, &y, &vx, &vy, &n_readed );
			message += n_readed;
			
            game_data->sensor_handler.ok_look_ball( x, y, vx, vy );
			
			// Datos de todos los jugadores
			while( *message != ')' )
			{
				sscanf( message, "( (p \"%[-0-9a-zA-Z ().+*/?<>_]\" %d ) %lf %lf %lf %lf %lf %lf ) %n",
						team, &unum, &x, &y, &vx, &vy, &body_angle, &neck_angle, &n_readed );
				message += n_readed;
				
                game_data->sensor_handler.ok_look_player( team, unum, x, y, vx, vy, body_angle, neck_angle );
			}
		}
		else if ( strcmp( str1, "synch_see" ) == 0 )
		{
            game_data->sensor_handler.ok_synch_see();
		}
		else
			std::cout << "'Parser::parse_ok(): 'ok' unkown form" << std::endl;
}

void Parser::parse_player_param(char *message)
{
        double init_value2;
        char * subcadena = NULL;
        subcadena = strstr(message,"allow_mult_default_type");
        if (subcadena){
            sscanf(subcadena,"allow_mult_default_type %lf",&init_value2);

            game_data->game_parameter.player_param.allow_mult_default_type=init_value2;
        }
        subcadena = strstr(message, "catchable_area_l_stretch_max ");
        if (subcadena){
            sscanf(subcadena, "catchable_area_l_stretch_max %lf",&init_value2);
            game_data->game_parameter.player_param.catchable_area_l_stretch_max=init_value2;
        }
        subcadena = strstr(message, "catchable_area_l_stretch_min ");
        if (subcadena) {
            sscanf(subcadena, "catchable_area_l_stretch_min %lf",&init_value2);
            game_data->game_parameter.player_param.catchable_area_l_stretch_min=init_value2;
        }
        subcadena = strstr(message, "dash_power_rate_delta_max ");
        if (subcadena) {
            sscanf(subcadena, "dash_power_rate_delta_max %lf",&init_value2);
            game_data->game_parameter.player_param.dash_power_rate_delta_max=init_value2;
        }
        subcadena = strstr(message, "dash_power_rate_delta_min ");
        if (subcadena) {
            sscanf(subcadena, "dash_power_rate_delta_min %lf",&init_value2);
            game_data->game_parameter.player_param.dash_power_rate_delta_min=init_value2;
        }
        subcadena = strstr(message, "effort_max_delta_factor ");
        if (subcadena) {
            sscanf(subcadena, "effort_max_delta_factor %lf",&init_value2);
            game_data->game_parameter.player_param.effort_max_delta_factor=init_value2;
        }
        subcadena = strstr(message, "effort_min_delta_factor ");
        if (subcadena) {
            sscanf(subcadena, "effort_min_delta_factor %lf",&init_value2);
            game_data->game_parameter.player_param.effort_min_delta_factor=init_value2;
        }
        subcadena = strstr(message, "effort_min_delta_factor ");
        if (subcadena) {
            sscanf(subcadena, "effort_min_delta_factor %lf",&init_value2);
            game_data->game_parameter.player_param.effort_min_delta_factor=init_value2;
        }
        subcadena = strstr(message, "extra_stamina_delta_max ");
        if (subcadena) {
            sscanf(subcadena, "extra_stamina_delta_max %lf",&init_value2);
            game_data->game_parameter.player_param.extra_stamina_delta_max=init_value2;
        }
        subcadena = strstr(message, "extra_stamina_delta_min ");
        if (subcadena) {
            sscanf(subcadena, "extra_stamina_delta_min %lf",&init_value2);
            game_data->game_parameter.player_param.extra_stamina_delta_min=init_value2;
        }
        subcadena = strstr(message, "foul_detect_probability_delta_factor ");
        if (subcadena) {
            sscanf(subcadena, "foul_detect_probability_delta_factor %lf",&init_value2);
            game_data->game_parameter.player_param.foul_detect_probability_delta_factor=init_value2;
        }
        subcadena = strstr(message, "inertia_moment_delta_factor ");
        if (subcadena) {
            sscanf(subcadena, "inertia_moment_delta_factor %lf",&init_value2);
            game_data->game_parameter.player_param.inertia_moment_delta_factor=init_value2;
        }
        subcadena = strstr(message, "kick_power_rate_delta_max ");
        if (subcadena) {
            sscanf(subcadena, "kick_power_rate_delta_max %lf",&init_value2);
            game_data->game_parameter.player_param.kick_power_rate_delta_max=init_value2;
        }
        subcadena = strstr(message, "kick_power_rate_delta_min ");
        if (subcadena) {
            sscanf(subcadena, "kick_power_rate_delta_min %lf",&init_value2);
            game_data->game_parameter.player_param.kick_power_rate_delta_min=init_value2;
        }
        subcadena = strstr(message, "kick_rand_delta_factor ");
        if (subcadena) {
            sscanf(subcadena, "kick_rand_delta_factor %lf",&init_value2);
            game_data->game_parameter.player_param.kick_rand_delta_factor=init_value2;
        }
        subcadena = strstr(message, "kickable_margin_delta_max ");
        if (subcadena) {
            sscanf(subcadena, "kickable_margin_delta_max %lf",&init_value2);
            game_data->game_parameter.player_param.kickable_margin_delta_max=init_value2;
        }
        subcadena = strstr(message, "kickable_margin_delta_min ");
        if (subcadena) {
            sscanf(subcadena, "kickable_margin_delta_min %lf",&init_value2);
            game_data->game_parameter.player_param.kickable_margin_delta_min=init_value2;
        }
        subcadena = strstr(message, "new_dash_power_rate_delta_max ");
        if (subcadena) {
            sscanf(subcadena, "new_dash_power_rate_delta_max %lf",&init_value2);
            game_data->game_parameter.player_param.new_dash_power_rate_delta_max=init_value2;
        }
        subcadena = strstr(message, "new_dash_power_rate_delta_min ");
        if (subcadena) {
            sscanf(subcadena, "new_dash_power_rate_delta_min %lf",&init_value2);
            game_data->game_parameter.player_param.new_dash_power_rate_delta_min=init_value2;
        }
        subcadena = strstr(message, "new_stamina_inc_max_delta_factor ");
        if (subcadena) {
            sscanf(subcadena, "new_stamina_inc_max_delta_factor %lf",&init_value2);
            game_data->game_parameter.player_param.new_stamina_inc_max_delta_factor=init_value2;
        }
        subcadena = strstr(message, "player_decay_delta_max ");
        if (subcadena) {
            sscanf(subcadena, "player_decay_delta_max %lf",&init_value2);
            game_data->game_parameter.player_param.player_decay_delta_max=init_value2;
        }
        subcadena = strstr(message, "player_decay_delta_min ");
        if (subcadena) {
            sscanf(subcadena, "player_decay_delta_min %lf",&init_value2);
            game_data->game_parameter.player_param.player_decay_delta_min=init_value2;
        }
        subcadena = strstr(message, "player_size_delta_factor ");
        if (subcadena) {
            sscanf(subcadena, "player_size_delta_factor %lf",&init_value2);
            game_data->game_parameter.player_param.player_size_delta_factor=init_value2;
        }
        subcadena = strstr(message, "player_speed_max_delta_max ");
        if (subcadena) {
            sscanf(subcadena, "player_speed_max_delta_max %lf",&init_value2);
            game_data->game_parameter.player_param.player_speed_max_delta_max=init_value2;
        }
        subcadena = strstr(message, "player_speed_max_delta_min ");
        if (subcadena) {
            sscanf(subcadena, "player_speed_max_delta_min %lf",&init_value2);
            game_data->game_parameter.player_param.player_speed_max_delta_min=init_value2;
        }
        subcadena = strstr(message, "player_types ");
        if (subcadena) {
            sscanf(subcadena, "player_types %lf",&init_value2);
            game_data->game_parameter.player_param.player_types=init_value2;
        }
        subcadena = strstr(message, "pt_max ");
        if (subcadena) {
            sscanf(subcadena, "pt_max %lf",&init_value2);
            game_data->game_parameter.player_param.pt_max=init_value2;
        }
        subcadena = strstr(message, "random_seed ");
        if (subcadena) {
            sscanf(subcadena, "random_seed %lf",&init_value2);
            game_data->game_parameter.player_param.random_seed=init_value2;
        }
        subcadena = strstr(message, "stamina_inc_max_delta_factor ");
        if (subcadena) {
            sscanf(subcadena, "stamina_inc_max_delta_factor %lf",&init_value2);
            game_data->game_parameter.player_param.stamina_inc_max_delta_factor=init_value2;
        }
        subcadena = strstr(message, "subs_max ");
        if (subcadena) {
            sscanf(subcadena, "subs_max %lf",&init_value2);
            game_data->game_parameter.player_param.subs_max=init_value2;
        }
}

void Parser::parse_player_type(char *message)
{
    double init_value3;
    int int_value;
    PlayerType new_type;
    char * subcadena;
    subcadena = NULL;
    subcadena=strstr(message, "id ");
        if (subcadena) {
            sscanf(subcadena, "id %d",&int_value);
            new_type.id=int_value;
        }
        subcadena=strstr(message, "player_speed_max ");
        if (subcadena) {
            sscanf(subcadena, "player_speed_max %lf",&init_value3);
            new_type.player_speed_max=init_value3;
        }
        subcadena=strstr(message, "stamina_inc_max ");
        if (subcadena) {
            sscanf(subcadena, "stamina_inc_max %lf",&init_value3);
            new_type.stamina_inc_max=init_value3;
        }
        subcadena=strstr(message, "player_decay ");
        if (subcadena) {
            sscanf(subcadena, "player_decay %lf",&init_value3);
            new_type.player_decay = init_value3;
        }
        subcadena=strstr(message, "inertia_moment ");
        if (subcadena) {
            sscanf(subcadena, "inertia_moment %lf",&init_value3);
            new_type.inertia_moment = init_value3;
        }
        subcadena=strstr(message, "dash_power_rate ");
        if (subcadena) {
            sscanf(subcadena, "dash_power_rate %lf",&init_value3);
            new_type.dash_power_rate = init_value3;
        }
        subcadena=strstr(message, "player_size ");
        if (subcadena) {
            sscanf(subcadena, "player_size %lf",&init_value3);
            new_type.player_size=init_value3;
        }
        subcadena=strstr(message, "kickable_margin ");
        if (subcadena) {
            sscanf(subcadena, "kickable_margin %lf",&init_value3);
            new_type.kickable_margin=init_value3;
        }
        subcadena=strstr(message, "kick_rand ");
        if (subcadena) {
            sscanf(subcadena, "kick_rand %lf",&init_value3);
            new_type.kick_rand=init_value3;
        }
        subcadena=strstr(message, "extra_stamina ");
        if (subcadena) {
            sscanf(subcadena, "extra_stamina %lf",&init_value3);
            new_type.extra_stamina=init_value3;
        }
        subcadena=strstr(message, "effort_max ");
        if (subcadena) {
            sscanf(subcadena, "effort_max %lf",&init_value3);
            new_type.effort_max=init_value3;
        }
        subcadena=strstr(message, "effort_min ");
        if (subcadena) {
            sscanf(subcadena, "effort_min %lf",&init_value3);
            new_type.effort_min=init_value3;
        }
        subcadena=strstr(message, "kick_power_rate ");
        if (subcadena) {
            sscanf(subcadena, "kick_power_rate %lf",&init_value3);
            new_type.kick_power_rate=init_value3;
        }
        subcadena=strstr(message, "foul_detect_probability ");
        if (subcadena) {
            sscanf(subcadena, "foul_detect_probability %lf",&init_value3);
            new_type.foul_detect_probability=init_value3;
        }
        subcadena=strstr(message, "catchable_area_l_stretch ");
        if (subcadena) {
            sscanf(subcadena, "catchable_area_l_stretch %lf",&init_value3);
            new_type.catchable_area_l_stretch=init_value3;
        }
        game_data->game_parameter.player_types.push_back( new_type );

}

void Parser::parse_see(char *message)
{


    //std::cout << "parse see begin " << endl;
    int int_value4, int_value5;
    double flo_value, flo_value1, flo_value2, flo_value3;
    int unum, count, j, k, l, g;
    char * subcadena2 = NULL;
    char * subcadena = NULL;
    char * subcadena3 = NULL;

    count = 0;

    sscanf(message,"(see %d",&int_value4);

    game_data->sensor_handler.begin_see( int_value4 );


    subcadena2=strstr(message,"(f t l 50)"); // 1
 if (subcadena2 != NULL)
     flpar( subcadena2,FTL50);

     subcadena2=strstr(message,"(f t l 40)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FTL40);

     subcadena2=strstr(message,"(f t l 30)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FTL30);

     subcadena2=strstr(message,"(f t l 20)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FTL20);

     subcadena2=strstr(message,"(f t l 10)");// 5
 if (subcadena2 != NULL)
     flpar( subcadena2,FTL10);


     subcadena2=strstr(message,"(f t r 10)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FTR10);

     subcadena2=strstr(message,"(f t r 20)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FTR20);

     subcadena2=strstr(message,"(f t r 30)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FTR30);

     subcadena2=strstr(message,"(f t r 40)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FTR40);

        subcadena2=strstr(message,"(f t r 50)"); // 10
 if (subcadena2 != NULL)
     flpar( subcadena2,FTR50);

     subcadena2=strstr(message,"(f r t 10)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FRT10);

     subcadena2=strstr(message,"(f r t 20)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FRT20);

        subcadena2=strstr(message,"(f r t 30)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FRT30);

     subcadena2=strstr(message,"(f r b 10)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FRB10);

         subcadena2=strstr(message,"(f r b 20)"); // 15
 if (subcadena2 != NULL)
     flpar( subcadena2,FRB20);

        subcadena2=strstr(message,"(f r b 30)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FRB30);

     subcadena2=strstr(message,"(f b r 50)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FBR50);

     subcadena2=strstr(message,"(f b r 40)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FBR40);

     subcadena2=strstr(message,"(f b r 30)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FBR30);

     subcadena2=strstr(message,"(f b r 20)"); // 20
 if (subcadena2 != NULL)
     flpar( subcadena2,FBR20);

     subcadena2=strstr(message,"(f b r 10)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FBR10);

 subcadena2=strstr(message,"(f b l 10)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FBL10);

     subcadena2=strstr(message,"(f b l 20)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FBL20);

     subcadena2=strstr(message,"(f b l 30)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FBL30);

     subcadena2=strstr(message,"(f b l 40)"); // 25
 if (subcadena2 != NULL)
     flpar( subcadena2,FBL40);

     subcadena2=strstr(message,"(f b l 50)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FBL50);

     subcadena2=strstr(message,"(f l b 30)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FLB30);

     subcadena2=strstr(message,"(f l b 20)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FLB20);

     subcadena2=strstr(message,"(f l b 10)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FLB10);


     subcadena2=strstr(message,"(f l t 10)"); // 30
 if (subcadena2 != NULL)
     flpar( subcadena2,FLT10);

     subcadena2=strstr(message,"(f l t 20)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FLT20);

     subcadena2=strstr(message,"(f l t 30)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FLT30);


     subcadena2=strstr(message,"(f g r t)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FGRT);


 subcadena2 = strstr( message, "(f g l t)");
 if( subcadena2 != NULL )
     flpar( subcadena2, FGLT );

 subcadena2 = strstr( message, "(f g l b)" ); // 35
 if( subcadena2 != NULL )
     flpar( subcadena2, FGLB );


 subcadena2 = strstr( message, "(f g r b)" );
 if( subcadena2 != NULL )
     flpar( subcadena2, FGRB );

     subcadena2=strstr(message,"(f p l t)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FPLT);

        subcadena2=strstr(message,"(f p r t)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FPRT);

     subcadena2=strstr(message,"(f p r c)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FPRC);

     subcadena2=strstr(message,"(f p r b)"); // 40
 if (subcadena2 != NULL)
     flpar( subcadena2,FPRB);

     subcadena2=strstr(message,"(f p l b)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FPLB);

 subcadena2=strstr(message,"(f p l c)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FPLC);

 subcadena2=strstr(message,"(f r b)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FRB);

 subcadena2=strstr(message,"(f c b)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FCB);

 subcadena2=strstr(message,"(f l b)"); // 45
 if (subcadena2 != NULL)
     flpar( subcadena2,FLB);

 subcadena2=strstr(message,"(f r t)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FRT);

 subcadena2=strstr(message,"(f c t)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FCT);

 subcadena2=strstr(message,"(f l t)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FLT);

 subcadena2=strstr(message,"(f t 0)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FT0);

 subcadena2=strstr(message,"(f b 0)"); // 50
 if (subcadena2 != NULL)
 flpar( subcadena2,FB0);

 subcadena2=strstr(message,"(f l 0)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FL0);

 subcadena2=strstr(message,"(f r 0)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FR0);

    subcadena2=strstr(message,"(f c)");
 if (subcadena2 != NULL)
     flpar( subcadena2,FC);

 subcadena2=strstr(message,"(g r)");
 if (subcadena2 != NULL)
     flpar( subcadena2,GR);

 subcadena2=strstr(message,"(g l)"); // 55
 if (subcadena2 != NULL)
     flpar( subcadena2,GL);


        //lineas

                subcadena2=strstr(message,"l t");
        if (subcadena2 != NULL){
            lipar( subcadena2, LINE_TOP);

        }
                subcadena2=strstr(message,"l r");
        if (subcadena2 != NULL){
            lipar( subcadena2, LINE_RIGHT);
        }
               subcadena2=strstr(message,"l b");
        if (subcadena2 != NULL){
            lipar( subcadena2, LINE_BOTTOM);

        }
            subcadena2=strstr(message,"l l");
        if (subcadena2 != NULL){

            lipar( subcadena2, LINE_LEFT);
        }

    //ball

            subcadena2=strstr(message,"(b");
        if (subcadena2 != NULL)
        {
            subcadena=strstr(subcadena2,")");
            if (subcadena){
                j=strlen(subcadena);
                subcadena3=strstr(subcadena+1,")");
                l=strlen(subcadena3);
                k=j-l;
                for (g=0;g<=k;g++){
                    if(isspace(*subcadena)){
                        count++;
                    }
                    subcadena++;
                }
                if(count==6){
                    subcadena=strstr(subcadena2,")");
                    //sscanf(subcadena+1," %lf %d %lf %lf %lf %d",&flo_value,&int_value5,&flo_value1,&flo_value2,&flo_value3,&int_value6);

                    count=0;
                }
                if(count==5){
                    subcadena=strstr(subcadena2,")");
                    sscanf(subcadena+1," %lf %d %lf %lf %lf",&flo_value,&int_value5,&flo_value1,&flo_value2,&flo_value3);
                    //game_data->sensor_handler.see_ball(flo_value,int_value5,flo_value1,flo_value2,flo_value3);

                    count=0;
                }
                if(count==4){
                    subcadena=strstr(subcadena2,")");
                    sscanf(subcadena+1," %lf %d %lf %lf",&flo_value,&int_value5,&flo_value1,&flo_value2);
                    game_data->sensor_handler.see_ball(flo_value,int_value5,flo_value1,flo_value2);
                    count=0;
                }
                if(count==3){
                    subcadena=strstr(subcadena2,")");
                    sscanf(subcadena+1," %lf %d %lf",&flo_value,&int_value5,&flo_value1);
                    //game_data->sensor_handler.see_ball(flo_value,int_value5,flo_value1);
                    count=0;
                }
                if(count==2){
                    subcadena=strstr(subcadena2,")");
                    sscanf(subcadena+1," %lf %d",&flo_value,&int_value5);
                    game_data->sensor_handler.see_ball(flo_value,int_value5);
                    count=0;
                }
                if(count==1){
                    subcadena=strstr(subcadena2,")");
                    sscanf(subcadena+1," %lf",&flo_value);
                    //game_data->sensor_handler.see_ball(flo_value);
                    count=0;
                }
            }
        }

        subcadena2 = strstr( message,"(B" );
        if ( subcadena2 != NULL )
        {
            float dis;
            int dir;
            subcadena2 += 4; // Nos saltamos "(B) "
            sscanf( subcadena2, "%f %d )", &dis, &dir );
            game_data->sensor_handler.see_ball(dis, dir);

        }

        //Jugadores

        // name info
        int  name_ndata  	= 0; // Número de datos del nombre (equipo, número,...)
        bool has_team 		= false;
        bool has_unum 		= false;
        bool has_goalie 	= false;

        // regular info
        int  player_ndata 	= 0; // Número de datos del jugador (distancia, dirección, ...)
        double dist 	= 0.0;
        double dir		= 0.0;
        double distch   = 0.0;
        double dirch	= 0.0;
        double body		= 0.0;
        double head		= 0.0;
        double pointdir = 0.0;
        bool   tackle   = false;
        bool   kick	    = false;

        // auxiliar variables

        char * aux_cad 		= NULL;
        int n_readed		= 0;
        char team_name[128];

        // situamos a subcadena en la posición del primer jugador (si lo hay)
        // y repetimos para todos los jugadores

        // Falta tomar en cuenta el caso cuando está detrás el jugador:
        // ((P) <DISTANCE> <DIRECTION>)
        subcadena = strstr( message, "(p" );
        while( subcadena )
        {
        	team_name[0] = '\0';
        	unum 		 = NDEF_NUM;
        	has_goalie	 = false;

            player_ndata= 0; // Número de datos del jugador (distancia, dirección, ...)
            dist 		= NDEF_NUM;
            dir			= NDEF_NUM;
            distch   	= NDEF_NUM;
            dirch		= NDEF_NUM;
            body		= NDEF_NUM;
            head		= NDEF_NUM;
            pointdir	= NDEF_NUM;
            tackle   	= false;
            kick	    = false;
            n_readed	= 0;

        	aux_cad = subcadena; // Usamos aux_cad para contar los parámetros

        	// Saltamos "(p"
        	aux_cad = aux_cad + 2;

        	// Contamos cuántos datos del nombre tenemos
            while( *aux_cad != ')' )
            {
                if( *aux_cad == ' ' )
                    name_ndata++;
                // El nombre podría contener espacios, saltamos todo el nombre
                // es decir, lo que está dentro de las comillas.
                if( *aux_cad == '\"')
                    do aux_cad++; while( *aux_cad != '\"' );

                aux_cad++;
            }

            // Tomaré todas las combinaciones por si se llegan a necesitar, aunque las usuales son:
            // (p), (p "TEAMNAME"), (p "TEAMNAME" UNUM), (p "TEAMNAME" UNUM goalie), ....


            switch( name_ndata )
            {
            // En todos los casos, el puntero termina en el paréntesis que cierra los datos del nombre
            case 0: // (p)
            	n_readed = 0;
            	subcadena += 2;
            	break;
            case 1:
            	subcadena += 3; // Saltamos "(p "
            	if( *subcadena == '\"' ) // (p "TEAMNAME")
            	{
            		has_team = true;
            		sscanf( subcadena, " \"%[-0-9a-zA-Z ().+*/?<>_]\" %n", team_name, &n_readed );
            		subcadena += n_readed;
            	}
            	else if( isdigit( *subcadena ) ) // (p UNUM)
            	{
            		has_unum = true;
            		sscanf( subcadena, " %d %n", &unum, &n_readed );
            		subcadena += n_readed;
            	}
            	else if( *subcadena == 'g' ) // // (p goalie)
            	{
            		has_goalie = true;
            		subcadena += 6; // "goalie"
            	}
            	break;
            case 2:
            	subcadena += 3; // Saltamos "(p "
            	if( *subcadena == '\"' ) // (p "TEAMNAME"...
            	{
            		has_team = true;
            		sscanf( subcadena, " \"%[-0-9a-zA-Z ().+*/?<>_]\" %n", team_name, &n_readed );
            		subcadena += n_readed;
            		if( isdigit( *subcadena ) ) // (p "TEAMNAME" UNUM)
            		{
            			has_unum = true;
            			sscanf( subcadena, "%d %n", &unum, &n_readed );
            			subcadena += n_readed;
            		}
            		else if( *subcadena == 'g' ) // (p "TEAMNAME" goalie)
            		{
            			has_goalie = true;
            			subcadena += 6;
            		}
            	}
            	else if( isdigit( *subcadena ) ) // (p UNUM
            	{
            		has_unum = true;
            		sscanf( subcadena, "%d %n", &unum, &n_readed );
            		subcadena += n_readed;
            		if( *subcadena == '\"' ) // (p UNUM "TEAMNAME")
            		{
            			has_team = true;
            			sscanf( subcadena, " \"%[-0-9a-zA-Z ().+*/?<>_]\" %n", team_name, &n_readed );
            			subcadena += n_readed;
            		}
            		else if( *subcadena == 'g' )
            		{
            			has_goalie = true;
            			subcadena += 6;
            		}
            	}
            	else if( *subcadena == 'g' ) // // (p goalie
            	{
            		has_goalie = true;
            		subcadena += 6; // goalie
            		subcadena ++; // El espacio
            		if( isdigit( *subcadena ) )
            		{
            			has_unum = true;
            			sscanf( subcadena, "%d %n", &unum, &n_readed );
            			subcadena += n_readed;
            		}
            		else if( *subcadena == '\"')
            		{
            			has_team = true;
            			sscanf( subcadena, " \"%[-0-9a-zA-Z ().+*/?<>_]\" %n", team_name, &n_readed );
            			subcadena += n_readed;
            		}
            	}
            	break;
            case 3:
            	has_goalie  = true;
            	has_unum	= true;
            	has_team	= true;
            	subcadena += 3;
            	if( *subcadena == '\"' ) // (p "TEAMNAME"...
            	{
            		sscanf( subcadena, " \"%[-0-9a-zA-Z ().+*/?<>_]\" %n", team_name, &n_readed );
            		subcadena += n_readed;
            		if( isdigit( *subcadena ) ) // (p "TEAMNAME" UNUM goalie)
            		{
            			sscanf( subcadena, "%d %n", &unum, &n_readed );
            			subcadena += n_readed;
            			subcadena += 6;
            		}
            		else if( *subcadena == 'g' ) // (p "TEAMNAME" goalie UNUM)
            		{
            			subcadena += 6; // goalie
            			subcadena ++; // espacio
            			sscanf( subcadena, "%d %n", &unum, &n_readed );
            			subcadena += n_readed;
            		}
            	}
            	else if( isdigit( *subcadena ) ) // (p UNUM...
            	{
            		sscanf( subcadena, "%d %n", &unum, &n_readed );
            		subcadena += n_readed;
            		subcadena ++;
            		if( *subcadena == '\"' ) // (p UNUM "TEAMNAME" goalie)
            		{
            			sscanf( subcadena, " \"%[-0-9a-zA-Z ().+*/?<>_]\" %n", team_name, &n_readed );
            			subcadena += n_readed;
            			subcadena ++; // espacio
            			subcadena += 6; // goalie
            		}
            		else if( *subcadena == 'g' ) //(p UNUM goalie "TEAMNAME")
            		{

            			subcadena += 6; // goalie
            			subcadena ++;	// espacio
            			sscanf( subcadena, " \"%[-0-9a-zA-Z ().+*/?<>_]\" %n", team_name, &n_readed );
            			subcadena += n_readed;
            		}

            	}
            	else if( *subcadena == 'g' )
            	{
            		subcadena += 6; // goalie
            		subcadena ++; // espacio
            		if( isdigit( *subcadena ) ) // (p goalie UNUM "TEAMNAME")
            		{
            			sscanf( subcadena,
            					"%d \"%[-0-9a-zA-Z ().+*/?<>_]\" %n",
            					&unum, team_name, &n_readed );
            			subcadena += n_readed;

            		}
            		else if( *subcadena == '\"' ) // (p goalie "TEAMNAME" UNUM)
            		{
            			sscanf( subcadena,
            					"\"%[-0-9a-zA-Z ().+*/?<>_]\" %d %n",
            					team_name, &unum, &n_readed );
            			subcadena += n_readed;
            		}
            	}
            	break;
            }

            // Saltamos al paréntesis que cierra la info del nombre
            while( *subcadena != ')' )
            	subcadena++;
            subcadena ++; // Saltamos el paréntesis


            aux_cad = subcadena; // aux_cad y subcadena ahora está en la posicion del primer parametro
            while( (*aux_cad) != ')' ) //contamos los parametros
            {
            	if( isspace(*aux_cad) )
            		player_ndata ++;
            	aux_cad++;
            }

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


            // subcadena está en la posicion del primer parametro
            switch( player_ndata )
            {
            case 1: // " <DIR>)" : space = 1
            	sscanf( subcadena, "%lf %n", &dir, &n_readed );
            	subcadena += n_readed;
            	break;
            case 2:// " <DIST> <DIR>)" : space = 2
            	sscanf( subcadena, "%lf %lf %n", &dist, &dir, &n_readed );
            	subcadena += n_readed;
            	break;
            case 3:// " <DIST> <DIR> <TACKLE|KICK>)" : space = 3
            	sscanf( subcadena, "%lf %lf %n", &dist, &dir, &n_readed );
            	subcadena += n_readed;
            	if( *subcadena == 'k' )
            	{
            		kick = true;
            		subcadena += 4; // kick
            	}
            	else if( *subcadena == 't' )
            	{
            		tackle = true;
            		subcadena += 6; // tackle
            	}
            	break;
            case 4:
            	// " <DIST> <DIR> <POINTDIR> <TACKLE|KICK>)" : space = 4
            	// " <DIST> <DIR> <DISTCH> <DIRCH>)" : space = 4

            	sscanf( subcadena, "%lf %lf %n", &dir, &dist, &n_readed );
            	subcadena += n_readed;
            	aux_cad = subcadena;
            	aux_cad = move_to_next_word( aux_cad );
            	if( *aux_cad == 't' ) // " <DIST> <DIR> <POINTDIR> tackle)
            	{
            		tackle = true;
            		sscanf( subcadena,
            				"%lf tackle %n",
            				&pointdir, &n_readed );
                    subcadena += n_readed;

            	}
            	else if( *aux_cad == 'k' ) // " <DIST> <DIR> <POINTDIR> kick)
            	{
            		kick = true;
            		sscanf( subcadena,
            				"%lf kick %n",
            				&pointdir, &n_readed );
                    subcadena += n_readed;
            	}
            	else if( isdigit( *aux_cad ) ) //  " <DIST> <DIR> <DISTCH> <DIRCH>)
            	{
            		sscanf( subcadena,
            				"%lf %lf %n",
            				&distch, &dirch, &n_readed );
                    subcadena += n_readed;
            	}
            	break;
            case 5:// " <DIST> <DIR> <DISTCH> <DIRCH> <BODY>)" : space = 5  only sserver-4
            	sscanf( subcadena,
            			"%lf %lf %lf %lf %lf %n",
            			&dist, &dir, &distch, &dirch, &body, &n_readed );
                subcadena += n_readed;
                break;
            case 6: // " <DIST> <DIR> <DISTCH> <DIRCH> <BODY> <HEAD>)" : space = 6
            	sscanf( subcadena,
            			"%lf %lf %lf %lf %lf",
            			&dist, &dir, &distch, &dirch, &body, &head, &n_readed );
                subcadena += n_readed;
                break;
            case 7:
            	// " <DIST> <DIR> <DISTCH> <DIRCH> <BODY> <HEAD> <POINTDIR>)" : space = 7
            	// " <DIST> <DIR> <DISTCH> <DIRCH> <BODY> <HEAD> <TACKLE|KICK>)" : space = 7
            	sscanf( subcadena,
            			"%lf %lf %lf %lf %lf %lf %n",
            			&dist, &dir, &distch, &dirch, &body, &head, &n_readed );
                subcadena += n_readed;
                //std::cout << subcadena << std::endl;
                if( isdigit(*subcadena) ) // " <DIST> <DIR> <DISTCH> <DIRCH> <BODY> <HEAD> <POINTDIR>)"
                {
                	sscanf( subcadena,
                			"%lf %n",
                			&pointdir, &n_readed );
                }
                else if( *subcadena == 't' ) // " <DIST> <DIR> <DISTCH> <DIRCH> <BODY> <HEAD> tackle)"
                {
                	tackle = true;
                	subcadena += 6;

                }
                else if( *subcadena == 'k' ) // " <DIST> <DIR> <DISTCH> <DIRCH> <BODY> <HEAD> kick)"
                {
                	kick = true;
                	subcadena += 4;
                }
                break;
            case 8:
            	// " <DIST> <DIR> <DISTCH> <DIRCH> <BODY> <HEAD> <POINTDIR> <TACKLE|KICK>)" : space = 8
            	sscanf( subcadena,
            			"%lf %lf %lf %lf %lf %lf %lf %n",
            			&dist, &dir, &distch, &dirch, &body, &head, &pointdir, &n_readed );
                subcadena += n_readed;
                if( *subcadena == 't' )
                {
                	tackle = true;
                	subcadena += 6;
                }
                else if( *subcadena == 'k' )
                {
                	kick = true;
                	subcadena += 4;

                }
                break;
            } // switch player_ndata
            game_data->sensor_handler.see_player( team_name, unum, has_goalie,
                        										  dist,
                        										  dir,
                        										  distch,
                        										  dirch,
                        										  body,
                        										  head,
                        										  pointdir,
                        										  tackle,
                        										  kick
                        										  );

             //Para probar
            /*
            std::cout << "name_ndata = " << name_ndata << std::endl;
			std::cout << "has_team   = " << has_team << std::endl;
                    std::cout << "has_unum   = " << has_unum << std::endl;
                    std::cout << "has_goalie = " << has_goalie << std::endl;
                    std::cout << "team_name  = " << team_name << std::endl;
                    std::cout << "unum       = " << unum << std::endl;
                    std::cout << std::endl;
                    std::cout << "player_ndata = " << player_ndata << std::endl;
                    std::cout << "dist = " << dist << std::endl;
                    std::cout << "dir = " << dir << std::endl;
                    std::cout << "distch = " << distch << std::endl;
                    std::cout << "dirch = " << dirch << std::endl;
                    std::cout << "body = " << body << std::endl;
                    std::cout << "head = " << head << std::endl;
                    std::cout << "pointdir = " << pointdir << std::endl;
                    std::cout << "kick = " << kick << std::endl;
                    std::cout << "tackle = " << tackle << std::endl;
                    std::cout << subcadena << std::endl;
                    //*/


        // Repetimos para todos los jugadores
        subcadena = strstr( subcadena, "(p" );
    }

}

void Parser::parse_sense(char * message )
{
    int int_value, int_value2, int_value3;
    char char_value, * char_s_value, * char_s_value2;
    char * subcadena;
    double double_value, double_value2;
    char_s_value  = (char*)malloc(100*sizeof(char) );
    char_s_value2 = (char*)malloc(100*sizeof(char) );
    subcadena = NULL;

    sscanf(message,"(sense_body %d", &int_value);

    game_data->sensor_handler.begin_sense( int_value ); //enviamos el tiempo

    subcadena = strstr(message, "view_mode");
    sscanf(subcadena,"view_mode %s %s", char_s_value, char_s_value2);

    if( strstr( char_s_value, "high" ) )
    {
        game_data->sensor_handler.sense_view_mode_quality( HIGH );
    }
    else
        game_data->sensor_handler.sense_view_mode_quality( LOW );

    if( strstr( char_s_value2, "narrow" ) )
    {
        game_data->sensor_handler.sense_view_mode_width( NARROW );
    }
    else if( strstr( char_s_value2, "normal") )
    {
        game_data->sensor_handler.sense_view_mode_width( NORMAL );
    }
    else
        game_data->sensor_handler.sense_view_mode_width( WIDE );
    subcadena = strstr(message, "stamina");
    if( subcadena )
    {
    //sscanf(subcadena,"stamina %lf %lf %lf", &(->stamina_1_stamina), &(this->stamina_2_effort), &(this->stamina_3) ); a'un no se si es entero o flotante la stamina
    sscanf(subcadena,"stamina %lf %lf %d", &double_value, &double_value2, &int_value3 );
    game_data->sensor_handler.sense_stamina( double_value );
    game_data->sensor_handler.sense_effort ( double_value2 );
    game_data->sensor_handler.sense_staminaCapacity(int_value3 );
    }

    subcadena = strstr(message, "speed");
    if( subcadena )
    {
        //sscanf(subcadena, "speed %lf %lf", &this->speed_1_amount, &this->speed_2_direction);
        sscanf(subcadena, "speed %lf %lf", &double_value, &double_value2);
        game_data->sensor_handler.sense_speed_amount( double_value );
        game_data->sensor_handler.sense_speed_direction( double_value2 );
        //std::cout << double_value2 << endl;
    }

    subcadena = strstr(message, "head_angle");
    if( subcadena )
    {
        //sscanf(subcadena, "head_angle %d", &this->head_angle);
        sscanf(subcadena, "head_angle %d", &int_value);
        game_data->sensor_handler.sense_head_angle( int_value );
    }


    subcadena = strstr(message, "kick");
    if( subcadena )
    {
        sscanf(subcadena, "kick %d", &int_value);
        game_data->sensor_handler.sense_kick( int_value );
    }

    subcadena = strstr(message, "dash");
    if( subcadena )
    {
        sscanf(subcadena, "dash %d", &int_value );
        game_data->sensor_handler.sense_dash( int_value );
    }

    subcadena = strstr(message, "turn");
    if( subcadena )
    {
        sscanf(subcadena, "turn %d", &int_value );
        game_data->sensor_handler.sense_turn( int_value );
    }

    subcadena = strstr(message, "say");
    if( subcadena )
    {
        sscanf(subcadena, "say %d", &int_value);
        game_data->sensor_handler.sense_say( int_value );
    }


    subcadena = strstr(message, "turn_neck");
    if( subcadena )
    {
        sscanf(subcadena, "turn_neck %d", &int_value);
        game_data->sensor_handler.sense_turn_neck( int_value );
    }


    subcadena = strstr(message, "catch");
    if( subcadena )
    {
        sscanf(subcadena, "catch %d", &int_value);
        game_data->sensor_handler.sense_catchh( int_value );
    }


    subcadena = strstr(message, "move");
    if( subcadena )
    {
        sscanf(subcadena, "move %d", &int_value);
        game_data->sensor_handler.sense_move( int_value );
    }


    subcadena = strstr(message, "change_view" );
    if( subcadena )
    {
        sscanf(subcadena, "change_view %d", &int_value );
        game_data->sensor_handler.sense_change_view( int_value );
    }


    subcadena = strstr(message, "arm");
    if( subcadena )
    {
        subcadena = strstr(subcadena, "movable");
        if( subcadena )
        {
            sscanf(subcadena, "movable %d", &int_value);
            game_data->sensor_handler.sense_arm_movable( int_value );
        }

        subcadena = strstr(subcadena, "expires");
        if( subcadena )
        {
            sscanf(subcadena, "expires %d", &int_value);
            game_data->sensor_handler.sense_arm_expires( int_value );
        }
        subcadena = strstr(subcadena, "target");
        if( subcadena )
        {
            sscanf(subcadena, "target %d %d", &int_value, &int_value2);
            game_data->sensor_handler.sense_arm_target_1( int_value );
            game_data->sensor_handler.sense_arm_target_2( int_value2 );

        }
        subcadena = strstr(subcadena, "count");
        if( subcadena )
        {
            sscanf(subcadena, "count %d", &int_value);
            game_data->sensor_handler.sense_arm_count( int_value );
        }
    }


    subcadena = strstr(message, "focus");
    if( subcadena )
    {
        //pendiente, aun no concemos los valores que puede tomar target

        subcadena = strstr(subcadena, "target");
        if( subcadena[7] == 'n' )  //subcadena[7] podria contener el inicio de 'none', o el lado del jugador
        {
            game_data->sensor_handler.sense_focus_target( 'n', 0 );
        }
        else
        {
            sscanf(subcadena, "target %c %d", &char_value, &int_value );
            game_data->sensor_handler.sense_focus_target( char_value, int_value);
        }

        subcadena = strstr(subcadena, "count");
        if( subcadena )
        {
            sscanf(subcadena, "count %d", &int_value);
            game_data->sensor_handler.sense_focus_count( int_value );
        }
    }


    subcadena = strstr(message, "tackle");
    if( subcadena )
    {
        subcadena = strstr(subcadena, "expires");
        sscanf(subcadena, "expires %d", & int_value);
        game_data->sensor_handler.sense_tackle_expires( int_value );
        subcadena = strstr(subcadena, "count");
        sscanf(subcadena, "count %d", &int_value );
        game_data->sensor_handler.sense_tackle_count( int_value );
    }

    subcadena = strstr(message, "collision");
    if( subcadena )
    {
        switch ( subcadena[10] ) //no he enlistado los tipos de collision
        {
        case 'n':
            game_data->sensor_handler.sense_collision( COLLISION_NONE );
            break;
        default:
            game_data->sensor_handler.sense_collision( COLLISION_NONE );
            break;
        }
    }

    subcadena = strstr(message, "foul");
    if( subcadena )
    {
        subcadena = strstr(subcadena, "charged");
        sscanf(subcadena, "charged %d", &int_value);
        game_data->sensor_handler.sense_foul_charged( int_value );
        subcadena = strstr(subcadena, "card");
        if      ( subcadena[5] == 'n' )
            game_data->sensor_handler.sense_foul_card( FCARD_NONE );
        else if ( subcadena[5] == 'y' )
            game_data->sensor_handler.sense_foul_card( FCARD_YELLOW );
        else if ( subcadena[5] == 'r' )
            game_data->sensor_handler.sense_foul_card( FCARD_RED );
        else
            game_data->sensor_handler.sense_foul_card( FCARD_NONE );
    }
    free( char_s_value );
    free( char_s_value2 );
    //game_data->sensor_handler.last_sense.print();
}

void Parser::parse_server_param(char *message)
{
    double init_value;
    int int_value;
        char * char_s_value;
        char *subcadena = NULL;
        char_s_value  = (char*)malloc(100*sizeof(char) );

        subcadena = strstr(message, "audio_cut_dist");
        if (subcadena){
            sscanf(subcadena,"audio_cut_dist %d", &int_value);
        game_data->game_parameter.server_param.audio_cut_dist=int_value;
        }
        subcadena = strstr(message, "auto_mode");
        if (subcadena){
            sscanf(subcadena,"auto_mode %d", &int_value);
            game_data->game_parameter.server_param.auto_mode=int_value;}
        subcadena = strstr(message, "back_dash_rate");
        if (subcadena){
            sscanf(subcadena,"back_dash_rate %lf", &init_value);
            game_data->game_parameter.server_param.back_dash_rate=init_value;}
            subcadena=strstr(message, "back_passes");
        if (subcadena){
            sscanf( subcadena,"back_passes %d", &int_value);
            game_data->game_parameter.server_param.back_passes=int_value;}
        subcadena=strstr(message,"ball_accel_max");
        if (subcadena){
            sscanf(subcadena,"ball_accel_max %lf", &init_value);
            game_data->game_parameter.server_param.ball_accel_max=init_value;}
        subcadena=strstr(message, "ball_decay");
        if (subcadena){
            sscanf(subcadena,"ball_decay %lf", &init_value);
            game_data->game_parameter.server_param.ball_decay=init_value;}
        subcadena=strstr(message, "ball_rand");
        if(subcadena){
            sscanf(subcadena, "ball_rand %lf", &init_value);
            game_data->game_parameter.server_param.ball_rand=init_value;}
        subcadena=strstr(message,"ball_size");
        if (subcadena){
            sscanf(subcadena,"ball_size %lf", &init_value);
            game_data->game_parameter.server_param.ball_size=init_value;}
        subcadena=strstr(message, "ball_speed_max");
        if (subcadena){
            sscanf(subcadena, "ball_speed_max %d", &int_value);
            game_data->game_parameter.server_param.ball_speed_max=int_value;}
        subcadena=strstr(message, "ball_stuck_area");
        if (subcadena){
            sscanf(subcadena,"ball_stuck_area %d", &int_value);
            game_data->game_parameter.server_param.ball_stuck_area=int_value;}
        subcadena=strstr(message,"ball_weight");
        if (subcadena){
            sscanf(subcadena,"ball_weight %lf", &init_value);
            game_data->game_parameter.server_param.ball_weight=init_value;}
        subcadena=strstr(message,"catch_ban_cycle");
        if (subcadena){
            sscanf(subcadena,"catch_ban_cycle %d", &int_value);
            game_data->game_parameter.server_param.catch_ban_cycle=int_value;}
        subcadena=strstr(message, "catch_probability");
        if (subcadena){
            sscanf(subcadena,"catch_probability %lf", &init_value);
            game_data->game_parameter.server_param.catch_probability=init_value;}
        subcadena=strstr(message,"catchable_area_l");
        if (subcadena){
            sscanf(subcadena, "catchable_area_l %lf", &init_value);
            game_data->game_parameter.server_param.catchable_area_l=init_value;}
        subcadena=strstr(message,"catchable_area_w");
        if (subcadena){
            sscanf(subcadena,"catchable_area_w %lf", &init_value);
            game_data->game_parameter.server_param.catchable_area_w=init_value;}
        subcadena=strstr(message, "ckick_margin");
        if(subcadena){
            sscanf(subcadena,"ckick_margin %d", &int_value);
            game_data->game_parameter.server_param.ckick_margin=int_value;}
        subcadena=strstr(message, "clang_advice_win");
        if (subcadena){
            sscanf(subcadena,"clang_advice_win %d", &int_value);
            game_data->game_parameter.server_param.clang_advice_win=int_value;}
        subcadena=strstr(message, "clang_define_win");
        if(subcadena){
            sscanf(subcadena,"clang_define_win %d", &int_value);
            game_data->game_parameter.server_param.clang_define_win=int_value;}
        subcadena=strstr(message, "clang_del_win");
        if(subcadena){
            sscanf(subcadena, "clang_del_win %d", &int_value);
            game_data->game_parameter.server_param.clang_del_win=int_value;}
        subcadena=strstr(message, "clang_info_win");
        if(subcadena){
            sscanf(subcadena, "clang_info_win %d", &int_value);
            game_data->game_parameter.server_param.clang_info_win=int_value;}
        subcadena=strstr(message, "clang_mess_delay");
        if (subcadena){
            sscanf(subcadena, "clang_mess_delay %d", &int_value);
            game_data->game_parameter.server_param.clang_mess_delay=int_value;}
        subcadena=strstr(message, "clang_mess_per_cycle");
        if(subcadena){
            sscanf(subcadena, "clang_mess_per_cycle %d", &int_value);
            game_data->game_parameter.server_param.clang_mess_per_cycle=int_value;}
        subcadena=strstr(message,"clang_meta_win");
        if (subcadena){
            sscanf(subcadena, "clang_meta_win %d", &int_value);
            game_data->game_parameter.server_param.clang_meta_win=int_value;}
        subcadena=strstr(message,"clang_rule_win");
        if (subcadena){
            sscanf(subcadena, "clang_rule_win %d", &int_value);
            game_data->game_parameter.server_param.clang_rule_win=int_value;}
        subcadena=strstr(message,"clang_win_size");
        if (subcadena){
            sscanf(subcadena, "clang_win_size %d", &int_value);
            game_data->game_parameter.server_param.clang_win_size=int_value;}
        subcadena=strstr(message, "coach");
        if (subcadena){
            sscanf(subcadena, "coach %d", &int_value);
            game_data->game_parameter.server_param.coach=int_value;}
        subcadena=strstr(message, "coach_msg_file");
        if (subcadena){
            sscanf(subcadena, "coach_msg_file %s", char_s_value);
            game_data->game_parameter.server_param.coach_msg_file=char_s_value;}
        subcadena=strstr(message, "coach_port");
        if (subcadena){
            sscanf(subcadena,"coach_port %d", &int_value);
            game_data->game_parameter.server_param.coach_port=int_value;}
        subcadena=strstr(message, "coach_w_referee");
        if(subcadena){
            sscanf(subcadena,"coach_w_referee %d", &int_value);
            game_data->game_parameter.server_param.coach_w_referee=int_value;}
        subcadena=strstr(message, "connect_wait");
        if (subcadena){
            sscanf(subcadena, "connect_wait %d", &int_value);
            game_data->game_parameter.server_param.connect_wait=int_value;}
        subcadena=strstr(message,"control_radius");
        if (subcadena){
            sscanf(subcadena,"control_radius %d", &int_value);
            game_data->game_parameter.server_param.control_radious=int_value;}
        subcadena=strstr(message,"dash_angle_step");
        if (subcadena){
            sscanf(subcadena,"dash_angle_step %d",&int_value);
            game_data->game_parameter.server_param.dash_angle_step=int_value;}
        subcadena=strstr(message,"dash_power_rate");
        if (subcadena){
            sscanf(subcadena,"dash_power_rate %lf",&init_value);
            game_data->game_parameter.server_param.dash_power_rate=init_value;}
        subcadena=strstr(message,"drop_ball_time");
        if (subcadena){
            sscanf(subcadena,"drop_ball_time %d",&int_value);
            game_data->game_parameter.server_param.drop_ball_time=int_value;}
        subcadena=strstr(message,"effort_dec");
        if (subcadena){
            sscanf(subcadena,"effort_dec %lf",&init_value);
            game_data->game_parameter.server_param.effort_dec=init_value;}
        subcadena=strstr(message,"effort_dec_thr");
        if (subcadena){
            sscanf(subcadena,"effort_dec_thr %lf",&init_value);
            game_data->game_parameter.server_param.effort_dec_thr=init_value;}
        subcadena=strstr(message,"effort_dec_inc");
        if (subcadena){
            sscanf(subcadena,"effort_dec_inc %lf",&init_value);
            game_data->game_parameter.server_param.effort_dec_inc = init_value;}
        subcadena=strstr(message,"effort_inc");
        if (subcadena){
            sscanf(subcadena,"effort_inc %lf",&init_value);
            game_data->game_parameter.server_param.effort_inc=init_value;}
        subcadena=strstr(message,"effort_inc_thr");
        if (subcadena){
            sscanf(subcadena,"effort_inc_thr %lf",&init_value);
            game_data->game_parameter.server_param.effort_inc_thr =init_value;}
        subcadena=strstr(message,"effort_init");
        if (subcadena){
            sscanf(subcadena,"effort_init %d",&int_value);
            game_data->game_parameter.server_param.effort_init=int_value;}
        subcadena=strstr(message,"effort_min");
        if (subcadena){
            sscanf(subcadena,"effort_min %lf",&init_value);
            game_data->game_parameter.server_param.effort_min=init_value;}
        subcadena=strstr(message,"extra_half_time");
        if (subcadena){
            sscanf(subcadena,"extra_half_time %d",&int_value);
            game_data->game_parameter.server_param.extra_half_time=int_value;}
        subcadena=strstr(message,"extra_stamina");
        if (subcadena){
            sscanf(subcadena,"extra_stamina %d",&int_value);
            game_data->game_parameter.server_param.extra_stamina=int_value;}
        subcadena=strstr(message,"forbid_kick_off_offside");
        if (subcadena){
            sscanf(subcadena,"forbid_kick_off_offside %d",&int_value);
            game_data->game_parameter.server_param.forbid_lick_off_offside=int_value;}
        subcadena=strstr(message,"foul_cycles");
        if (subcadena){
            sscanf(subcadena,"foul_cycles %d",&int_value);
            game_data->game_parameter.server_param.foul_cycles=int_value;}
        subcadena=strstr(message,"foul_detect_probability");
        if (subcadena){
            sscanf(subcadena,"foul_detect_probability %d",&int_value);
            game_data->game_parameter.server_param.foul_detect_probability=int_value;}
        subcadena=strstr(message,"foul_exponent");
        if (subcadena){
            sscanf(subcadena,"foul_exponent %d",&int_value);
            game_data->game_parameter.server_param.foul_exponent=int_value;}
        subcadena=strstr(message,"free_kick_faults");
        if (subcadena){
            sscanf(subcadena,"free_kick_faults %d",&int_value);
            game_data->game_parameter.server_param.free_kick_faults=int_value;}
        subcadena=strstr(message,"freeform_send_period");
        if (subcadena){
            sscanf(subcadena,"freeform_send_period %d", &int_value);
            game_data->game_parameter.server_param.freeform_send_period=int_value;}
        subcadena=strstr(message,"freeform_wait_period");
        if (subcadena){
            sscanf(subcadena,"freeform_wait_period %d", &int_value);
            game_data->game_parameter.server_param.freeform_wait_period=int_value;}
        subcadena=strstr(message,"fullstate_l");
        if (subcadena){
            sscanf(subcadena,"fullstate_l %d",&int_value);
            game_data->game_parameter.server_param.fullstate_l=int_value;}
        subcadena=strstr(message,"fullstate_r");
        if (subcadena){
            sscanf(subcadena,"fullstate_r %d",&int_value);
            game_data->game_parameter.server_param.fullstate_r=int_value;}
        subcadena=strstr(message,"game_log_compression");
        if (subcadena){
            sscanf(subcadena,"game_log_compression %d",&int_value);
            game_data->game_parameter.server_param.game_log_compression=int_value;}
        subcadena=strstr(message,"game_log_dated");
        if (subcadena){
            sscanf(subcadena,"game_log_dated %d",&int_value);
            game_data->game_parameter.server_param.game_log_dated=int_value;}
        subcadena=strstr(message,"game_log_dir");
        if (subcadena){
            sscanf(subcadena,"game_log_dir %s", char_s_value);
            game_data->game_parameter.server_param.game_log_dir=char_s_value;}
        subcadena=strstr(message,"game_log_fixed");
        if (subcadena){
            sscanf(subcadena,"game_log_fixed %d",&int_value);
            game_data->game_parameter.server_param.game_log_fixed=int_value;}
        subcadena=strstr(message,"game_log_fixed_name");
        if (subcadena){
            sscanf(subcadena,"game_log_fixed_name %s", char_s_value);
            game_data->game_parameter.server_param.game_log_fixed_name=char_s_value;}
        subcadena=strstr(message,"game_log_version");
        if (subcadena){
            sscanf(subcadena,"game_log_version %d",&int_value);
            game_data->game_parameter.server_param.game_log_version=int_value;}
        subcadena=strstr(message,"game_logging");
        if (subcadena){
            sscanf(subcadena,"game_logging %d",&int_value);
            game_data->game_parameter.server_param.game_logging=int_value;}
        subcadena=strstr(message,"game_over_wait");
        if (subcadena){
            sscanf(subcadena,"game_over_wait %d",&int_value);
            game_data->game_parameter.server_param.game_over_wait=int_value;}
        subcadena = strstr(message,"goal_wait");/*
        if (subcadena){
            sscanf(subcadena,"goal_wiat %lf",&init_value);
            game_data->game_parameter.server_param.goal_wait=init_value;}*/
        subcadena = strstr(message,"goal_width");
        if (subcadena){
            sscanf(subcadena,"goal_width %lf",&init_value);
            game_data->game_parameter.server_param.goal_width=init_value;}
        subcadena=strstr(message,"goalie_max_moves");
        if (subcadena){
            sscanf(subcadena,"goalie_max_moves %d",&int_value);
            game_data->game_parameter.server_param.goalie_max_moves=int_value;}
        subcadena=strstr(message,"golden_goal");
        if (subcadena){
            sscanf(subcadena,"golden_goal %d",&int_value);
            game_data->game_parameter.server_param.golden_goal=int_value;}
        subcadena=strstr(message,"half_time");
        if (subcadena){
            sscanf(subcadena,"half_time %d", &int_value);
            game_data->game_parameter.server_param.half_time=int_value;}
        subcadena=strstr(message,"hear_inc");
        if (subcadena){
            sscanf(subcadena,"hear_inc %d", &int_value);
            game_data->game_parameter.server_param.hear_inc=int_value;}
        subcadena=strstr(message,"hear_decay");
        if (subcadena){
            sscanf(subcadena,"hear_decay %d", &int_value);
            game_data->game_parameter.server_param.hear_decay = int_value;}
        subcadena=strstr(message,"hear_max");
        if (subcadena){
            sscanf(subcadena,"hear_max %d",&int_value);
            game_data->game_parameter.server_param.hear_max=int_value;}
        subcadena=strstr(message,"inertia_moment");
        if (subcadena){
            sscanf(subcadena,"inertia_moment %d",&int_value);
            game_data->game_parameter.server_param.inertia_moment=int_value;}
        subcadena=strstr(message,"keepaway");
        if (subcadena){
            sscanf(subcadena,"keepaway %d",&int_value);
            game_data->game_parameter.server_param.keepaway=int_value;}
        subcadena=strstr(message,"keepaway_length");
        if (subcadena){
            sscanf(subcadena,"keepaway_length %d", &int_value);
            game_data->game_parameter.server_param.keepaway_length=int_value;}
        subcadena=strstr(message,"keepaway_log_dated");
        if (subcadena){
            sscanf(subcadena,"keepaway_log_dated %d",&int_value);
            game_data->game_parameter.server_param.keepaway_log_dated=int_value;}
        subcadena=strstr(message, "keepaway_log_dir");
        if (subcadena){
            sscanf(subcadena,"keepaway_log_dir %s", char_s_value);
            game_data->game_parameter.server_param.keepaway_log_dir=char_s_value;}
        subcadena=strstr(message,"keepaway_log_fixed");
        if (subcadena){
            sscanf(subcadena,"keepaway_log_fixed %d",&int_value);
            game_data->game_parameter.server_param.keepaway_log_fixed=int_value;}
        subcadena=strstr(message,"keepaway_log_fixed_name");
        if (subcadena){
            sscanf(subcadena,"keepaway_log_fixed_name %s",char_s_value);
            game_data->game_parameter.server_param.keepaway_log_fixed_name=char_s_value;}
        subcadena=strstr(message,"keepaway_logging");
        if (subcadena){
            sscanf(subcadena,"keepaway_logging %d",&int_value);
            game_data->game_parameter.server_param.keepaway_logging=int_value;}
        subcadena=strstr(message,"keepaway_start");
        if (subcadena){
            sscanf(subcadena,"keepaway_start %d",&int_value);
            game_data->game_parameter.server_param.keepaway_start=int_value;}
        subcadena=strstr(message,"keepaway_width");
        if (subcadena){
            sscanf(subcadena,"keepaway_width %lf",&init_value);
            game_data->game_parameter.server_param.keepaway_width=init_value;}
        subcadena=strstr(message,"kick_off_wait");
        if (subcadena){
            sscanf(subcadena,"kick_off_wait %d",&int_value);
            game_data->game_parameter.server_param.kick_off_wait=int_value;}
        subcadena=strstr(message,"kick_power_rate");
        if (subcadena){
            sscanf(subcadena,"kick_power_rate %lf",&init_value);
            game_data->game_parameter.server_param.kick_power_rate=init_value;}
        subcadena=strstr(message,"kick_rand");
        if (subcadena){
            sscanf(subcadena,"kick_rand %lf",&init_value);
            game_data->game_parameter.server_param.kick_rand=init_value;}
        subcadena=strstr(message,"kick_rand_factor_l");
        if (subcadena){
            sscanf(subcadena,"kick_rand_factor_l %d",&int_value);
            game_data->game_parameter.server_param.kick_rand_factor_l=int_value;}
        subcadena=strstr(message,"kick_rand_factor_r");
        if (subcadena){
            sscanf(subcadena,"kick_rand_factor_r %d",&int_value);
            game_data->game_parameter.server_param.kick_rand_factor_r=int_value;}
        subcadena=strstr(message,"kick_margin");
        if (subcadena){
            sscanf(subcadena,"kick_margin %lf",&init_value);
            game_data->game_parameter.server_param.kickable_margin=init_value;}
        subcadena=strstr(message,"landmark_file");
        if (subcadena){
            sscanf(subcadena,"landmark_file %s", char_s_value);
            game_data->game_parameter.server_param.landmark_file=char_s_value;}
        subcadena=strstr(message,"log_date_format");
        if (subcadena){
            sscanf(subcadena,"log_date_format %s", char_s_value);
            game_data->game_parameter.server_param.log_date_format=char_s_value;}
        subcadena=strstr(message,"log_times");
        if (subcadena){
            sscanf(subcadena,"log_times %d",&int_value);
            game_data->game_parameter.server_param.log_times=int_value;}
        subcadena=strstr(message,"max_back_tackle_power");
        if(subcadena){
            sscanf(subcadena,"max_back_tackle_power %d",&int_value);
            game_data->game_parameter.server_param.max_back_tackle_power=int_value;}
        subcadena=strstr(message,"max_dash_angle");
        if (subcadena){
            sscanf(subcadena,"max_dash_angle %d",&int_value);
            game_data->game_parameter.server_param.max_dash_angle=int_value;}
        subcadena=strstr(message,"max_dash_power");
        if (subcadena){
            sscanf(subcadena,"max_dash_angle %d",&int_value);
            game_data->game_parameter.server_param.max_dash_power=int_value;}
        subcadena=strstr(message,"max_goal_kicks");
        if (subcadena){
            sscanf(subcadena,"max_goal_kicks %d",&int_value);
            game_data->game_parameter.server_param.max_goal_kicks=int_value;}
        subcadena=strstr(message,"max_monitors");
        /*if (subcadena){
            sscanf(subcadena,"max_monitors %d",&int_value);
            game_data->game_parameter.server_param.max_monitors=int_value;}
        */
        subcadena=strstr(message,"max_tackle_power");
        if (subcadena){
            sscanf(subcadena,"max_tackle_power %d",&int_value);
            game_data->game_parameter.server_param.max_tackle_power=int_value;}
        subcadena=strstr(message,"maxmoment");
        if (subcadena){
            sscanf(subcadena,"maxmoment %d",&int_value);
            game_data->game_parameter.server_param.maxmoment=int_value;}
        subcadena=strstr(message,"maxneckang");
        if (subcadena){
            sscanf(subcadena,"maxneckang %d",&int_value);
            game_data->game_parameter.server_param.maxneckang=int_value;}
        subcadena=strstr(message,"maxneckmoment");
        if (subcadena){
            sscanf(subcadena,"maxneckmoment %d",&int_value);
            game_data->game_parameter.server_param.maxneckmoment=int_value;}
        subcadena=strstr(message,"maxpower");
        if (subcadena){
            sscanf(subcadena,"maxpower %d",&int_value);
            game_data->game_parameter.server_param.maxpower=int_value;}
        subcadena=strstr(message,"min_dash_angle");
        if (subcadena){
            sscanf(subcadena,"min_dash_angle %d",&int_value);
            game_data->game_parameter.server_param.min_dash_angle=int_value;}
        subcadena=strstr(message,"min_dash_power");
        if (subcadena){
            sscanf(subcadena,"min_dash_power %d",&int_value);
            game_data->game_parameter.server_param.min_dash_power=int_value;}
        subcadena=strstr(message,"minmoment");
        if (subcadena){
            sscanf(subcadena,"minmoment %d",&int_value);
            game_data->game_parameter.server_param.minmoment=int_value;}
        subcadena=strstr(message,"minneckang");
        if (subcadena){
            sscanf(subcadena,"minneckang %d",&int_value);
            game_data->game_parameter.server_param.minneckang=int_value;}
        subcadena=strstr(message,"minneckmoment");
        if (subcadena){
            sscanf(subcadena,"minneckmoment %d",&int_value);
            game_data->game_parameter.server_param.minneckmoment=int_value;}
        subcadena=strstr(message,"minpower");
        if (subcadena){
            sscanf(subcadena,"minpower %d",&int_value);
            game_data->game_parameter.server_param.minpower=int_value;}
        subcadena=strstr(message,"nr_extra_halfs");
        if (subcadena){
            sscanf(subcadena,"nr_extra_halfs %d",&int_value);
            game_data->game_parameter.server_param.nr_extra_halfs=int_value;}
        subcadena=strstr(message,"nr_normal_halfs");
        if (subcadena){
            sscanf(subcadena,"nr_normal_halfs %d",&int_value);
            game_data->game_parameter.server_param.nr_normal_halfs=int_value;}
        subcadena=strstr(message,"offside_active_area_size");
        if (subcadena){
            sscanf(subcadena,"offside_active_area_size %lf",&init_value);
            game_data->game_parameter.server_param.offside_active_area_size=init_value;}
        subcadena=strstr(message,"offside_kick_margin");
        if (subcadena){
            sscanf(subcadena,"offside_kick_margin %lf",&init_value);
            game_data->game_parameter.server_param.offside_kick_margin=init_value;}
        subcadena=strstr(message,"olcoach_port");
        if (subcadena){
            sscanf(subcadena,"olcoach_port %d",&int_value);
            game_data->game_parameter.server_param.olcoach_port=int_value;}
        subcadena=strstr(message,"old_coach_hear");
        if (subcadena){
            sscanf(subcadena,"old_coach_hear %d", &int_value);
            game_data->game_parameter.server_param.old_coach_hear=int_value;}
        subcadena=strstr(message,"pen_allow_mult_kicks");
        if (subcadena){
            sscanf(subcadena,"pen_allow_mult_kicks %d",&int_value);
            game_data->game_parameter.server_param.pen_allow_mult_kicks=int_value;}
        subcadena=strstr(message,"pen_before_setup_wait");
        if (subcadena){
            sscanf(subcadena,"pen_before_setup_wait %d",&int_value);
            game_data->game_parameter.server_param.pen_before_setup_wait=int_value;}
        subcadena=strstr(message,"pen_coach_moves_players");
        if (subcadena){
            sscanf(subcadena,"pen_coach_moves_players %d",&int_value);
            game_data->game_parameter.server_param.pen_coach_moves_players=int_value;}
        subcadena=strstr(message,"pen_dist_x");
        if (subcadena){
            sscanf(subcadena,"pen_dist_x %d", &int_value);
            game_data->game_parameter.server_param.pen_dist_x=int_value;}
        subcadena=strstr(message,"pen_max_extra_kicks");
        if (subcadena){
            sscanf(subcadena,"pen_max_extra_kicks %d", &int_value);
            game_data->game_parameter.server_param.pen_max_extra_kicks=int_value;}
        subcadena=strstr(message,"pen_max_goalie_dist_x");
        if (subcadena){
            sscanf(subcadena,"pen_max_goalie_dist_x %d", &int_value);
            game_data->game_parameter.server_param.pen_max_goalie_dist_x=int_value;}
        subcadena=strstr(message,"pen_nr_kicks");
        if (subcadena){
            sscanf(subcadena,"pen_nr_kicks %d",&int_value);
            game_data->game_parameter.server_param.pen_nr_kicks=int_value;}
        subcadena=strstr(message,"pen_random_winner");
        if (subcadena){
            sscanf(subcadena,"pen_random_winner %d",&int_value);
            game_data->game_parameter.server_param.pen_random_winner=int_value;}
        subcadena=strstr(message,"pen_ready_wait");
        if (subcadena){
            sscanf(subcadena,"pen_ready_wait %d",&int_value);
            game_data->game_parameter.server_param.pen_ready_wait=int_value;}
        subcadena=strstr(message,"pen_setup_wait");
        if (subcadena){
            sscanf(subcadena,"pen_setup_wait %d",&int_value);
            game_data->game_parameter.server_param.pen_setup_wait=int_value;}
        subcadena=strstr(message,"pen_taken_wait");
        if (subcadena){
            sscanf(subcadena,"pen_taken_wait %d",&int_value);
            game_data->game_parameter.server_param.pen_taken_wait=int_value;}
        subcadena=strstr(message,"penalty_shoot_outs");
        if (subcadena){
            sscanf(subcadena,"penalty_shoot_outs %d",&int_value);
            game_data->game_parameter.server_param.penalty_shoot_outs=int_value;}
        subcadena=strstr(message,"player_accel_max");
        if (subcadena){
            sscanf(subcadena,"player_accel_max %d",&int_value);
            game_data->game_parameter.server_param.player_accel_max=int_value;}
        subcadena=strstr(message,"player_decay");
        if (subcadena){
            sscanf(subcadena,"player_decay %lf",&init_value);
            game_data->game_parameter.server_param.player_decay=init_value;}
        subcadena=strstr(message,"player_rand");
        if (subcadena){
            sscanf(subcadena,"player_rand %lf",&init_value);
            game_data->game_parameter.server_param.player_rand=init_value;}
        subcadena=strstr(message,"player_size");
        if (subcadena){
            sscanf(subcadena,"player_size %lf",&init_value);
            game_data->game_parameter.server_param.player_size=init_value;}
        subcadena=strstr(message,"player_speed_max");
        if (subcadena){
            sscanf(subcadena,"player_speed_max %lf",&init_value);
            game_data->game_parameter.server_param.player_speed_max=init_value;}
        subcadena=strstr(message,"player_speed_max_min");
        if (subcadena){
            sscanf(subcadena,"player_speed_max_min %lf",&init_value);
            game_data->game_parameter.server_param.player_speed_max_min=init_value;}
        subcadena=strstr(message,"player_weight");
        if (subcadena){
            sscanf(subcadena,"player_weight %d",&int_value);
            game_data->game_parameter.server_param.player_weight=int_value;}
        subcadena=strstr(message,"point_to_ban");
        if (subcadena){
            sscanf(subcadena,"point_to_ban %d",&int_value);
            game_data->game_parameter.server_param.point_to_ban=int_value;}
        subcadena=strstr(message,"point_to_duration");
        if (subcadena){
            sscanf(subcadena,"point_to_duration %d",&int_value);
            game_data->game_parameter.server_param.point_to_duration=int_value;}
        subcadena=strstr(message,"port");
        if (subcadena){
            sscanf(subcadena,"port %d",&int_value);
            game_data->game_parameter.server_param.port=int_value;}
        subcadena=strstr(message,"prand_factor_l");
        if (subcadena){
            sscanf(subcadena,"prand_factor_l %d",&int_value);
            game_data->game_parameter.server_param.prand_factor_l=int_value;}
        subcadena=strstr(message,"prand_factor_r");
        if (subcadena){
            sscanf(subcadena,"prand_factor_r %d",&int_value);
            game_data->game_parameter.server_param.prand_factor_r=int_value;}
        subcadena=strstr(message,"profile");
        if (subcadena){
            sscanf(subcadena,"profile %d",&int_value);
            game_data->game_parameter.server_param.profile=int_value;}
        subcadena=strstr(message,"proper_goal_kicks");
        if (subcadena){
            sscanf(subcadena,"proper_goal_kicks %d",&int_value);
            game_data->game_parameter.server_param.proper_goal_kicks=int_value;}
        subcadena=strstr(message,"quantize_step_l");
        if (subcadena){
            sscanf(subcadena,"quantize_step_l %lf )", &init_value);
            game_data->game_parameter.server_param.quantize_step_l = init_value;

        }
        subcadena=strstr(message,"quantize_step");
        if (subcadena){
            sscanf(subcadena,"quantize_step %lf",&init_value);
            game_data->game_parameter.server_param.quantize_step=init_value;

        }
        subcadena=strstr(message,"record_messages");
        if (subcadena){
            sscanf(subcadena,"record_messages %d",&int_value);
            game_data->game_parameter.server_param.record_messages=int_value;}
        subcadena=strstr(message,"recover_dec");
        if (subcadena){
            sscanf(subcadena,"recover_dec %lf",&init_value);
            game_data->game_parameter.server_param.recover_dec=init_value;}
        subcadena=strstr(message,"recover_dec_thr");
        if (subcadena){
            sscanf(subcadena,"recover_dec_thr %lf",&init_value);
            game_data->game_parameter.server_param.recover_dec_thr=init_value;}
        subcadena=strstr(message,"recover_init");
        if (subcadena){
            sscanf(subcadena,"recover_init %d",&int_value);
            game_data->game_parameter.server_param.recover_init=int_value;}
        subcadena=strstr(message,"recover_min");
        if (subcadena){
            sscanf(subcadena,"recover_min %lf",&init_value);
            game_data->game_parameter.server_param.recover_min=init_value;}
        subcadena=strstr(message,"recv_step");
        if (subcadena){
            sscanf(subcadena,"recv_step %d",&int_value);
            game_data->game_parameter.server_param.recv_step=int_value;}
        subcadena=strstr(message,"red_card_probability");
        if (subcadena){
            sscanf(subcadena,"red_card_probability %d",&int_value);
            game_data->game_parameter.server_param.red_card_probability=int_value;}
        subcadena=strstr(message,"say_coach_cnt_max");
        if (subcadena){
            sscanf(subcadena,"say_coach_cnt_max %d",&int_value);
            game_data->game_parameter.server_param.say_coach_cnt_max=int_value;}
        subcadena=strstr(message,"say_coach_msg_size");
        if (subcadena){
            sscanf(subcadena,"say_coach_msg_size %d",&int_value);
            game_data->game_parameter.server_param.say_coach_msg_size = int_value;}
        subcadena=strstr(message,"say_msg_size");
        if (subcadena){
            sscanf(subcadena,"say_msg_size %d",&int_value);
            game_data->game_parameter.server_param.say_msg_size=int_value;}
        subcadena=strstr(message,"send_comms");
        if (subcadena){
            sscanf(subcadena,"send_comms %d",&int_value);
            game_data->game_parameter.server_param.send_comms=int_value;}
        subcadena=strstr(message,"send_step");
        if (subcadena){
            sscanf(subcadena,"send_step %d",&int_value);
            game_data->game_parameter.server_param.send_step=int_value;}
        subcadena=strstr(message,"send_vi_step");
        if (subcadena){
            sscanf(subcadena,"send_vi_step %d",&int_value);
            game_data->game_parameter.server_param.send_vi_step=int_value;}
        subcadena=strstr(message,"sense_body_step");
        if (subcadena){
            sscanf(subcadena,"sense_body_step %d",&int_value);
            game_data->game_parameter.server_param.sense_body_step=int_value;}
        subcadena=strstr(message,"side_dash_rate");
        if (subcadena){
            sscanf(subcadena,"side_dash_rate %d",&int_value);
            game_data->game_parameter.server_param.side_dash_rate=int_value;}
        subcadena=strstr(message,"simulator_step");
        if (subcadena){
            sscanf(subcadena,"simulator_step %d",&int_value);
            game_data->game_parameter.server_param.simulator_step=int_value;}
        subcadena=strstr(message,"slow_down_factor");
        if (subcadena){
            sscanf(subcadena,"slow_down_factor %d",&int_value);
            game_data->game_parameter.server_param.slow_down_factor=int_value;}
        subcadena=strstr(message,"slowness_on_top_for_left_team");
        if (subcadena){
            sscanf(subcadena,"slowness_on_top_for_left_team %d",&int_value);
            game_data->game_parameter.server_param.slowness_on_top_for_left_team=int_value;}
        subcadena=strstr(message,"slowness_on_top_for_right_team");
        if (subcadena){
            sscanf(subcadena,"slowness_on_top_for_right_team %d",&int_value);
            game_data->game_parameter.server_param.slowness_on_top_for_right_team=int_value;}
        subcadena=strstr(message,"stamina_capacity");
        if (subcadena){
            sscanf(subcadena,"stamina_capacity %d",&int_value);
            game_data->game_parameter.server_param.stamina_capacity=int_value;}

        subcadena=strstr(message,"stamina_inc_max");
        if (subcadena){
            sscanf(subcadena,"stamina_inc_max %d",&int_value);
            game_data->game_parameter.server_param.stamina_inc_max = int_value;}

        subcadena=strstr(message,"stamina_max");
        if (subcadena){
            sscanf(subcadena,"stamina_max %d",&int_value);
            game_data->game_parameter.server_param.stamina_max=int_value;}
        subcadena=strstr(message,"start_goal_l");
        if (subcadena){
            sscanf(subcadena,"start_goal_l %d",&int_value);
            game_data->game_parameter.server_param.start_goal_l=int_value;}
        subcadena=strstr(message,"start_goal_r");
        if (subcadena){
            sscanf(subcadena,"start_goal_r %d",&int_value);
            game_data->game_parameter.server_param.start_goal_r=int_value;}
        subcadena=strstr(message,"stopped_ball_vel");
        if (subcadena){
            sscanf(subcadena,"stopped_ball_vel %d",&int_value);
            game_data->game_parameter.server_param.stopped_ball_vel=int_value;}
        subcadena=strstr(message,"synch_micro_sleep");
        if (subcadena){
            sscanf(subcadena,"synch_micro_sleep %lf",&init_value);
            game_data->game_parameter.server_param.synch_micro_sleep=init_value;}
        subcadena=strstr(message,"synch_mode");
        if (subcadena){
            sscanf(subcadena,"synch_mode %d",&int_value);
            game_data->game_parameter.server_param.synch_mode=int_value;}
        subcadena=strstr(message,"synch_offset");
        if (subcadena){
            sscanf(subcadena,"synch_offset %d",&int_value);
            game_data->game_parameter.server_param.synch_offset=int_value;}
        subcadena=strstr(message,"synch_see_offset");
        if (subcadena){
            sscanf(subcadena,"synch_see_offset %d",&int_value);
            game_data->game_parameter.server_param.synch_see_offset=int_value;}
        subcadena=strstr(message,"tackle_back_dist");
        if (subcadena){
            sscanf(subcadena,"tackle_back_dist %d",&int_value);
            game_data->game_parameter.server_param.tackle_back_dist=int_value;}
        subcadena=strstr(message,"tackle_exponent");

        subcadena=strstr(message,"tackle_cycles");
        if (subcadena){
            sscanf(subcadena,"tackle_cycles %d",&int_value);
            game_data->game_parameter.server_param.tackle_cycles = int_value;}
        subcadena=strstr(message,"tackle_exponent");

        subcadena=strstr(message,"tackle_dist");
        if (subcadena){
            sscanf(subcadena,"tackle_dist %d",&int_value);
            game_data->game_parameter.server_param.tackle_dist = int_value;}
        subcadena=strstr(message,"tackle_exponent");

        if (subcadena){
            sscanf(subcadena,"tackle_exponent %d",&int_value);
            game_data->game_parameter.server_param.tackle_exponent=int_value;}
        subcadena=strstr(message,"tackle_power_rate");
        if (subcadena){
            sscanf(subcadena,"tackle_power_rate %lf",&init_value);
            game_data->game_parameter.server_param.tackle_power_rate=init_value;}
        subcadena=strstr(message,"tackle_rand_factor");
        if (subcadena){
            sscanf(subcadena,"tackle_rand_factor %d",&int_value);
            game_data->game_parameter.server_param.tackle_rand_factor=int_value;}
        subcadena=strstr(message,"tackle_width");
        if (subcadena){
            sscanf(subcadena,"tackle_width %lf",&init_value);
            game_data->game_parameter.server_param.tackle_width=init_value;}
        subcadena=strstr(message,"team_actuator_noise");
        if (subcadena){
            sscanf(subcadena,"team_actuator_noise %d",&int_value);
            game_data->game_parameter.server_param.team_actuator_noise=int_value;}
        subcadena=strstr(message,"team_l_start");
        if (subcadena){
            sscanf(subcadena,"team_l_start %s", char_s_value );
                   game_data->game_parameter.server_param.team_l_start=char_s_value ;}
        subcadena=strstr(message,"team_r_start");
        if (subcadena){
            sscanf(subcadena,"team_r_start %d",&int_value);
            game_data->game_parameter.server_param.team_r_start=int_value;}
        subcadena=strstr(message,"text_log_compression");
                   if (subcadena){
            sscanf(subcadena,"text_log_compression %d",&int_value);
                       game_data->game_parameter.server_param.text_log_compression=int_value;}
        subcadena=strstr(message,"text_log_dated");
                   if (subcadena){
            sscanf(subcadena,"text_log_dated %d",&int_value);
                       game_data->game_parameter.server_param.text_log_dated=int_value;}
        subcadena=strstr(message,"text_log_dir");
                   if (subcadena){
            sscanf(subcadena,"text_log_dir %s", char_s_value );
                       game_data->game_parameter.server_param.text_log_dir=char_s_value;}
        subcadena=strstr(message,"text_log_fixed");
                   if (subcadena){
            sscanf(subcadena,"text_log_fixed %d",&int_value);
                       game_data->game_parameter.server_param.text_log_fixed=int_value;}
        subcadena=strstr(message,"text_log_fixed_name");
                   if (subcadena){
            sscanf(subcadena,"text_log_fixed_name %s", char_s_value );
                       game_data->game_parameter.server_param.text_log_fixed_name=char_s_value;}
        subcadena=strstr(message,"text_logging");
                   if (subcadena){
            sscanf(subcadena,"text_logging %d",&int_value);
                       game_data->game_parameter.server_param.text_logging=int_value;}
        subcadena=strstr(message,"use_offside");
                   if (subcadena){
            sscanf(subcadena,"use_offside %d",&int_value);
                       game_data->game_parameter.server_param.use_offside=int_value;}
        subcadena=strstr(message,"verbose");
                   if (subcadena){
            sscanf(subcadena,"verbose %d",&int_value);
                       game_data->game_parameter.server_param.verbose=int_value;}
        subcadena=strstr(message,"visible_angle");
                   if (subcadena){
            sscanf(subcadena,"visible_angle %d",&int_value);
                       game_data->game_parameter.server_param.visible_angle=int_value;}
        subcadena=strstr(message,"visible_distance");
                   if (subcadena){
            sscanf(subcadena,"visible_distance %d",&int_value);
                       game_data->game_parameter.server_param.visible_distance=int_value;}
        subcadena=strstr(message,"wind_ang");
                   if (subcadena){
            sscanf(subcadena,"wind_ang %d",&int_value);
                       game_data->game_parameter.server_param.wind_ang=int_value;}
        subcadena=strstr(message,"wind_dir");
                   if (subcadena){
            sscanf(subcadena,"wind_dir %d",&int_value);
                       game_data->game_parameter.server_param.wind_dir=int_value;}
        subcadena=strstr(message,"wind_force");
                   if (subcadena){
            sscanf(subcadena,"wind_foce %d",&int_value);
                       game_data->game_parameter.server_param.wind_force=int_value;}
        subcadena=strstr(message,"wind_none");
                   if (subcadena){
            sscanf(subcadena,"wind_none %d",&int_value);
                       game_data->game_parameter.server_param.wind_none=int_value;}
        subcadena=strstr(message,"wind_rand");
                   if (subcadena){
            sscanf(subcadena,"wind_rand %d",&int_value);
                       game_data->game_parameter.server_param.wind_rand=int_value;}
        subcadena=strstr(message,"wind_random");
                   if (subcadena){
            sscanf(subcadena,"wind_random %d",&int_value);
                       game_data->game_parameter.server_param.wind_random=int_value;}
    //game_data->game_parameter.server_param.print();
    free( char_s_value );
}

int Parser::spaces_bef_parenthesis( char * message )
{
    int count = 0;
    while( *message != ')')
    {
        if( *message == ' ')
            count++;
        message++;
    }
    return count;
}


void Parser::flpar( char *subcadena2, EFlag c){
                    int int_value4;
                    double flo_value, flo_value2, flo_value1;
                    char *subcadena=NULL;
                    char *subcadena3=NULL;
                    int j,k,l,g,count;
                    count = 0;
                    using namespace std;
                    subcadena=strstr(subcadena2,")");


                    if (subcadena){
                        j=strlen(subcadena);
                        subcadena3=strstr(subcadena+1,")");
                        l=strlen(subcadena3);
                        k=j-l;
                        for (g=0;g<=k;g++){
                            if(isspace(*subcadena)){
                                count=count+1;
                            }
                            subcadena++;
                        }

                        if(count==4){
                            subcadena=strstr(subcadena2,")");
                            sscanf(subcadena+1," %lf %d %lf %lf",&flo_value,&int_value4,&flo_value1,&flo_value2);
                            game_data->sensor_handler.see_flag( c,
                                                             flo_value,
                                                             int_value4,
                                                             flo_value1,
                                                             flo_value2);


                        }
                        if(count==3){
                            subcadena=strstr(subcadena2,")");
                            sscanf(subcadena+1," %lf %d %lf",&flo_value,&int_value4,&flo_value1);

                            // No sabemos aún cuáles son los tres valores que recibimos
                            //game_data->sensor_handler.see_flag(c,flo_value,int_value5,flo_value1);
                            // printf("id: %d  ", c); //#gil_mark
                        }

                        if(count==2){
                            subcadena=strstr(subcadena2,")");
                            sscanf(subcadena+1," %lf %d",&flo_value,&int_value4);
                            game_data->sensor_handler.see_flag(c,flo_value,int_value4);
                        }
                        if(count==1){
                            subcadena=strstr(subcadena2,")");
                            sscanf(subcadena+1," %lf",&flo_value);

                            // No sabemos aún en qué caso recibimos un solo valor y cuál es.
                            //game_data->sensor_handler.see_flag(c,flo_value);
                            //printf("id: %d  ", c); //#gil_mark
                        }

                    }

                }
void Parser::lipar(char *subcadena2, ELine c)
{
        int int_value4;
        double flo_value, flo_value2, flo_value1;
        char *subcadena=NULL;
        char *subcadena3=NULL;
        int j,k,l,g,count;
        count = 0;
        using namespace std;
        subcadena=strstr(subcadena2,")");

        if (subcadena){
            j=strlen(subcadena);
            subcadena3=strstr(subcadena+1,")");
            l=strlen(subcadena3);
            k=j-l;
            for (g=0;g<=k;g++){
                if(isspace(*subcadena)){
                    count=count+1;
                }
                subcadena++;
            }

            if(count==4){
                subcadena=strstr(subcadena2,")");
                sscanf(subcadena+1," %lf %d %lf %lf",&flo_value,&int_value4,&flo_value1,&flo_value2);
                game_data->sensor_handler.see_line(c,flo_value,int_value4,flo_value1,flo_value2);

            }
            if(count==3){
                subcadena=strstr(subcadena2,")");
                sscanf(subcadena+1," %lf %d %lf",&flo_value,&int_value4,&flo_value1);
                //game_data->sensor_handler.see_line(c,flo_value,int_value4,flo_value1);
            }

            if(count==2){
                subcadena=strstr(subcadena2,")");
                sscanf(subcadena+1," %lf %d",&flo_value,&int_value4);
                game_data->sensor_handler.see_line(c,flo_value,int_value4);
            }
            if(count==1){
                subcadena=strstr(subcadena2,")");
                sscanf(subcadena+1," %lf",&flo_value);
                //game_data->sensor_handler.see_line(c,flo_value);
            }

        }

}

PlayModeHearable Parser::parse_play_mode(char *char_play_mode, int & num )
{
    /* Entrada: Una cadena de caracteres cuya primer palabra es algún modo de juego y una variable entera
       Salida: El tipo de modo de juego, un enum PLayMode; devuelve -1 en caso de no corresponder a ninguno

       Algunos modos de juego tienen incrustado un numero, en caso de haberlo,
       se guardará ese número en la variable num. Hasta ahora solo se han identificado algunos de todos los modos
       de juego que presentan esta característica
    */

    //int aux_num; //en esta variable se guarda el unum o numero de goles dependiendo del playmode, este módulo aún no se implementa
    num = UNDEFINED_NUMBER;
    if(strcmp(char_play_mode,"time_over")==0)
        return TIME_OVER;
    else if (strcmp(char_play_mode,"goalie_catch_ball_l")==0)
        return GOALIE_CATCH_BALL_L;
    else if (strcmp(char_play_mode,"goalie_catch_ball_r")==0)
        return GOALIE_CATCH_BALL_R;
    else if (strcmp(char_play_mode,"before_kick_off")==0)
    {
        return BEFORE_KICK_OFF;
    }
    else if (strcmp(char_play_mode, "play_on")==0) {
        return PLAY_ON;
    }
    else if (strcmp(char_play_mode, "kick_off_l")==0) {
        return KICK_OFF_L;
    }
    else if (strcmp(char_play_mode,"kick_off_r")==0) {
        return KICK_OFF_R;
    }
    else if (strcmp(char_play_mode, "kick_in_l")==0) {
        return KICK_IN_L;
    }
    else if (strcmp(char_play_mode, "kick_in_r")==0) {
        return KICK_IN_R;
    }
    else if (strcmp(char_play_mode,"free_kick_l")==0) {
        return FREE_KICK_L;
    }
    else if (strcmp(char_play_mode, "free_kick_r")==0) {
        return FREE_KICK_R;
    }
    else if (strcmp(char_play_mode, "corner_kick_l")==0) {
        return CORNER_KICK_L;
    }
    else if (strcmp(char_play_mode, "corner_kick_r")==0) {
        return CORNER_KICK_R;
    }
    else if (strcmp(char_play_mode, "goal_kick_l")==0) {
        return GOAL_KICK_L;
    }
    else if (strcmp(char_play_mode, "goal_kick_r")==0) {
        return GOAL_KICK_R;
    }
    else if (strcmp(char_play_mode, "drop_ball")==0) {
        return DROP_BALL;
    }
    else if (strcmp(char_play_mode,"offside_l" )==0) {
        return OFFSIDE_L;
    }
    else if (strcmp(char_play_mode, "offside_r")==0) {
        return OFFSIDE_R;
    }
    else if (strcmp(char_play_mode, "penalty_kick_l")==0) {
        return PENALTY_KICK_L;
    }
    else if (strcmp(char_play_mode, "penalty_kick_r")==0) {
        return PENALTY_KICK_R;
    }
    else if (strcmp(char_play_mode, "first_half_over")==0) {
        return FIRST_HALF_OVER;
    }
    else if (strcmp(char_play_mode, "pause")==0) {
        return PAUSE;
    }
    else if (strcmp(char_play_mode, "human_judge")==0) {
        return HUMAN_JUDGE;
    }
    else if (strcmp(char_play_mode, "foul_l")==0) {
        return FOUL_L;
    }
    else if (strcmp(char_play_mode, "foul_r")==0) {
        return FOUL_R;
    }
    else if (strcmp(char_play_mode, "foul_charge_l")==0) {
        return FOUL_CHARGE_L;
    }
    else if (strcmp(char_play_mode, "foul_charge_r")==0) {
        return FOUL_CHARGE_R;
    }
    else if (strcmp(char_play_mode, "foul_push_l")==0) {
        return FOUL_PUSH_L;
    }
    else if (strcmp(char_play_mode,"foul_push_r" )==0) {
        return FOUL_PUSH_R;
    }
    else if (strcmp(char_play_mode, "foul_multiple_attack_l")==0) {
        return FOUL_MULTIPLE_ATTACK_L;
    }
    else if (strcmp(char_play_mode, "foul_multiple_attack_r")==0) {
        return FOUL_MULTIPLE_ATTACK_R;
    }
    else if (strcmp(char_play_mode, "foul_ballout_l")==0) {
        return FOUL_BALLOUT_L;
    }
    else if (strcmp(char_play_mode, "foul_ballout_r")==0) {
        return FOUL_BALLOUT_R;
    }
    else if (strcmp(char_play_mode, "back_pass_l")==0) {
        return BACK_PASS_L;
    }
    else if (strcmp(char_play_mode, "back_pass_r")==0) {
        return BACK_PASS_R;
    }
    else if (strcmp(char_play_mode, "free_kick_fault_l")==0) {
        return FREE_KICK_FAULT_L;
    }
    else if (strcmp(char_play_mode, "free_kick_fault_r")==0) {
        return FREE_KICK_FAULT_R;
    }
    else if (strcmp(char_play_mode, "catch_fault_l")==0) {
        return CATCH_FAULT_L;
    }
    else if (strcmp(char_play_mode,"indirect_free_kick_l")==0) {
        return INDIRECT_FREE_KICK_L;
    }
    else if (strcmp(char_play_mode,"indirect_free_kick_r")==0) {
        return INDIRECT_FREE_KICK_R;
    }
    else if (strcmp(char_play_mode,"penalty_seup_l")==0) {
        return PENALTY_SETUP_L;
    }
    else if (strcmp(char_play_mode,"penalty_setup_r")==0) {
        return PENALTY_SETUP_R;
    }
    else if (strcmp(char_play_mode,"penalty_ready_l" )==0) {
        return PENALTY_READY_L;
    }
    else if (strcmp(char_play_mode, "penalty_ready_r")==0) {
        return PENALTY_READY_R;
    }
    else if (strcmp(char_play_mode, "penalty_taken_l")==0) {
        return PENALTY_TAKEN_L;
    }
    else if (strcmp(char_play_mode, "penalty_taken_r")==0) {
        return PENALTY_TAKEN_R;
    }
    else if (strcmp(char_play_mode, "penalty_miss_l")==0) {
        return PENALTY_MISS_L;
    }
    else if (strcmp(char_play_mode, "penalty_miss_r")==0) {
        return PENALTY_MISS_R;
    }
    else if (strcmp(char_play_mode, "penalty_score_l")==0) {
        return PENALTY_SCORE_L;
    }
    else if (strcmp(char_play_mode, "penalty_score_r")==0) {
        return PENALTY_SCORE_R;
    }
    else if (strcmp(char_play_mode, "penalty_onfield_l")==0) {
        return PENALTY_ONFIELD_L;
    }
    else if (strcmp(char_play_mode, "penalty_onfield_r")==0) {
        return PENALTY_ONFIELD_R;
    }
    else if (strcmp(char_play_mode, "penalty_foul_l")==0) {
        return PENALTY_FOUL_L;
    }
    else if (strcmp(char_play_mode, "penalty_foul_r")==0) {
        return PENALTY_FOUL_R;
    }
    else if (strcmp(char_play_mode, "penalty_winner_l")==0) {
        return PENALTY_WINNER_L;
    }
    else if (strcmp(char_play_mode, "penalty_winner_r")==0) {
        return PENALTY_WINNER_R;
    }
    else if (strcmp(char_play_mode, "penalty_draw")==0)
        return PENALTY_DRAW;
    //Aquí inician los que tienen incrustado algún número.
    /*y aqui lacagamos en algo
    else

        if( strstr(char_play_mode, "goal_l_") )
        {
            char_play_mode = strstr(char_play_mode, "goal_l_");
            sscanf(char_play_mode, "goal_l_%d", &aux_num );
            num = aux_num;
            return GOAL_L;
        }
        else if( strstr( char_play_mode, "goal_r_"))
        {
            char_play_mode = strstr(char_play_mode, "goal_l_");
            sscanf(char_play_mode, "goal_r_%d", &aux_num );
            num = aux_num;
            return GOAL_R;
        }
        else if( strstr( char_play_mode, "yellow_card_l_"))
        {
            char_play_mode = strstr(char_play_mode, "yellow_card_l_");
            sscanf(char_play_mode, "yellow_card_l_%d", &aux_num );
            num = aux_num;
            return YELLOW_CARD_L;
        }
        else if( strstr( char_play_mode, "yellow_card_r"))
        {
            char_play_mode = strstr(char_play_mode, "yellow_card_r");
            sscanf(char_play_mode, "yellow_card_r%d", &aux_num );
            num = aux_num;
            return YELLOW_CARD_R;
        }
        else if( strstr( char_play_mode, "red_card_l"))
        {
            char_play_mode = strstr(char_play_mode, "red_card_l");
            sscanf(char_play_mode, "red_card_l%d", &aux_num );
            num = aux_num;
            return RED_CARD_L;
        }
        else if( strstr( char_play_mode, "red_card_r"))
        {
            char_play_mode = strstr(char_play_mode, "red_card_r");
            sscanf(char_play_mode, "red_card_r%d", &aux_num );
            num = aux_num;
            return RED_CARD_R;
        }
        */
    else
            return (PlayModeHearable)0; //Regresamos cero en caso de algún error
}

void Parser::parse_see_global( char * message )
{
			double x, y, vx, vy, body_angle, neck_angle;
			char team[56]; // Pendiende te verificar cuál es la longitud máxima del equipo.
			int unum, n_readed, time;
			
			message += 12; // Omitimos "(see_global "
			
			// Datos del tiempo y la portería derecha
			sscanf( message, "%d ( ( g r ) %lf %lf ) %n", &time, &x, &y, &n_readed );
			message += n_readed;
			
            game_data->sensor_handler.see_global_begin( time );
			
			// Datos de la portería izquierda
			sscanf( message, "( ( g l ) %lf %lf ) %n", &x, &y, &n_readed );
			message += n_readed;
			
			// Datos del balón
			sscanf( message, "( (b) %lf %lf %lf %lf ) %n", &x, &y, &vx, &vy, &n_readed );
			message += n_readed;
			
            game_data->sensor_handler.see_global_ball( x, y, vx, vy );
			
			// Datos de todos los jugadores
			while( *message != ')' )
			{
				sscanf( message, "( (p \"%[-0-9a-zA-Z ().+*/?<>_]\" %d ) %lf %lf %lf %lf %lf %lf ) %n", team, &unum, &x, &y, &vx, &vy, &body_angle, &neck_angle, &n_readed );
				message += n_readed;
				
                game_data->sensor_handler.see_global_player( team, unum, x, y, vx, vy, body_angle, neck_angle );
			}
}

char * Parser::move_to_next_word(char * str)
{
    /*
      Entrada: cualquier cadena
      Salida: un puntero hacia el caracter situado después del espacio siguiente en la cadena.
             En caso de no haber más espacios, regresa NULL
      */
    char * return_pointer;
    int size;
    int i;
    int found = 0;
    if( str == NULL ) return NULL;
    return_pointer = str;
    size = strlen( str );
    for( i=0; i<size && found == 0;  i++)
    {
        if( isspace(*return_pointer) )
            found = 1;
        else
            return_pointer++;
    }
    if( found )

        return ++return_pointer;
    else
        return NULL;
}
