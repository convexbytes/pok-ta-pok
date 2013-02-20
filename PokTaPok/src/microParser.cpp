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

bool MicroParser::parenthesis_are_matched(char* message){
	//Regresa verdadero en caso de que no haya paréntesis de sobra ni hagan falta
    //CharStack parenthesis_pile;
    vector<char> parenthesis_pile;
    bool error = false;
	int i;
	char c = '(';
    for(i=0; message[i] != '\0' && !error; i++)
	{
        if(message[i] == '(')
		{
            parenthesis_pile.push_back(c);
		}
		else
        if(message[i] == ')')
		{

            if ( parenthesis_pile.empty() )
			{
                error = true;
			}
            else
            {
                parenthesis_pile.pop_back();
            }
		}
	}
    if(error)
    {
        return false;
    }
    else
        if( parenthesis_pile.empty() )
        {
            return true;
        }
        else
        {
            return false;
        }	
}




