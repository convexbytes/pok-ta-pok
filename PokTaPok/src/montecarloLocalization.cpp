#include "montecarloLocalization.h"
#include "localizationMethods.h"
#include "utilities.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
MontecarloLocalization::MontecarloLocalization(GameData *game_data )
{
    this->game_data = game_data;
}


void MontecarloLocalization::montecarlo_prediction( Particula *particulas,
                                                    Control U,
                                                    Particula *particulas_nuevas
                                                    )
{
    int i;

    for( i=0; i<NUM_PARTICULAS; i++ )
    {
        particulas_nuevas[i] = Sample_Motion_Model( U, particulas[i] );   
    }
}

void MontecarloLocalization::montecarlo_correction( Particula   *particulas,
                                                    vector<Flag> *banderas,
                                                    Particula *particulas_nuevas
                                                    )
{
    int     i, j, banderas_vistas;
    int     posicion_peso_max = 0;
    int     time;
    double  draw;
    double  peso[NUM_PARTICULAS];
    double  rango[NUM_PARTICULAS+1];

    double  total_peso=0.0;
    double  peso_maximo=0.0;

    rango[0]=0.0;

    Flag    bandera;

    draw            = 0.0;
    banderas_vistas = banderas->size();

    for( i=0; i<NUM_PARTICULAS; i++ )
            {
             peso[i] = 0.0;
            }

    for( i=0; i<NUM_PARTICULAS; i++ )
    {
        for( j=0; j<banderas_vistas; j++)
        {
            bandera = banderas->at(j);
            peso[i] = peso[i] + Landmark_Model_Known_Corresponce( bandera, particulas[i] );
        }

        total_peso = total_peso + peso[i];

        if(peso[i]>peso_maximo)
          {
              peso_maximo =  peso[i];
              posicion_peso_max = i;
          }
    }


    for( i=1; i<NUM_PARTICULAS+1; i++ )
    {
        rango[i] = rango[i-1] + peso[i-1];
    }

    /************ Draw Step *********************/
    for( i=0; i<NUM_PARTICULAS; i++ )
       {
           draw = drand48()*total_peso;  //# aleatorio entre 0.0 y total_peso
           //draw = drand48();
           for(j=0; j<NUM_PARTICULAS; j++ )
              {
                if(draw >= rango[j] && draw < rango[j+1])
                   {
                       particulas_nuevas[i] = particulas[i];

                      j = NUM_PARTICULAS; //salimos del for
                   }
              }
       }

    /* Imprimimos la partícula de mayor peso
*/
    time = game_data->obs_handler.last_obs_time;
    printf( "time: %d x: %lf y: %lf bodyAngle: %lf weight: %lf place: %d\n",
			time,
            particulas_nuevas[posicion_peso_max].x,
            particulas_nuevas[posicion_peso_max].y,
            particulas_nuevas[posicion_peso_max].theta,
            peso[posicion_peso_max],
            posicion_peso_max
            );

	
}


