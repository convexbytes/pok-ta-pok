#include "montecarloLocalization.h"
#include "localizationMethods.h"
#include "utilities.h"
#include <stdlib.h>
#include <cmath>
#include <time.h>
#include <stdio.h>
MontecarloLocalization::MontecarloLocalization(GameData *game_data )
{
    this->game_data = game_data;
    indice_part_mayor_peso = 0;
}

///CREADOR DEL GRID  ++++///////////////////////
void MontecarloLocalization::montecarlo_prediction( Particula *particulas,
                                                    Control U,
                                                    Particula *particulas_nuevas
                                                    )
{   
	int  i;
	for (i = 0; i < NUM_PARTICULAS ; i++)
	 {
	  particulas_nuevas[i] = Sample_Motion_Model(U,particulas[i],i);   /// Realiza el movimiento para las 4 posiciones que definen el grid
	/*  printf(" %lf %lf %lf p[%d]",particulas_nuevas[i].x,
								  particulas_nuevas[i].y,
								  particulas_nuevas[i].theta,
								  i);*/
	 }	  
}
/// ////////////+++CREADOR DEL GRID+++////////
/// SE crea el grid en base al modelo de movimiento
Particula MontecarloLocalization::Sample_Motion_Model( Control const    &  U,
                                                       Particula const  &  particula,
                                                       int indice
                                                       )
{
    double inertia_moment   = game_data->game_parameter.server_param.inertia_moment;
    double player_rand      = game_data->game_parameter.server_param.player_rand;
    double effort           = game_data->sensor_handler.last_sense.effort;
    double stamina          = game_data->sensor_handler.last_sense.stamina;
    double body_dis_vel     = game_data->sensor_handler.last_sense.speed_amount;
    double body_dir_vel     = game_data->sensor_handler.last_sense.speed_direction;
    double neck_dir         = game_data->sensor_handler.last_sense.head_angle;
    
    double speed_max        = game_data->game_parameter.server_param.player_speed_max;                   /// Agregado por gomez

    double act_ang = 0.0;
    double r1, r2, rmax, r_angle;
    
    double mov_x_min, mov_x_max, mov_y_min, mov_y_max;
    double act_ang_min, act_ang_max;
    
    double  magnitud_velocidad;
    double  finalOrientation;
    double  sumaCos,sumaSin;
   
    static Particula X;   
      
    Vector2D aceleracion( 0.0, 0.0);
    Vector2D velocidad( 0.0, 0.0);
    Vector2D movimiento( 0.0, 0.0 );
    
    mov_x_min = mov_x_max = mov_y_min = mov_y_max = act_ang_min = act_ang_max = 0.0;
    r1 = r2 = rmax = r_angle = magnitud_velocidad = finalOrientation = sumaCos = sumaSin = 0.0;
   
    if( U.dash_power == 0.0 )
    {
        if( U.turn_angle == 0.0)
        {
            if( body_dis_vel == 0.0 )  // No importa la direccion que tenga dir la velocidad es cero
            {
                movimiento.x = movimiento.y = act_ang = 0.0;
                
                //X.theta = particula.theta;
            }
            else //velocidad distinta de cero
            {
                movimiento = Vector2D::fromPolar( body_dis_vel, grad2rad( body_dir_vel  + particula.theta + neck_dir ) );
                
                rmax = player_rand * body_dis_vel;  //player_rand multiplicado por la norma de la velocidad
                
                r1 = -rmax*2.0;
                r2 =  rmax*2.0;
                
                /// Obtenemos la coordenada en X y Y mas grande y mas pequeña de ambas
                
                mov_x_min = movimiento.x + r1;
                mov_x_max = movimiento.x + r2;
                
                mov_y_min = movimiento.y + r1;
                mov_y_max = movimiento.y + r2;                 
                                
                //X.theta = particula.theta;
            }
        }
        else  //Turn != 0.0
        {
            if( body_dis_vel == 0.0 ) // dash_power y body_dis_vel igual a cero,  turn distinto a cero.
            {
				act_ang_min = U.turn_angle * ( 1.0 - player_rand );
                act_ang_max = U.turn_angle * ( 1.0 + player_rand );                
              
                //X.theta = entre180 (particula.theta + U.turn_angle);
                		                      
                movimiento.x = movimiento.y = 0.0;
            }
            else
            {   
                movimiento = Vector2D::fromPolar( body_dis_vel, Deg2Rad( body_dir_vel  + particula.theta + neck_dir ) );
         
                rmax = player_rand * body_dis_vel; //player_rand multiplicado por la norma de la velocidad
         
                act_ang_min = U.turn_angle * ( 1.0 - player_rand ) / (1.0 + inertia_moment*body_dis_vel );
                act_ang_max = U.turn_angle * ( 1.0 + player_rand ) / (1.0 + inertia_moment*body_dis_vel );
                
              /*  X.theta = entre180(
									particula.theta 
									+ Rad2Deg(atan2(sin(Deg2Rad(act_ang_min)) + sin(Deg2Rad(act_ang_max)),
									    		    cos(Deg2Rad(act_ang_min)) + cos(Deg2Rad(act_ang_max))
												   )
											 )
								  );       
                */
                r1 = -rmax*2.0;
                r2 =  rmax*2.0;
                
                /// Obtenemos la coordenada en X y Y mas grande y mas pequeña de ambas
                
                mov_x_min = movimiento.x + r1;
                mov_x_max = movimiento.x + r2;
                
                mov_y_min = movimiento.y + r1;
                mov_y_max = movimiento.y + r2; 
                
            }
        }
    }
    else  // Dash != 0.0
    {
        if( U.turn_angle == 0.0 )
        {
            if( body_dis_vel == 0.0 )  // dash_power distinto a cero,  turn y body_dis_vel igual a cero.
            {   
				aceleracion = modelo_aceleracion( U.dash_power,
                                                  U.dash_dir,
                                                  stamina,
                                                  effort,
                                                  Deg2Rad( particula.theta ) );
                //aceleracion = modelo_aceleracion_gomez( U.dash_power, stamina, effort, particula.theta );
                rmax = player_rand * aceleracion.normita();
                
                r1 = -rmax*2.0;  
                r2 =  rmax*2.0; 
                
                /// Obtenemos la coordenada en X y Y mas grande y mas pequeña de ambas
                
                mov_x_min = movimiento.x + r1;
                mov_x_max = movimiento.x + r2;
                
                mov_y_min = movimiento.y + r1;
                mov_y_max = movimiento.y + r2;                              
                
                //X.theta = particula.theta;
              
            }
            else  // dash_power y body_dis_vel distinto a cero,  turn igual a cero.
            {
                aceleracion = modelo_aceleracion( U.dash_power,
                                                  0.0,
                                                  stamina,
                                                  effort,
                                                  Deg2Rad( particula.theta ) );
                                                  
                velocidad = Vector2D::fromPolar(body_dis_vel, Deg2Rad( body_dir_vel + particula.theta + neck_dir ) );
                  
                rmax = player_rand * ( aceleracion + velocidad ).normita() ;
                
                velocidad += aceleracion;
                
                magnitud_velocidad = velocidad.normita();

                // Normalizamos la velocidad, la max velocidad es speed_max
                if( magnitud_velocidad > speed_max )
                   {
					    velocidad.x = velocidad.x * speed_max / magnitud_velocidad;
					    velocidad.y = velocidad.y * speed_max / magnitud_velocidad;
					}    
                              
                r1 = -rmax*2.0;       
                r2 =  rmax*2.0;       
                                  
                movimiento.x = velocidad.x;
                movimiento.y = velocidad.y;                
                
                /// Obtenemos la coordenada en X y Y mas grande y mas pequeña de ambas
                
                mov_x_min = movimiento.x + r1;
                mov_x_max = movimiento.x + r2;
                
                mov_y_min = movimiento.y + r1;
                mov_y_max = movimiento.y + r2;
                
                //                X.theta = particula.theta;
            }
        }
        else
        {
            if( body_dis_vel == 0.0 ) // dash_power y turn distinto a cero,  body_dis_vel igual a cero.
            {
                //no es posible realizar dash y turn en el mismo ciclo
                //printf("Aqui no tiene que entrar\n");
            }
            else
            {
				//printf("Aqui no tiene que entrar\n");
                //no es posible realizar dash y turn en el mismo ciclo
            }
        }
    }

    /// ***************************************************** ///
        
    switch (indice)
	{
		case 0:	X.x     = particula.x     + mov_x_min;
		        X.y     = particula.y     + mov_y_min;		        
                X.theta = particula.theta;
		        break;
		case 1: X.x     = particula.x     + mov_x_max;
		        X.y     = particula.y     + mov_y_min; 		        
                X.theta = particula.theta;
		        break;
		case 2: X.x     = particula.x     + mov_x_min;
		        X.y     = particula.y     + mov_y_max;		        
                X.theta = particula.theta;
		        break;
		case 3: X.x     = particula.x     + mov_x_max;
		        X.y     = particula.y     + mov_y_max;		       
		        X.theta = particula.theta;
		        break;
		    	
		default: //printf("\n***Indice de entrada no valido***\n");
		        break;			
	}       
      			       							          
    return X;  
    	
}

