#ifndef CLIENT_H
#define CLIENT_H
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


class Client
{
private:
    Client();
    static Client client;
public:
    //initial();
    static Client & instance();
	~Client();
    void main_loop();

    static void * process_thread_function(void *parameter);
    static void * sending_thread_function(void *parameter);
    static void   signal_controller( int num );

    pthread_mutex_t  message_stack_mutex; /* mutex para el main_loop y el process_thread */
    pthread_mutex_t  command_mutex; /* mutex para el process_thread y el sending_thread */
    pthread_mutex_t  time_mutex;
    pthread_t        process_thread;
    pthread_t        sending_thread;

private:
    char buffer_in[4096]; //4096 es el tamaño usado por send y receive en udpsocket

    GameData        *   game_data;
    AgentResponse   *   agent_response;
    AgentResponse   *   response_commited;
    Parser          *   parser;
    GilAgent        *   agent;
    LocalizationEngine * localization_engine;

    deque<string>       messages;
    //Synchronizer        synchronizer; //Decidimos que el sincronizador no era necesario usando tres hilos
    MP_MessageType      last_msg_type;
    int sending_wait_time; //microsegundos

    void initialize     ( );
    void pre_filter     ( char *server_msg );
    bool server_is_alive( );
    void set_sending_wait_time(char *message);

};

#endif