Particula MontecarloLocalization::Sample_Motion_Model( Control const    &  U,
                                                       Particula const  &  particula
                                                       )
{

    double inertia_moment   = game_data->game_parameter.server_param.inertia_moment;
    double player_rand      = game_data->game_parameter.server_param.player_rand;

    double effort          = game_data->obs_handler.last_sense.effort;
    double stamina         = game_data->obs_handler.last_sense.stamina;
    double body_dis_vel    = game_data->obs_handler.last_sense.speed_amount;
    double body_dir_vel    = game_data->obs_handler.last_sense.speed_direction;
    double neck_dir        = game_data->obs_handler.last_sense.head_angle;

    double act_ang = 0.0;
    double r1, r2, rmax, r_angle;

    Vector2D aceleracion;
    Vector2D velocidad;
    Vector2D movimiento;
    static Particula X;

    //Árbol de expansión mínima.
    if( U.dash_power == 0.0 )
    {
        if( U.turn_angle == 0.0)
        {
            if( body_dis_vel == 0.0 )
            {
                movimiento.x = movimiento.y = act_ang = 0.0;
            }
            else //velocidad distinta de cero
            {
                movimiento = Vector2D::fromPolar( body_dis_vel, grad2rad( body_dir_vel  + particula.theta + neck_dir ) );
                rmax = player_rand * body_dis_vel; //player_rand multiplicado por la norma de la velocidad
                r1 = drand48()*2.0*rmax - rmax;
                r2 = drand48()*2.0*rmax - rmax;
                movimiento.x += r1;
                movimiento.y += r2;
                act_ang = 0.0;
            }
        }
        else
        {
            if( body_dis_vel == 0.0 ) // dash_power y body_dis_vel igual a cero,  turn distinto a cero.
            {
                r_angle = drand48()*2.0*player_rand - player_rand;
                act_ang = U.turn_angle * ( 1.0 + r_angle );
                movimiento.x = movimiento.y = 0.0;
            }
            else // dash_power igual a cero,  turn y body_dis_vel distinto a cero.
            {
                movimiento = Vector2D::fromPolar( body_dis_vel, grad2rad( body_dir_vel  + particula.theta + neck_dir ) );
                rmax = player_rand * body_dis_vel; //player_rand multiplicado por la norma de la velocidad
                r1 = drand48()*2.0*rmax - rmax;
                r2 = drand48()*2.0*rmax - rmax;
                r_angle = drand48()*2.0*player_rand - player_rand;
                movimiento.x += r1;
                movimiento.y += r2;
                act_ang = U.turn_angle * ( 1.0 + r_angle ) / (1.0 + inertia_moment*body_dis_vel );
            }
        }
    }
    else
        if( U.turn_angle == 0.0 )
        {
            if( body_dis_vel == 0.0 )  // dash_power distinto a cero,  turn y body_dis_vel igual a cero.
            {
                aceleracion = modelo_aceleracion( U.dash_power,
                                                  0.0,
                                                  stamina,
                                                  effort,
                                                  Deg2Rad( particula.theta ) );
                //aceleracion = modelo_aceleracion_gomez( U.dash_power, stamina, effort, particula.theta );
                rmax = player_rand * aceleracion.normita();
                r1 = drand48()*2.0*rmax - rmax;
                r2 = drand48()*2.0*rmax - rmax;
                movimiento.x = aceleracion.x + r1;
                movimiento.y = aceleracion.y + r2;
                act_ang = 0.0;
            }
            else  // dash_power y body_dis_vel distinto a cero,  turn igual a cero.
            {
                aceleracion = modelo_aceleracion( U.dash_power,
                                                  0.0,
                                                  stamina,
                                                  effort,
                                                  Deg2Rad( particula.theta ) );
                //aceleracion = modelo_aceleracion_gomez( U.dash_power, stamina, effort, particula.theta );
                velocidad = Vector2D::fromPolar(body_dis_vel, grad2rad( body_dir_vel + particula.theta + neck_dir ) );
                //printf( "Aceleración: %lf %lf \t Velocidad: %lf %lf theta:: %lf \n", aceleracion.x, aceleracion.y, velocidad.x, velocidad.y, body_dir_vel + particula.theta + neck_dir  );
                rmax = player_rand * ( aceleracion + velocidad ).normita() ;
                r1 = drand48()*2.0*rmax - rmax;
                r2 = drand48()*2.0*rmax - rmax;
                movimiento.x = aceleracion.x + velocidad.x + r1;
                movimiento.y = aceleracion.y + velocidad.y + r2;
                act_ang = 0.0;
            }
        }
        else
        {
            if( body_dis_vel == 0.0 ) // dash_power y turn distinto a cero,  body_dis_vel igual a cero.
            {
                //no es posible realizar dash y turn en el mismo ciclo
            }
            else
            {
                //no es posible realizar dash y turn en el mismo ciclo
            }
        }
    X.x = particula.x + movimiento.x;
    X.y = particula.y + movimiento.y;
    X.theta = entre180( (double)(particula.theta + act_ang) );
    //printf("suma %lf \t xtheta: %lf", particula.theta + act_ang, X.theta );
    return X;
}



