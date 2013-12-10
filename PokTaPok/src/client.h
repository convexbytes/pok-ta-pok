#ifndef CLIENT_H
#define CLIENT_H
#define SERVER_MAX_DELAY_SECS 3

#include "udpSocket.h"
#include "gameData.h"
#include "parser.h"
#include "localizationAdapter.h"
#include "microParser.h"
#include "udpSocket.h"
#include "serializer.h"
#include "poktapokAgent.h"



#include <pthread.h>
#include <deque>
#include <string>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;


class Client
{
private:
    Client();
    static Client * _instance;
public:
    //initial();
    static Client & instance();
	~Client();

    //Se modifica el método para recibir los nuevos parámetros
    void main_loop( string server, unsigned int port, string team_name, int goalie );

    static void * process_thread_function (void *parameter);
    static void * sending_thread_function (void *parameter);


    // mutex para acceder a la pila de mensajes. Lo usan: main_loop y process_thread
    pthread_mutex_t  message_stack_mutex;
    // mutex para acceder al comando a enviar. Lo usan: process_thread y sending_thread
    pthread_mutex_t  command_mutex;
    // threads
    pthread_t        process_thread;
    pthread_t        sending_thread;
    // variable de condición para el envío y su respectivo mutex
    pthread_cond_t	 sending_thread_cond;
    pthread_mutex_t	 sending_thread_cond_mutex;

private:
    char buffer_in[4096]; //4096 es el tamaño usado por send y receive en udpsocket

    GameData        *   game_data;
    AgentCommand        agent_command;
    AgentCommand        command_to_send;
    Parser          *   parser;
    Agent           *   agent;
    deque<string>       messages;
    //Synchronizer        synchronizer; //Decidimos que el sincronizador no era necesario usando tres hilos
    MP_MessageType      last_msg_type;

    void initialize     ( );
};

#endif

