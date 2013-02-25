#ifndef PARSER_H
#define PARSER_H

#include "gameData.h"
#include "microParser.h"
#include "sensorHandler.h"

enum ParserMsgType
{
    P_PARSING_ERROR,
    P_SERVER_PARAM,
    P_PLAYER_PARAM,
    P_PLAYER_TYPE,
    P_SEE,
    P_SEE_GLOBAL,
    P_SENSE,
    P_HEAR,
    P_MSG,
    P_INIT,
    P_OK,
    P_ERROR
};

class Parser
{
public:
    Parser( GameData * game_data );
    ParserMsgType parse( char * message );
private:
    GameData    * game_data;

	// Parseamos cada tipo de mensaje.
    void parse_server_param  ( char * message );
    void parse_player_param  ( char * message );
    void parse_player_type   ( char * message );
    void parse_see           ( char * message );
    void parse_see_global	 ( char * message );
    void parse_sense         ( char * message );
    void parse_hear          ( char * message );
    void parse_msg           ( char * message );
    void parse_init          ( char * message );
    void parse_ok            ( char * message );
    void parse_error         ( char * message );
    
	// Los siguientes no son tipos de mensaje, sino utilidades
    void parse_flag          ( char * sub_msg, EFlag id, const char format[] );
    void parse_line          ( char * sub_msg, ELine id, const char format[] );
    PlayModeHearable parse_play_mode ( char * char_play_mode, int & num );

    void flpar              (char *subcadena2, EFlag c);
    void lipar              ( char *subcadena2, ELine c);
    int spaces_bef_parenthesis( char * message);
    char * move_to_next_word( char *str );

};

#endif

