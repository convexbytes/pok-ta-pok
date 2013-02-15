#include "client.h"

using namespace std;

int main( int argc, char **argv )
{
    bool goalie;
    if( argc > 1 )
    {
        if( strcmp( "p", argv[1] ) == 0 )
        {
                goalie = true;
        }
        else
            goalie = false;
    }
    else
        goalie = false;

    Client::instance().main_loop( goalie );

    return 0;
}

