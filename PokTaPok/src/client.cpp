#include "client.h"
#include "udpSocket.h"
#include "gameData.h"
#include "agentResponse.h"
#include "parser.h"
#include "localizationEngine.h"
#include "microParser.h"
#include "udpSocket.h"
#include "serializer.h"
#include "gilAgent.h"
#include <pthread.h>
#include <signal.h>
#include <deque>
#include <string>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

// ¿por qué decidiste poner estas variables aqui en lugar de en la definición de la clase?
//
pthread_mutex_t message_stack_mutex; /* mutex para el main_loop y el process_thread */
pthread_mutex_t command_mutex; /* mutex para el process_thread y el sending_thread */
pthread_mutex_t time_mutex;
pthread_t process_thread;
pthread_t sending_thread;

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
	int process_thread_error, sending_thread_error;
	game_data = new GameData();
	parser = new Parser(game_data);
	agent = new GilAgent();
	agent_response = new AgentResponse();
	response_commited = new AgentResponse();
	localization_engine = new LocalizationEngine(game_data, response_commited);

	this->last_msg_type = MP_NONE;

	//Creamos los mutex.
	if ((pthread_mutex_init(&message_stack_mutex, NULL)
			|| pthread_mutex_init(&command_mutex, NULL)
			|| pthread_mutex_init(&time_mutex, NULL)) != 0) {
		printf("\nMutex could not be created, exiting...\n");
		exit(1);
	}
	//Creamos los hilos.
	process_thread_error = pthread_create(&process_thread, NULL,
			Client::process_thread_function, NULL);
	sending_thread_error = pthread_create(&sending_thread, NULL,
			Client::sending_thread_function, NULL);
	if (process_thread_error || sending_thread_error) {
		printf("\nThreads could not be created, exiting...\n");
		exit(1);
	}
	//Asociamos nuestra función para manejar señales.
	signal(SIGINT, signal_controller);
}

Client::~Client() {
	// no necesitamos eliminar los threads y los mutex?
}

void Client::main_loop() {
	Client::instance().initialize();
	USock::instance().Initial("localhost", 6000);
	USock::instance().Send("(init poktapok (version 15.1))");
	MP_MessageType mp_type; //Tipo de mensaje del microparser
	while (server_is_alive()) {
		USock::instance().Receive(buffer_in);
		mp_type = Client::instance().last_msg_type =
				(MP_MessageType) MicroParser::get_message_type(buffer_in); //Le decimos al hilo de envio el tipo de mensaje que nos lleg̣a.
		if (mp_type == MP_SENSE_BODY) {
			//printf("Sense arrived\n");
		}
		pthread_mutex_lock(&message_stack_mutex);
		Client::instance().messages.push_back(buffer_in);
		pthread_mutex_unlock(&message_stack_mutex);

	}
}

void * Client::process_thread_function(void *parameter) {
	char server_message_aux[4096];
	bool stack_empty;
	while (1) {
		pthread_mutex_lock(&message_stack_mutex);
		stack_empty = Client::instance().messages.empty();
		pthread_mutex_unlock(&message_stack_mutex);
		if (stack_empty) //Si la pila tiene mensajes, leemos; si no tiene, dormimos.
		{
			usleep(1000); //Duerme un milisegundo mientras se llena el stack de mensajes
		} else {
			pthread_mutex_lock(&message_stack_mutex);
			strcpy(server_message_aux,
					Client::instance().messages.front().c_str());
			Client::instance().messages.pop_front();
			pthread_mutex_unlock(&message_stack_mutex);

			//Pasamos al "filtro" para alimentar game_data.
			Client::instance().pre_filter(server_message_aux);

			//Vaciamos el response.
			Client::instance().agent_response->flush();

			//Recibimos la respuesta del agente.
			Client::instance().agent->do_process(Client::instance().game_data,
					Client::instance().agent_response,
					Client::instance().response_commited);

			//Ejecutamos el serializer y guardamos el mensaje.
			//pthread_mutex_lock( & command_mutex ); //Este mutex causa un deadlock la última vez que se probó.
			//Serializer::generate_command( Client::instance().command, Client::instance().agent_response->command );
			//pthread_mutex_unlock( & command_mutex );

		}
	}
	return NULL;
}

void* Client::sending_thread_function(void *parameter) {
	int time;
	MP_MessageType msg_type;
	char command_aux[4096];
	while (1) {
		//pthread_mutex_lock( & time_mutex );
		msg_type = Client::instance().last_msg_type;
		//pthread_mutex_unlock( & time_mutex );
		if (msg_type == MP_SENSE_BODY) {
			//Destinado para cuando trabaje el módulo synchronizer
			//Client::instance().synchronizer.msg_arrived(msg_type);
			//time = Client::instance().synchronizer.get_current_cycle_wait_time();
			time = 90000;
			msg_type = MP_NONE;
			usleep(time);

			// es esto threadsafe?
			//
			//pthread_mutex_lock( & command_mutex );
			strcpy(command_aux, Client::instance().command);
			//pthread_mutex_lock( & command_mutex );

			// ¿la línea anterior no debería ir después de esto?
			//
			Serializer::generate_command(Client::instance().command, Client::instance().agent_response->command);
			USock::instance().Send(command_aux);
			*Client::instance().response_commited =
					*Client::instance().agent_response;
			//cout << "\nSent: " << Client::instance().command;
		} else {
			usleep(1000);
		}

	}
	return NULL;
}

bool Client::server_is_alive() {
	return true;
}

void Client::pre_filter(char * server_msg) {
	parser->parse(server_msg);
	localization_engine->update_world();

}

void Client::signal_controller(int num) {
	// ¿por qué no pusimos este código en el main?
	//
	//Este código se ejecutará cuando se reciba alguna señal.
	printf("\nReleasing memory and exiting program...");
	pthread_kill(process_thread, SIGQUIT);
	pthread_kill(sending_thread, SIGQUIT);
	pthread_mutex_destroy(&message_stack_mutex);
	pthread_mutex_destroy(&command_mutex);
	pthread_mutex_destroy(&time_mutex);
	delete Client::instance().parser;
	delete Client::instance().game_data;
	delete Client::instance().agent_response;
	delete Client::instance().response_commited;
	delete Client::instance().localization_engine;
	delete Client::instance().agent;
	printf("\nSignal received, exiting now...\n");
	exit(0);
}
