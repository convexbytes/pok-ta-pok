#include "microParser.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <vector>
using namespace std;

MP_MessageType MicroParser::get_message_type(char * message)
{
	char *tipo;
    tipo = strstr(message, "init");
	if(tipo)
        return MP_INIT;

    tipo = strstr(message, "server_param");
	if(tipo)
        return MP_SERVER_PARAM;

    tipo = strstr(message, "player_param");
	if(tipo)
        return MP_PLAYER_PARAM;

    tipo = strstr(message, "player_type");
	if(tipo)
        return MP_PLAYER_TYPE;

    tipo = strstr(message, "msg");
	if(tipo)
        return MP_MSG;

    tipo = strstr(message, "hear");
	if(tipo)
        return MP_HEAR;

    tipo = strstr(message, "sense_body");
	if(tipo)
        return MP_SENSE_BODY;

    tipo = strstr(message, "see_global");
    if( tipo )
        return MP_SEE_GLOBAL;

    tipo = strstr(message, "see");
	if(tipo)
        return MP_SEE;

	tipo = strstr(message, "ok");
    if( tipo )
		return MP_OK;

    std::cout << message << std::endl;

    return MP_NONE;
}


