#include "client.h"
#include "udpSocket.h"
#include "gameData.h"
#include "parser.h"
#include "localizationEngine.h"
#include "microParser.h"
#include "udpSocket.h"
#include "serializer.h"

#include <pthread.h>
#include <signal.h>
#include <deque>
#include <string>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;


Client * Client::_instance = 0;

Client::Client() {

}

Client & Client::instance()
{
    if( _instance == 0)
    {
        _instance = new Client;
    }
    return *_instance;
}

void Client::initialize() {
    int process_thread_error; // Para capturar el error a la hora de crear el hilo de proceso
    int sending_thread_error; // Para capturar el error a la hora de crear el hilo de envío
    //Iniciamos todos los objetos para cargar toda la memoria al principio

    game_data   = 0;
    parser      = 0;
    agent       = 0;
    agent_command      = 0;
    command_to_commit  = 0;


    game_data           = new GameData  ( );
    parser              = new Parser    (game_data);
    agent               = new PokTaPokAgentV1  ( game_data );
    agent_command      = new AgentCommand     ( );
    command_to_commit  = new AgentCommand     ( );

    //Comprobamos que se crearon todos los objetos
    if( !( game_data && parser && agent && agent_command
           && command_to_commit) )
    {
        std::cout << "Objects could not be created, exiting..."
                  << std::endl;

        if( game_data )
            delete game_data ;
        if( parser )
            delete parser ;
        if( agent )
            delete agent ;
        if( agent_command )
            delete agent_command ;
        if( command_to_commit )
            delete command_to_commit ;


        exit (1);
    }

	this->last_msg_type = MP_NONE;

	//Creamos los mutex.
    if ( (     pthread_mutex_init( & Client::instance().message_stack_mutex, NULL )
            || pthread_mutex_init( & Client::instance().command_mutex, NULL)
            || pthread_mutex_init( & Client::instance().time_mutex, NULL)
         ) != 0 )
    {
        std::cout << "Mutex could not be created, exiting..."
                  << std::endl;
        exit(1);
	}
	//Creamos los hilos.
    process_thread_error = pthread_create( & Client::instance().process_thread,
                                             NULL,
                                             Client::process_thread_function,
                                             NULL );

    sending_thread_error = pthread_create( & Client::instance().sending_thread,
                                             NULL,
                                             Client::sending_thread_function,
                                             NULL );

    if ( process_thread_error || sending_thread_error)
    {
        std::cout << "Threads could not be created, exiting..."
                  << std::endl;
		exit(1);
	}

}

Client::~Client()
{
    Client & instance = Client::instance();

	pthread_kill            ( Client::instance().process_thread, SIGQUIT );
    pthread_kill            ( Client::instance().sending_thread, SIGQUIT );
    pthread_mutex_destroy   ( & Client::instance().message_stack_mutex );
    pthread_mutex_destroy   ( & Client::instance().command_mutex );
    pthread_mutex_destroy   ( & Client::instance().time_mutex );

    if( instance.parser )
        delete instance.parser;

    if( instance.game_data )
        delete instance.game_data;

    if( instance.agent_command )
        delete instance.agent_command;

    if( instance.command_to_commit)
        delete instance.command_to_commit;

    if( instance.agent )
        delete instance.agent;

    instance.game_data   = 0;
    instance.parser      = 0;
    instance.agent       = 0;
    instance.agent_command      = 0;
    instance.command_to_commit  = 0;

}

void Client::main_loop( bool goalie )
{
    Client::instance().initialize();
    USock::instance().Initial("localhost", 6000);

    if( goalie )
    {
        USock::instance().Send("(init PokTaPok (version 15.1) (goalie))");
    }
    {
        USock::instance().Send("(init PokTaPok (version 15.1))");
    }


    while( server_is_alive() )
    {

        USock::instance().Receive( buffer_in );

        Client::instance().last_msg_type =
            (MP_MessageType) MicroParser::get_message_type(buffer_in); //Le decimos al hilo de envio el tipo de mensaje que nos lleg̣a.
        //Client::instance().last_msg_cycle = MicroParser::get_message_time( buffer_in );

        pthread_mutex_lock( & Client::instance().message_stack_mutex );

            Client::instance().messages.push_back( buffer_in );

        pthread_mutex_unlock( & Client::instance().message_stack_mutex );

	}
}

void * Client::Client::process_thread_function(void *parameter)
{
	char server_message_aux[4096];
	bool stack_empty;
    AgentCommand * comm_aux;
	while (1) {
        pthread_mutex_lock( & Client::instance().message_stack_mutex );
		stack_empty = Client::instance().messages.empty();
        pthread_mutex_unlock( & Client::instance().message_stack_mutex );
		if (stack_empty) //Si la pila tiene mensajes, leemos; si no tiene, dormimos.
		{
			usleep(1000); //Duerme un milisegundo mientras se llena el stack de mensajes
        }
        else
        {
            pthread_mutex_lock( & Client::instance().message_stack_mutex);

                strcpy( server_message_aux,
                        Client::instance().messages.front().c_str()
                        );

                Client::instance().messages.pop_front();

            pthread_mutex_unlock(&Client::instance().message_stack_mutex);

            // Parseo, actualiza el objeto sensor_handler.
            Client::instance().parser->parse( server_message_aux );

            // Recibimos la respuesta del agente.
	    Client::instance().agent->do_process(Client::instance().game_data,
                    				 Client::instance().agent_command );

            // Actualizamos el comando que se tiene que enviar,
            // se utilizó un cambio de apuntadores para agilizar el proceso.
            comm_aux = Client::instance().agent_command;
            Client::instance().agent_command = Client::instance().command_to_commit;
            Client::instance().command_to_commit = comm_aux;
		}
	}
	return NULL;
}

void* Client::Client::sending_thread_function(void *parameter)
{
    int                 wait_time;
    unsigned int        i;
    MP_MessageType      msg_type;
    std::vector<string> commands;

    while (1)
    {
        //pthread_mutex_lock( & Client::time_mutex );
		msg_type = Client::instance().last_msg_type;
        //pthread_mutex_unlock( & Client::time_mutex );

        if ( msg_type == MP_SENSE_BODY )
        {
			//Destinado para cuando trabaje el módulo synchronizer
			//Client::instance().synchronizer.msg_arrived(msg_type);
			//time = Client::instance().synchronizer.get_current_cycle_wait_time();

            wait_time = 80000; // Obtenido experimentalmente

            // **Esto no es threadsafe, debería tener mutex aquí
            // **Se implementará junto con boost

            msg_type = Client::instance().last_msg_type = MP_NONE;

            usleep( wait_time );

            commands.clear();

            // **Esto no es threadsafe, debería tener mutex aquí
            // **Se implementará junto con boost

            // Serializamos los comandos y los guardamos en el vector.
            Serializer::serializeAgentCommands( *Client::instance().command_to_commit,
                                                & commands );

            // Enviamos todos los comandos generados.
            for( i=0; i<commands.size(); i++ )
            {
                USock::instance().Send( commands[i].c_str() );
            }

            // Copiamos lo que se envió a los datos del juego.
            Client::instance().game_data->command_commited =
                    *Client::instance().command_to_commit;

        }
        else
        {
            usleep(1000);
		}

	}
	return NULL;
}

bool Client::server_is_alive()
{
    // Reservado para un análisis de detección de conexión con el servidor.
	return true;
}