/// ////////// Termina Creador del GRID ///////

/// ////////// Comienza la etapa de eleccion de particula ///////////////
void MontecarloLocalization::montecarlo_correction( Particula   *particulas,
                                                    SeeSensor   *vision,
                                                    Particula   *particulas_nuevas                                                    
                                                  )
{
    int     i,j,k,banderas_vistas;
    double  pesoMax, aumentoX, aumentoY, Xmin,Xmax,Ymin,Ymax;
    double  orientacion, xmin,ymin,xmax,ymax;
    double  finalOrientation;
    double  sumaCos,sumaSin;
    int     elementosX, elementosY;
    //int time;
    char   id_linea;
    

    double  neck_dir;
    neck_dir = game_data->sensor_handler.last_sense.head_angle;

    static double  grid[2][(tamGrid+1)*(tamGrid+1)];
    static double  peso[(tamGrid+1)*(tamGrid+1)];
    
    Vector2D coorb1,coorb2,posicion;
    vector<Flag>   *banderas = &vision->flags;
        
    pesoMax = 0.0;
    banderas_vistas = banderas->size();
    orientacion = 0.0;
    finalOrientation = 0.0;
    sumaSin = 0.0;
    sumaCos = 0.0;
    
    xmin = ymin = 9999.9999; 
    xmax = ymax = -9999.9999;    
        
    Xmin = particulas[0].x;
    Ymin = particulas[0].y;
    
    Xmax = particulas[3].x;
    Ymax = particulas[3].y;
    
    // aumentoX = (Xmax - Xmin) / tamGrid ;   
	// aumentoY = (Ymax - Ymin) / tamGrid ;   
	
	aumentoX = distanciaEntreParticulas;
	aumentoY = distanciaEntreParticulas;
	
	elementosX = ceil((Xmax - Xmin) / distanciaEntreParticulas);  //techo
	elementosY = ceil((Ymax - Ymin) / distanciaEntreParticulas);  //techo
	
	k=0;	   /// k es el número de particulas que es posible calcular con 
	           /// con una diferencia entre ellas de distanciaEntreParticulas 
	 
	
    /// Generamos el Grid con sus coordenadas    
    for (i = 0; i < elementosY+1 ; i++)
	 {
		for ( j = 0; j < elementosX+1 ; j++)
		       {
					grid[0][k] =  Xmin + (aumentoX*j);
	                grid[1][k] =  Ymin + (aumentoY*i);	  	           	   
	                peso[k] = 0.0;
	                k++;
	           }	    	  
	 }	
	/// el arreglo grid contiene las posibles coordenadas.	
   
   
    /// Elegimos la particula con mayor peso, o el promedio de las más pesadas
   for( i = 0 ; i < k ; i++ )
    {    
     for( j=0; j<banderas_vistas; j++)
        {
    	 peso[i] = peso[i] + Landmark_Model_Known_Corresponce( banderas->at(j), grid[0][i], grid[1][i], particulas[i].theta,neck_dir);
        }
             
     if(peso[i]>pesoMax)
          {
		   pesoMax = peso[i];		   		   
		  }	        
    } 
    
   if( pesoMax == 0.0 )  // AGENTE PERDIDO, ninguna particula del grid tiene peso
       {

		    if(vision->lines.size() >= 1 && banderas->size() >= 1)  // SI VE LINEA Y UNA BANDERA
		       {		        	
	            coorb1.x = banderas->at(0).get_global_coord().x;
		          coorb1.y = banderas->at(0).get_global_coord().y; 	          
		          
				  switch (vision->lines.at(0).id)
				  {
					case LINE_TOP: id_linea = 	't'; break;    // LINE_TOP
					case LINE_RIGHT:	id_linea =	'r'; break;    // LINE_RIGHT
					case LINE_BOTTOM:	id_linea =	'b'; break;    // LINE_BOTTOM
					case LINE_LEFT:	id_linea =	'l'; break;    // LINE_LEFT
					default: break;
						
				  }  
		          	 
				  posicion = ubicacionLineaBandera(vision->lines.at(0).dir, id_linea,
												   banderas->at(0).dis, banderas->at(0).dir,
												   coorb1);

				  particulas[0].x = posicion.x - 2.0;
				  particulas[0].y = posicion.y - 2.0;
                  
				  particulas[1].x = posicion.x + 2.0;
				  particulas[1].y = posicion.y - 2.0;
    
				  particulas[2].x = posicion.x - 2.0;
				  particulas[2].y = posicion.y + 2.0;
    
				  particulas[3].x = posicion.x + 2.0;
				  particulas[3].y = posicion.y + 2.0;
				  
    		  montecarlo_correction( particulas, vision , particulas_nuevas );   ///Correccion, encontramos particula con mas peso
    
		        } 
          else

		    if( banderas->size() >= 2)   // SI VEN DOS BANDERAS
	          {					 
		          coorb1.x = banderas->at(0).get_global_coord().x;
		          coorb1.y = banderas->at(0).get_global_coord().y;
		       
		          coorb2.x = banderas->at(1).get_global_coord().x;
		          coorb2.y = banderas->at(1).get_global_coord().y;

		          posicion = ubicacionBanderaBandera(banderas->at(0).dis, banderas->at(0).dir, coorb1,
				    								 banderas->at(1).dis, banderas->at(1).dir, coorb2);

		          xmin = particulas[0].x = posicion.x - 3.0;
				  ymin = particulas[0].y = posicion.y - 3.0;      
                  
				  particulas[1].x = posicion.x + 3.0;
				  particulas[1].y = posicion.y - 3.0;
    
				  particulas[2].x = posicion.x - 3.0;
				  particulas[2].y = posicion.y + 3.0;
    
				  xmax = particulas[3].x = posicion.x + 3.0;
				  ymax = particulas[3].y = posicion.y + 3.0;
				  
	     	      montecarlo_correction( particulas, vision , particulas_nuevas );   ///Correccion, encontramos particula con mas peso
    				  
			  }   
		   else
			  {
					 // NO SE PUEDE LOCALIZAR AL AGENTE
			  }
			       	 
       }
    else   // Utilizamos el Grid para localizar al agente
       {
	    for( i = 0; i < k; i++ )
	       {
	        if( peso[i]==pesoMax )
	           {
		        if( grid[0][i] < xmin )
		           xmin = grid[0][i];
		  
		        if( grid[1][i] < ymin )
		           ymin = grid[1][i];
		  
		        if( grid[0][i] > xmax )
		           xmax = grid[0][i];
		  
		        if( grid[1][i] > ymax )	 
		           ymax = grid[1][i];
		       }
	       }  
    	
    	 for( i = 0; i < NUM_PARTICULAS ; i++ )
	        {
		     switch (i)
		           {
		 	        case 0: particulas_nuevas[i].x = xmin;
		                    particulas_nuevas[i].y = ymin;
		            break; 
		            
		            case 1: particulas_nuevas[i].x = xmax;
					        particulas_nuevas[i].y = ymin;
		            break;
		    
		            case 2: particulas_nuevas[i].x = xmin;
					        particulas_nuevas[i].y = ymax;
			        break;
		 
		            case 3: particulas_nuevas[i].x = xmax;
					        particulas_nuevas[i].y = ymax;
			        break;
			
			        default: break;				 
	               }
	        }
	   } //termina el else  del grid   
	    
	
	/// Método para calcular la orientación del agente///    
	finalOrientation = orientacion = sumaSin = sumaCos = 0.0;
	for( i = 0; i <banderas_vistas ;i++)
		{
	     orientacion = entre180( Rad2Deg( 
	                                     atan2(banderas->at(i).get_global_coord().y - ((ymin+ymax)/2.0) ,
			         			               banderas->at(i).get_global_coord().x - ((xmin+xmax)/2.0) 
			         			               )
			         			        )
				    			- banderas->at(i).dir
				    			);
				    			
		 sumaSin += sin(Deg2Rad(orientacion)); // radianes
		 sumaCos += cos(Deg2Rad(orientacion)); // radianes		    					 
	     
		}
	
	finalOrientation = atan2(sumaSin,sumaCos);	//radianes
	
	for (i = 0; i < NUM_PARTICULAS; i++)
	    {
		 particulas_nuevas[i].theta = entre180( Rad2Deg(finalOrientation)- neck_dir);
	/*	 printf(" %lf %lf %lf p[%d] +\n",particulas_nuevas[i].x,
									   particulas_nuevas[i].y,
									   particulas_nuevas[i].theta,
									   i);	 	*/
	    }
    /// Terminamos asignando la orientacion final  ///
    
	//time = game_data->sensor_handler.last_see.time;
	
	
}
/// +++++++++++++   //////


