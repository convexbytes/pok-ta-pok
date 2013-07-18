#include "localizationAdapter.h"
#include "localizationMethods.h"
#include "gameData.h"
#include "geometry.h"
#include "gameCommand.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

LocalizationAdapter::LocalizationAdapter( GameData * game_data )
{
  //  int angulo_inicial;
    int i;
    this->game_data        = game_data;
    this->command_commited = &game_data->command_commited;
    this->montecarlo_loc   = new MontecarloLocalization( game_data );

   // angulo_inicial = 0;
    srand( time(NULL) );
    
    //inicializamos particulas.
    for( i=0; i<NUM_PARTICULAS; i++ )
    {
		// La cancha mide 52.5 X 32.0; añadimos un espacio extra, ya que podemos 
		// salir un poco de la cancha
        particulas[i].x =  particulas_nuevas[i].x =  -14.0; 
        particulas[i].y =  particulas_nuevas[i].y =  -25.0; 
        particulas[i].theta = particulas_nuevas[i].theta = 0.0;
    }
    this->p         = particulas;
    this->p_nuevas  = particulas_nuevas;
}

double
LocalizationAdapter::getX() const
{
	return x;
}

double
LocalizationAdapter::getY() const
{
	return y;
}

double
LocalizationAdapter::getAngle() const
{
	return angle;
}

void
LocalizationAdapter::updatePos(  )
{
    SensorType         sensor_type;
    vector<Flag>     * banderas;
    AgentCommand     * controles;
    Control            U;
    Particula        * p_aux;
    SeeSensor        * vision; 
    //static int         pasada;
    //static int         mensaje;
    //int                time;
    banderas = & game_data->sensor_handler.last_see.flags;
    vision = & game_data->sensor_handler.last_see;
    sensor_type = game_data->sensor_handler.last_sensor_type;
    controles = command_commited; // El último comando que se envió al servidor


    //double  neck_dir;
    //neck_dir = game_data->sensor_handler.last_sense.head_angle;
        
	//time = game_data->sensor_handler.last_sense.time;
    
    //int i;
    
    
    if( sensor_type == SENSOR_BODY )
    {  
        //Las particulas nuevas se convierten ahora en las viejas.
        p_aux = p;
        p = p_nuevas;
        p_nuevas = p_aux;

        // Obtenemos los controles
        // Consideramos el caso en que la dirección del comando dash es cero
        if( controles->dash_is_set() )
        {
            U.dash_power = controles->dash_power;
            U.dash_dir   = controles->dash_direction;
            U.turn_angle = 0.0;
        }
        else if( controles->turn_is_set() )
        {
            U.dash_power = 0.0;
            U.dash_dir   = 0.0;
            U.turn_angle = controles->turn_angle;
        }
        else
        {
            U.dash_power = 0.0;
            U.dash_dir   = 0.0;
            U.turn_angle = 0.0;            
        }
		
        montecarlo_loc->montecarlo_prediction( p, U, p_nuevas); /// CREADOR DEL GRID
        
        x          = (p_nuevas[0].x + p_nuevas[3].x)/2.0 ;
        y          = (p_nuevas[0].y + p_nuevas[3].y)/2.0 ;
        angle      =  p_nuevas[0].theta;                
        
        //printf("%d %lf %lf %lf %lf\n",time,x,y,angle,neck_dir);
               
    }
    else if( sensor_type == SENSOR_SEE && banderas->size() > 0 ) // No tiene caso hacer correción con 0 banderas
    {
        //Las particulas nuevas se convierten ahora en las viejas.
        p_aux = p;
        p = p_nuevas;
        p_nuevas = p_aux;        
        
        montecarlo_loc->montecarlo_correction( p, vision , p_nuevas );   ///Correccion, encontramos particula con mas peso
                 
        x          = (p_nuevas[0].x + p_nuevas[3].x)/2.0 ;
        y          = (p_nuevas[0].y + p_nuevas[3].y)/2.0 ;
        angle      =  p_nuevas[0].theta;                
        
        //printf("%d %lf %lf %lf %lf +\n",time,x,y,angle,neck_dir);
    }

    if( command_commited->move_is_set() )  ///Si aplicamos comando Move() colocamos la particula en esa posicion
    {
        for( int i=0; i<NUM_PARTICULAS; i++ )
        {
            p_nuevas[i].x = p[i].x = command_commited->move_x;
            p_nuevas[i].y = p[i].y = command_commited->move_y;
        }
    }
          
    x          = (p_nuevas[0].x + p_nuevas[3].x)/2.0 ;
    y          = (p_nuevas[0].y + p_nuevas[3].y)/2.0 ;
    angle      =  p_nuevas[0].theta;
    
}


