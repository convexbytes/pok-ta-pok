#ifndef SERIALIZER_H
#define SERIALIZER_H
#include <string>
#include "observation.h"
#include "gameCommand.h"


class Serializer
{    
public:
    Serializer();
    static void generate_command ( char *output,  GameCommand & game_command);
private:
    static char command_aux[4096];
    static char command_aux2[4096];


};
#endif /* SERIALIZER_H */
