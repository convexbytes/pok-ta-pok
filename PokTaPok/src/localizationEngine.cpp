#include "localizationEngine.h"
#include "localizationMethods.h"
#include "gameData.h"
#include "geometry.h"
#include "agentResponse.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

LocalizationEngine::LocalizationEngine( GameData * game_data, AgentCommand *command_commited )
{
    int angulo_inicial;
    int i;
    this->game_data      = game_data;
    this->command_commited = command_commited;
    this->montecarlo_loc = new MontecarloLocalization( game_data );

    angulo_inicial = 0;
    srand( time(NULL) );

    //inicializamos particulas.
    for( i=0; i<NUM_PARTICULAS; i++ )
    {
		// La cancha mide 52.5 X 32.0; añadimos un espacio extra, ya que podemos 
		// salir un poco de la cancha
        particulas[i].x =  particulas_nuevas[i].x =   ( drand48()*110.0 - 55.0 ); 
        particulas[i].y =  particulas_nuevas[i].y =     ( drand48()*80.0  - 40.0 );
        particulas[i].theta = particulas_nuevas[i].theta = ( 0.0 );

        /*particulas[i].set_x     ( -3.0 );
        particulas[i].set_y     ( -39.0 );
        particulas[i].set_theta ( 0.0 );

        particulas_nuevas[i].set_x     ( -3.0 );
        particulas_nuevas[i].set_y     ( -39.0 );
        particulas_nuevas[i].set_theta ( 0.0 );*/
    }
    this->p         = particulas;
    this->p_nuevas  = particulas_nuevas;
}

void LocalizationEngine::getNewPos( double &x, double &y, double &body_angle )
{
    ObservationType   obs_type;
    vector<Flag>    * banderas;
    AgentCommand     * controles;
    Control           U;
    Particula       * p_aux;

    banderas = & game_data->obs_handler.last_see.flags;
    obs_type = game_data->obs_handler.last_obs_type;
    controles = command_commited; // El último comando que se envió al servidor
    if( obs_type == OBS_SENSE )
    {
        // Obtenemos los controles
        // Consideramos el caso en que la dirección del comando dash es cero
        if( controles->dash_is_set() )
        {
            std::cout << "hijo de perra" << std::endl;
            U.dash_power = controles->dash_power;
            U.turn_angle = 0.0;
        }
        else if( controles->turn_is_set() )
        {
            U.dash_power = 0.0;
            U.turn_angle = controles->turn_angle;
        }
        else
        {
            U.dash_power = 0.0;
            U.turn_angle = 0.0;
        }

        //Las particulas nuevas se convierten ahora en las viejas.
        p_aux = p;
        p = p_nuevas;
        p_nuevas = p_aux;
        montecarlo_loc->montecarlo_prediction( p, U, p_nuevas);
    }
    else if( obs_type == OBS_SEE && banderas->size() > 0 ) // No tiene caso hacer correción con 0 banderas
    {
        //Las particulas nuevas se convierten ahora en las viejas.
        p_aux = p;
        p = p_nuevas;
        p_nuevas = p_aux;
        montecarlo_loc->montecarlo_correction( p, banderas, p_nuevas );
    }

    if( command_commited->move_is_set() )
    {
        for( int i=0; i<NUM_PARTICULAS; i++ )
        {
            p_nuevas[i].x = p[i].x = command_commited->move_x;
            p_nuevas[i].y = p[i].y = command_commited->move_y;
        }
    }

    x = p_nuevas[ montecarlo_loc->indiceMayorPeso() ].x;
    y = p_nuevas[ montecarlo_loc->indiceMayorPeso() ].y;
    body_angle  = p_nuevas[ montecarlo_loc->indiceMayorPeso() ].theta;

}


