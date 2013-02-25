#include "client.h"
#include "signal.h"
#include <iostream>

// Se ejecutará esta función cuando presionen ctrl-c en terminal
void signalController( int num );


int main( int argc, char **argv )
{
    bool goalie;
    if( argc == 1 )
    {
        goalie = false;

    }
    else if( argc == 2 )
    {
        if( strcmp( "p", argv[1] ) == 0 )
        {
                goalie = true;
        }
        else
        {
            std::cout << "Usage: poktapok [p]" << std::endl;
            return 0;
            //goalie = false;
        }

    }
    else
    {
        std::cout << "Usage: poktapok [p]" << std::endl;
        return  0;
        goalie = false;
    }

    //Asociamos nuestra función para manejar señales.
    signal( SIGINT, signalController );

    Client::instance().main_loop( goalie );

    return 0;
}


void signalController(int num)
{
    //Este código se ejecutará cuando se reciba alguna señal.

   std::cout << "Signal received, exiting now..." << std::endl;
    exit(0);
}