///  Inicia la ponderacion de cada particula del grid, es donde se da peso a cada particula del grid ///
double MontecarloLocalization::Landmark_Model_Known_Corresponce    (    Flag   & bandera,
                                                                             double x,
                                                                             double y,
                                                                             double theta,
                                                                             double neck_dir
                                                                   )
{
    double  r_aprox;
    double  probabilidad;
    double  distancia_x;
    double  distancia_y;
    double  r_min,r_max;
    double  q_step_l;
    double  denominador;
    double  numerador;
    double  phi_aprox_rad;
    double  phi_min, phi_max;
    double  variable_momentanea1,variable_momentanea2;
                                 
    q_step_l = game_data->game_parameter.server_param.quantize_step_l;
    
    distancia_x = bandera.get_global_coord().x - x;
    distancia_y = bandera.get_global_coord().y - y;
    
    r_aprox = sqrt( (distancia_x * distancia_x) + (distancia_y * distancia_y)  );
    
    r_min = exp( (rint ( log(  (rint( bandera.dis/ 0.1 ) - 0.5 ) * 0.1 ) / q_step_l ) - 0.5 ) * q_step_l );
    r_max = exp( (rint ( log(  (rint( bandera.dis/ 0.1 ) + 0.5 ) * 0.1 ) / q_step_l ) + 0.5 ) * q_step_l );
    
    ///Si la distancia sensada se encuentra entre la maxima y minima posible se pondera esa particula ///
    if(r_aprox >= r_min && r_aprox <= r_max )
      {
       variable_momentanea1 = 1.0; // / (r_max - r_min) ;      
      }
    else
      {
       variable_momentanea1 = 0.0;   
      }     
  /*
    numerador   = bandera.get_global_coord().y - y;
    denominador = bandera.get_global_coord().x - x;

    phi_aprox_rad = atan2( numerador, denominador ) - Deg2Rad(theta + neck_dir);

    phi_min = (rint( bandera.dir / 1.0) - 0.5) * 1.0;   // Grados
    phi_max = (rint( bandera.dir / 1.0) + 0.5) * 1.0;   // Grados
    
    ///Si el angulo sensado se encuentra entre el maximo y minimo posible se pondera esa particula ///
    if(Rad2Deg(phi_aprox_rad) >= phi_min && Rad2Deg(phi_aprox_rad) <= phi_max)
    {
        variable_momentanea2 = 1.0; // / (phi_max -  phi_min);
    }
    else
    {
        variable_momentanea2 = 0.0;
    }

    probabilidad = variable_momentanea1 + variable_momentanea2;
*/

    probabilidad = variable_momentanea1;
    return probabilidad;    
}