double MontecarloLocalization::Landmark_Model_Known_Corresponce(    Flag        bandera,
                                                                    Particula   pose
                                                                    )
{
    double  denominador;
    double  numerador;
    double  r_aprox;
    double  phi_aprox;
    double  probabilidad;
    double  distancia_x;
    double  distancia_y;
    double  r_min,r_max, phi_min, phi_max;
    double  desviacion_r, desviacion_phi;
    double  q_step_l;


    double variable_momentanea1, valiable_momentanea2;

    q_step_l = game_data->game_parameter.server_param.quantize_step_l;
    distancia_x = bandera.get_global_coord().x - pose.x;
    distancia_y = bandera.get_global_coord().y - pose.y;

    r_aprox = sqrt( (distancia_x * distancia_x) + (distancia_y * distancia_y)  );

    numerador   = bandera.get_global_coord().y - pose.y;
    denominador = bandera.get_global_coord().x - pose.x;

    if ( denominador > 0.0 )
        phi_aprox = atan( numerador/denominador );

    else if ( denominador < 0.0 )
        phi_aprox = signo( numerador ) * ( M_PI - atan( abs( numerador/denominador ) ) );

    else if ( denominador == 0.0 && numerador != 0.0 )
        phi_aprox = signo( numerador ) * ( M_PI / 2.0 );

    else
        phi_aprox = 0.0;

    phi_aprox = phi_aprox - grad2rad( pose.theta );

    //r_min = exp( (rint ( log(  (rint( bandera.distance/ 0.1 ) - 0.5 ) * 0.1 ) / q_step_l ) - 0.5 ) * q_step_l );
    //r_max = exp( (rint ( log(  (rint( bandera.distance/ 0.1 ) + 0.5 ) * 0.1 ) / q_step_l ) + 0.5 ) * q_step_l );

    r_min = exp( (rint ( log(  (rint( bandera.distance/ 0.1 ) - 0.5 ) * 0.1 ) / 0.1 ) - 0.5 ) * 0.01 );
    r_max = exp( (rint ( log(  (rint( bandera.distance/ 0.1 ) + 0.5 ) * 0.1 ) / 0.1 ) + 0.5 ) * 0.01 );

    desviacion_r = r_max - r_min;

    phi_min = (rint( bandera.direction / 1.0) - 0.5) * 1.0;
    phi_max = (rint( bandera.direction / 1.0) + 0.5) * 1.0;

    desviacion_phi = phi_max - phi_min;

//***//probabilidad = prob_normal_distribution( bandera.distance - r_aprox, desviacion_r ) * prob_normal_distribution( grad2rad( bandera.direction ) - phi_aprox, desviacion_phi );


    //variable_momentanea1 = prob_normal_distribution( bandera.distance - r_aprox, desviacion_r );
    variable_momentanea1 = prob_normal_distribution( bandera.distance - r_aprox, 3.0 );

    valiable_momentanea2 = prob_normal_distribution( grad2rad( bandera.direction ) - phi_aprox, desviacion_phi );


    probabilidad = variable_momentanea1 * valiable_momentanea2;


    return probabilidad;
}

double MontecarloLocalization::prob_normal_distribution( double x,
                                                         double stdev
                                                        )
{
    double r1, r2, r;

    r1 = 1.0/ ( sqrt( 2*M_PI ) * stdev );

    r2 = exp( (-0.5)*x*x/(stdev*stdev) );

    r=(exp( (-0.5)*x*x/(stdev*stdev) ))*(1.0/ ( sqrt( 2.0*M_PI ) * stdev ));


    //cout << "r:" << r <<", x:" << x << ", var:" << var << endl;

    return r;
}

double MontecarloLocalization::sample_normal_distribution( double var )
{
    double  r;
    int     i;

    r=0;

    for( i=0; i<12; i++ )
    {
        r += (drand48()*2*var) - var;
    }
    r = r * 0.5;

    return r;
}



Vector2D MontecarloLocalization::modelo_aceleracion( double power,
                                                     double dir,
                                                     double stamina,
                                                     double effort,
                                                     double body_angle
                                                     )
{

    ServerParam & param     = game_data->game_parameter.server_param;
    Vector2D    aceleracion;
    /* parámetros servidor... */
    double dashAngleStep    = param.dash_angle_step;
    double minDashPower     = param.min_dash_power;
    double maxDashPower     = param.max_dash_power;
    double minDashAngle     = param.min_dash_angle;
    double maxDashAngle     = param.max_dash_angle;
    double extraStamina     = param.extra_stamina;
    double sideDashRate     = param.side_dash_rate;
    double backDashRate     = param.back_dash_rate;
    double dashPowerRate    = param.dash_power_rate;
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

    return aceleracion;

}

Vector2D MontecarloLocalization::modelo_aceleracion_gomez( double dash_power,
                                                           double stamina,
                                                           double effort,
                                                           double body_angle)
{
    ServerParam & param     = game_data->game_parameter.server_param;
    /*double dashAngleStep    = param.dash_angle_step;
    double minDashPower     = param.min_dash_power;
    double maxDashPower     = param.max_dash_power;
    double minDashAngle     = param.min_dash_angle;
    double maxDashAngle     = param.max_dash_angle;
    double extraStamina     = param.extra_stamina;
    double sideDashRate     = param.side_dash_rate;
    double backDashRate     = param.back_dash_rate;*/
    double dashPowerRate    = param.dash_power_rate;

    double act_power;
    Vector2D aceleracion;
    act_power = effort*dash_power;
    aceleracion.x = act_power*dashPowerRate*cos( Rad2Deg(body_angle) );
    aceleracion.y = act_power*dashPowerRate*sin( Rad2Deg(body_angle) );
    return aceleracion;
}
