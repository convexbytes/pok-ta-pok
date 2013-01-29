#ifndef MICRO_PARSER_H
#define MICRO_PARSER_H

enum MP_MessageType
{
    MP_NONE = 0,
    MP_INIT,
    MP_SERVER_PARAM,
    MP_PLAYER_PARAM,
    MP_MSG,
    MP_HEAR,
    MP_SENSE_BODY,
    MP_SEE,
	MP_PLAYER_TYPE,
	MP_OK,
	MP_SEE_GLOBAL
};

class MicroParser
{
	public:
        static bool parenthesis_are_matched(char *message);
        static bool message_is_correct(char *message);
        static int get_message_type(char * message);
        static int get_message_time(char *message);
};


#endif //MICRO_PARSER_H