/// Modelo de aceleracion tomando las formulas directamente del servidor ///
Vector2D MontecarloLocalization::modelo_aceleracion( double power,
                                                     double dir,
                                                     double stamina,
                                                     double effort,
                                                     double body_angle
                                                     )
{

    ServerParam & param     = game_data->game_parameter.server_param;
    Vector2D    aceleracion;
    double magnitud;
    // parámetros servidor... //
    double dashAngleStep    = param.dash_angle_step;
    double minDashPower     = param.min_dash_power;
    double maxDashPower     = param.max_dash_power;
    double minDashAngle     = param.min_dash_angle;
    double maxDashAngle     = param.max_dash_angle;
    double extraStamina     = param.extra_stamina;
    double sideDashRate     = param.side_dash_rate;
    double backDashRate     = param.back_dash_rate;
    double dashPowerRate    = param.dash_power_rate;    
    double speed_max        = param.player_speed_max;                   /// Agregado por gomez
    
    //double slownessOnTopForLeft = param.slowness_on_top_for_left_team;
    //double slownessOnTopForRight = param.slowness_on_top_for_right_team;

    //double M_stamina        = 0.0;
    //game_data->obs_handler.last_sense.st

    power = NormalizeDashPower  ( power, minDashPower, maxDashPower );
    dir   = NormalizeDashAngle  ( dir, minDashAngle, maxDashAngle );

    if ( dashAngleStep < EPS )
    {
        // players can dash in any direction.
    }
    else
    {
        // The dash direction is discretized by server::dash_angle_step
        dir = dashAngleStep * rint( dir / dashAngleStep );
    }

    bool back_dash = power < 0.0;

    double power_need = ( back_dash
                          ? power * -2.0
                          : power );
    power_need = std::min( power_need, stamina + extraStamina );
    //M_stamina = std::max( 0.0, stamina() - power_need );

    power = ( back_dash
              ? power_need / -2.0
              : power_need );

    double dir_rate = ( std::fabs( dir ) > 90.0
                        ? param.back_dash_rate - ( ( backDashRate - sideDashRate)
                                                   * ( 1.0 - ( std::fabs( dir ) - 90.0 ) / 90.0 ) )
                        : sideDashRate + ( ( 1.0 - sideDashRate )
                                                   * ( 1.0 - std::fabs( dir ) / 90.0 ) )
                        );
    dir_rate = bound( 0.0, dir_rate, 1.0 );
    //printf( "dir_rate: %lf", dir_rate );
    double effective_dash_power = std::fabs( effort
                                             * power
                                             * dir_rate
                                             * dashPowerRate );
    /* Los parámetros por default de slownessOnTopForLeft y slownessOnTopForRight son 1, así que no afecta.

    if ( pos_y < 0.0 )
    {
        effective_dash_power /= ( side == 'l'
                                  ? slownessOnTopForLeft
                                  : slownessOnTopForRight );
    }
    */
    if ( back_dash )
    {
        dir += 180.0;
    }

    aceleracion = Vector2D::fromPolar( effective_dash_power,
                              normalize_angle( body_angle + Deg2Rad( dir ) ) );

    // Normalizamos, la aceleración max es un parámetro del servidor, igual a 1.05

    magnitud = aceleracion.normita();
    if( magnitud > speed_max )
        {
			aceleracion.x = aceleracion.x * speed_max / magnitud;
			aceleracion.y = aceleracion.y * speed_max / magnitud;
		}	

    return aceleracion;
}

