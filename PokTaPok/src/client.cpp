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


	game_data   = 0;
	parser      = 0;
	agent       = 0;
	agent_command      = 0;
	command_to_commit  = 0;

	//Iniciamos todos los objetos.
	game_data           = new GameData  ( );
	parser              = new Parser    (game_data);
	agent               = new PokTaPokAgentV1  ( game_data );
	agent_command       = new AgentCommand     ( );

	//Comprobamos que se crearon todos los objetos
	if( !( game_data && parser && agent && agent_command ) )
	{
		std::cerr << "Objects could not be created, exiting..."
				  << std::endl;

		if( game_data )
			delete game_data ;
		if( parser )
			delete parser ;
		if( agent )
			delete agent ;
		if( agent_command )
			delete agent_command ;

		exit (1);
	}

	this->last_msg_type = MP_NONE;

	//Creamos los mutex.
	if ( (     pthread_mutex_init( & Client::instance().message_stack_mutex, NULL )
			|| pthread_mutex_init( & Client::instance().command_mutex, NULL)
			|| pthread_mutex_init( & Client::instance().time_mutex, NULL)
	) != 0 )
	{
		std::cerr << "Mutex could not be created, exiting..."
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
		std::cerr << "Threads could not be created, exiting..."
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

	if( instance.agent )
		delete instance.agent;

	instance.game_data   = 0;
	instance.parser      = 0;
	instance.agent       = 0;
	instance.agent_command      = 0;

}

void Client::main_loop( bool goalie )
{
	Client::instance().initialize();
	USock::instance().Initial("localhost", 6000);

	if( goalie )
	{
		USock::instance().Send("(init PokTaPok (version 15.1) (goalie))");
	}
	else
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
	timespec wait_check_time;
	timespec rem_check_time;
	wait_check_time.tv_nsec = 100000; // .1 milisegundo
	wait_check_time.tv_sec = 0;
	//AgentCommand * comm_aux;
	while (1) {
		pthread_mutex_lock( & Client::instance().message_stack_mutex );
		stack_empty = Client::instance().messages.empty();
		pthread_mutex_unlock( & Client::instance().message_stack_mutex );
		if (stack_empty) //Si la pila tiene mensajes, leemos; si no tiene, dormimos.
		{
			nanosleep( &wait_check_time, &rem_check_time); //Duerme un milisegundo mientras se llena el stack de mensajes
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
			pthread_mutex_lock( &Client::instance().command_mutex );
			Client::instance().agent->do_process(Client::instance().game_data,
					Client::instance().agent_command );
			pthread_mutex_unlock( &Client::instance().command_mutex );

		}
	}
	return NULL;
}

void* Client::Client::sending_thread_function(void *parameter)
{
	MP_MessageType      msg_type;
	std::vector<string> commands;
	std::vector<string>::const_iterator it;
	timespec wait_time;
	timespec rem_time;
	timespec wait_check_time;
	timespec rem_check_time;
	wait_check_time.tv_nsec = 100000; // .1 milisegundo
	wait_check_time.tv_sec = 0;
	wait_time.tv_nsec = 80000000; // 80 milisegundos
	wait_time.tv_sec = 0;
	while (1)
	{
		//pthread_mutex_lock( & Client::time_mutex );
		msg_type = Client::instance().last_msg_type;
		//pthread_mutex_unlock( & Client::time_mutex );

		if ( msg_type == MP_SENSE_BODY )
		{

			msg_type = Client::instance().last_msg_type = MP_NONE;

			nanosleep( &wait_time, &rem_time );

			commands.clear();

			// Serializamos los comandos y los guardamos en el vector.
			pthread_mutex_lock( &Client::instance().command_mutex );
			Serializer::serializeAgentCommands( *Client::instance().agent_command,
					& commands );
			pthread_mutex_unlock( &Client::instance().command_mutex );

			// Enviamos todos los comandos generados.
			for( it = commands.begin(); it != commands.end(); ++it )
				USock::instance().Send( it->c_str() );

			//Copiamos lo que se envió a los datos del juego.
			pthread_mutex_lock( &Client::instance().command_mutex );
			Client::instance().game_data->command_commited =
					*Client::instance().agent_command;
			pthread_mutex_unlock( &Client::instance().command_mutex );
		}
		else
		{
			nanosleep( &wait_check_time, &rem_check_time);
		}

	}
	return NULL;
}

bool Client::server_is_alive()
{
	// Reservado para un análisis de detección de conexión con el servidor.
	return true;
}


