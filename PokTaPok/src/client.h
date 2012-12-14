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
public:
    //initial();
    static Client & instance();
	~Client();
    void main_loop();
    static void * process_thread_function(void *parameter);
    static void * sending_thread_function(void *parameter);
    static void  signal_controller( int num );

private:
    char buffer_in[4096]; //4096 es el tamaño usado por send y receive en udpsocket
    char command[4096];

    GameData        *   game_data;
    AgentResponse   *   agent_response;
    AgentResponse   *   response_commited;
    Parser          *   parser;
    GilAgent        *   agent;
    LocalizationEngine * localization_engine;

    deque<string>       messages;
    //Synchronizer        synchronizer;
    MP_MessageType      last_msg_type;
    int sending_wait_time; //microsegundos

    void initialize     ();
    void pre_filter     ( char *server_msg );
    bool server_is_alive();
    //bool connected;
    void set_sending_wait_time(char *message);

};

#endif
