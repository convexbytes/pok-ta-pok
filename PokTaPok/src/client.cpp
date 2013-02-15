#include "client.h"
#include "udpSocket.h"
#include "gameData.h"
#include "agentResponse.h"
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

void signal_controller(int num); //se ejecutará esta función cuando presionen ctrl-c en terminal

Client::Client() {

}

Client & Client::instance() {
	// para mejor claridad, utilizar una variable de clase
	//
	static Client client;
	return client;
}

void Client::initialize() {
    int process_thread_error; // Para capturar el error a la hora de crear el hilo de proceso
    int sending_thread_error; // Para capturar el error a la hora de crear el hilo de envío
    //Iniciamos todos los objetos para cargar toda la memoria al principio
    game_data           = new GameData  ( );
    parser              = new Parser    (game_data);
    agent               = new PokTaPokAgentV1  ( );
    agent_response      = new AgentCommand     ( );
    response_commited   = new AgentCommand     ( );
    response_to_commit  = new AgentCommand     ( );
    localization_engine = new LocalizationEngine( game_data, response_commited );
    //Comprobamos que se crearon todos los objetos
    if( !( game_data && parser && agent && agent_response
           && response_commited && localization_engine
           && response_to_commit) )
    {
        printf( "Objects could not be created, exiting...\n" );
        if( game_data )
            free( game_data );
        if( parser )
            free( parser );
        if( agent )
            free( agent );
        if( agent_response )
            free( agent_response );
        if( response_to_commit )
            free( response_to_commit );
        if( response_commited )
            free( response_commited );
        if( localization_engine )
            free( localization_engine );

        exit (-1);
    }

	this->last_msg_type = MP_NONE;

	//Creamos los mutex.
    if ( (     pthread_mutex_init( & Client::instance().message_stack_mutex, NULL )
            || pthread_mutex_init( & Client::instance().command_mutex, NULL)
            || pthread_mutex_init( & Client::instance().time_mutex, NULL)
         ) != 0 )
    {
		printf("\nMutex could not be created, exiting...\n");
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
		printf("\nThreads could not be created, exiting...\n");
		exit(1);
	}
	//Asociamos nuestra función para manejar señales.
	signal(SIGINT, signal_controller);
}

Client::~Client() {
	pthread_kill            ( Client::instance().process_thread, SIGQUIT );
    	pthread_kill            ( Client::instance().sending_thread, SIGQUIT );
    	pthread_mutex_destroy   ( & Client::instance().message_stack_mutex );
    	pthread_mutex_destroy   ( & Client::instance().command_mutex );
    	pthread_mutex_destroy   ( & Client::instance().time_mutex );

	delete Client::instance().parser;
	delete Client::instance().game_data;
	delete Client::instance().agent_response;
	delete Client::instance().response_commited;
    delete Client::instance().response_to_commit;
	delete Client::instance().localization_engine;
	delete Client::instance().agent;
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

    MP_MessageType mp_type; //Tipo de mensaje del microparser
    while( server_is_alive() )
    {

        USock::instance().Receive( buffer_in );

        mp_type = Client::instance().last_msg_type =
            (MP_MessageType) MicroParser::get_message_type(buffer_in); //Le decimos al hilo de envio el tipo de mensaje que nos lleg̣a.
        //Client::instance().last_msg_cycle = MicroParser::get_message_time( buffer_in );

        pthread_mutex_lock( & Client::instance().message_stack_mutex );

            Client::instance().messages.push_back( buffer_in );

        pthread_mutex_unlock( & Client::instance().message_stack_mutex );

	}
}

void * Client::Client::process_thread_function(void *parameter) {
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
					Client::instance().messages.front().c_str());

			Client::instance().messages.pop_front();

            pthread_mutex_unlock(&Client::instance().message_stack_mutex);

			//Pasamos al "filtro" para alimentar game_data.
			Client::instance().pre_filter(server_message_aux);

			//Vaciamos el response.
            //Client::instance().agent_response->reset();

			//Recibimos la respuesta del agente.
			Client::instance().agent->do_process(Client::instance().game_data,
					Client::instance().agent_response,
					Client::instance().response_commited);

            comm_aux = Client::instance().agent_response;
            Client::instance().agent_response = Client::instance().response_to_commit;
            Client::instance().response_to_commit = comm_aux;
		}
	}
	return NULL;
}

void* Client::Client::sending_thread_function(void *parameter) {
	int time;
    int i;
	MP_MessageType msg_type;

    std::vector<string> commands;
	while (1) {
        //pthread_mutex_lock( & Client::time_mutex );
		msg_type = Client::instance().last_msg_type;
        //pthread_mutex_unlock( & Client::time_mutex );
        if ( msg_type == MP_SENSE_BODY )
        {
			//Destinado para cuando trabaje el módulo synchronizer
			//Client::instance().synchronizer.msg_arrived(msg_type);
			//time = Client::instance().synchronizer.get_current_cycle_wait_time();
            time = 80000;
            msg_type = Client::instance().last_msg_type = MP_NONE;
            usleep(time);

			// es esto threadsafe?
            // esto no es threadsafe, debería tener mutex aquí

            commands.clear();

            //Serializer::generate_command( command_aux, *Client::instance().agent_response );

            Serializer::serializeAgentCommands( *Client::instance().response_to_commit,
                                                & commands );

            for( i=0; i<commands.size(); i++ )
            {
                USock::instance().Send( commands[i].c_str() );
            }

			*Client::instance().response_commited =
                    *Client::instance().response_to_commit;


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

void Client::pre_filter(char * server_msg)
{
    //double x, y, ang;

    parser->parse(server_msg); // Parseo, llena el objeto obs_handler

    //localization_engine->getNewPos( x, y, ang ); // Usa obs_handler para actualizar la pose del agente
}



void Client::signal_controller(int num) {
    // Pendiente de poner este código, junto con lo referente a las señales, en el main.
	//
	//Este código se ejecutará cuando se reciba alguna señal.
	printf("\nSignal received, exiting now...\n");
	exit(0);
}
