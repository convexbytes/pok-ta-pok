#ifndef SERIALIZER_H
#define SERIALIZER_H
#include <string>
#include <vector>

#include "gameCommand.h"

using namespace std;
class Serializer
{    
public:
    // Versión vieja del serializer, genera todos los comandos en una cadena;
    // pero se descartó porque el servidor no ejecuta dos comandos en un mismo
    // mensaje udp.
    static void generate_command ( char *output,
                                   AgentCommand & game_command);

    // Segunda versión del serializador, guarda cada comando serializado en un elemento
    // del vector.
    static void serializeAgentCommands( const AgentCommand & command,
                                        vector<std::string> *container );
    
    // Añadido para el entrenador.
    static void serializeTrainerCommands( const TrainerCommand  & command,
                                          vector<string> *container );

private:
    static char command_aux[4096];
    static char command_aux2[4096];


};
#endif /* SERIALIZER_H */
