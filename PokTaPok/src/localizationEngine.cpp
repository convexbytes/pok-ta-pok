#include "localizationEngine.h"
#include "localizationMethods.h"
#include "gameData.h"
#include "geometry.h"
#include "agentResponse.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

LocalizationEngine::LocalizationEngine( GameData * game_data, AgentResponse * agent_response )
{
    int angulo_inicial;
    int i;
    this->game_data      = game_data;
    this->agent_response = agent_response;
    this->montecarlo_loc = new MontecarloLocalization( game_data );

    angulo_inicial = 0;
    srand( time(NULL) );

    //inicializamos particulas.
    for( i=0; i<NUM_PARTICULAS; i++ )
    {

        particulas[i].x =  particulas_nuevas[i].x =   ( drand48()*105.0 - 52.5 ); //La cancha mide 52.5 X 32.0
        particulas[i].y =  particulas_nuevas[i].y =     ( drand48()*70.0  - 35.0 );
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

void LocalizationEngine::update_world()
{
    ObservationType   obs_type;
    vector<Flag>    * banderas;
    GameCommand     * controles;
    Control           U;
    Particula       * p_aux;
    int i;

    banderas = & game_data->obs_handler.last_see.flags;
    obs_type = game_data->obs_handler.last_obs_type;
    controles = & agent_response->command;
    if( obs_type == OBS_SENSE )
    {
        // obtenemos los controles
        if( controles->dash_is_set() )
        {
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
    else if( obs_type == OBS_SEE )
    {
        //Las particulas nuevas se convierten ahora en las viejas.
        p_aux = p;
        p = p_nuevas;
        p_nuevas = p_aux;
        montecarlo_loc->montecarlo_correction( p, banderas, p_nuevas );
    }

}

