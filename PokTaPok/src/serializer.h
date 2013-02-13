#ifndef SERIALIZER_H
#define SERIALIZER_H
#include <string>
#include <vector>

#include "observation.h"
#include "gameCommand.h"

using namespace std;
class Serializer
{    
public:
    static void generate_command ( char *output,  AgentCommand & game_command);

    // Segunda versión del serializador
    static void serializeAgentCommands( const AgentCommand & command, vector<std::string> *container );
    
    // Añadido para el entrenador
    static void serializeTrainerCommands( const TrainerCommand  & command, vector<string> *container );

private:
    static char command_aux[4096];
    static char command_aux2[4096];


};
#endif /* SERIALIZER_H */
