#include "montecarloLocalization.h"
#include "localizationMethods.h"
#include "utilities.h"
#include <stdlib.h>
#include <math.h>
#include <cmath>
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
    /*
    for( i=0; i<NUM_PARTICULAS ; i++   )
    printf( "x:%lf, y:%lf, theta:%lf \n",
            particulas[i].get_x(),
            particulas[i].get_y(),
            particulas[i].get_theta()
            );
*/


}

void MontecarloLocalization::montecarlo_correction( Particula   *particulas,
                                                    vector<Flag> *banderas,
                                                    Particula *particulas_nuevas
                                                    )
{
    int     i, j, banderas_vistas;
    int     posicion_peso_max;
    double  normalizador;
    double  draw;
    double  peso[NUM_PARTICULAS];
    double  prob_particulas[NUM_PARTICULAS];
    double  rango[NUM_PARTICULAS];
  //  srand48( time(NULL) );

    Flag    bandera;

    normalizador    = 0.0;
    draw            = 0.0;
    banderas_vistas = banderas->size();


    for( i=0; i<NUM_PARTICULAS; i++ ){
             peso[i] = 0.0;
            }

    for( i=0; i<NUM_PARTICULAS; i++ )
    {
        for( j=0; j<banderas_vistas; j++)
        {
            bandera = banderas->at(j);
            peso[i] = peso[i] + Landmark_Model_Known_Corresponce( bandera, particulas[i] );
        }
        normalizador += peso[i];
    }
    /*************** Correction Step  *****************/
    for( i=0; i<NUM_PARTICULAS; i++ )
    {
        prob_particulas[i] = peso[i] / normalizador;
    }

    for( i=0; i<NUM_PARTICULAS; i++ )
    {
        if( i==0 )
            rango[i] = 0.0 + prob_particulas[i];
        else
            rango[i] = rango[i-1] + prob_particulas[i];
    }

    /************ Draw Step *********************/
    for( i=0; i<NUM_PARTICULAS; i++ )
    {
        draw = drand48();
        for( j=0; j<NUM_PARTICULAS; j++ )
        {
            if( j==0 )
            {
                if( draw >= 0.0 && draw < rango[j] )
                {
                    particulas_nuevas[i].set_x      ( particulas[i].get_x() );
                    particulas_nuevas[i].set_y      ( particulas[i].get_y() );
                    particulas_nuevas[i].set_theta  ( particulas[i].get_theta() );
                }
            }
            else
            {
                if( draw >= rango[j-1] && draw < rango[j] )
                {
                    particulas_nuevas[i].set_x      ( particulas[i].get_x() );
                    particulas_nuevas[i].set_y      ( particulas[i].get_y() );
                    particulas_nuevas[i].set_theta  ( particulas[i].get_theta() );
                }
            }
        } //for j
    } // for i



   /*for( i=0; i<NUM_PARTICULAS; i++ )
    {
        printf( "***x:%lf, y:%lf, theta:%lf peso[%d] = %lf  rango[%d] = %lf\n",
                particulas_nuevas[i].get_x(),
                particulas_nuevas[i].get_y(),
                particulas_nuevas[i].get_theta(),
                i,
                peso[i],
                i,
                rango[i]
                );
    }
*/
/*
    printf( "x:%lf, y:%lf, theta:%lf \n",
            particulas_nuevas[posicion_peso_max].get_x(),
            particulas_nuevas[posicion_peso_max].get_y(),
            particulas_nuevas[posicion_peso_max].get_theta()
            );
   */

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

    q_step_l = game_data->game_parameter.server_param.quantize_step_l;
    distancia_x = bandera.get_global_coord().x - pose.get_x();
    distancia_y = bandera.get_global_coord().y - pose.get_y();

    r_aprox = sqrt( (distancia_x * distancia_x) + (distancia_y * distancia_y)  );

    numerador   = bandera.get_global_coord().y - pose.get_y();
    denominador = bandera.get_global_coord().x - pose.get_x();

    if ( denominador > 0.0 )
        phi_aprox = atan( numerador/denominador );

    else if ( denominador < 0.0 )
        phi_aprox = signo( numerador ) * ( MONT_PI - atan( absoluto( numerador/denominador ) ) );

    else if ( denominador == 0.0 && numerador != 0.0 )
        phi_aprox = signo( numerador ) * ( MONT_PI / 2.0 );

    else
        phi_aprox = 0.0;

    phi_aprox = phi_aprox - grad2rad( pose.get_theta() );

    r_min = exp( (rint ( log(  (rint( bandera.distance/ 0.1 ) - 0.5 ) * 0.1 ) / q_step_l ) - 0.5 ) * q_step_l );
    r_max = exp( (rint ( log(  (rint( bandera.distance/ 0.1 ) + 0.5 ) * 0.1 ) / q_step_l ) + 0.5 ) * q_step_l );

    desviacion_r = r_max - r_min;

    phi_min = (rint(bandera.direction / 1.0) - 0.5) * 1.0;
    phi_max = (rint(bandera.direction / 1.0) + 0.5) * 1.0;

    desviacion_phi = phi_max - phi_min;

    probabilidad = prob_normal_distribution( bandera.distance - r_aprox, desviacion_r ) * prob_normal_distribution( grad2rad( bandera.direction ) - phi_aprox, desviacion_phi );


//    printf( "Diferencia %lf \n",bandera.distance - r_aprox);

  //  printf( "Probabilidad %lf \n",probabilidad);


    return probabilidad;
}

Particula MontecarloLocalization::Sample_Motion_Model( Control      U,
                                                       Particula    particula
                                                       )
{/*
    double effort;
    double act_power;
    double body_dis_vel;
    double ax, ay, x_vel, y_vel,r1,r2,ux,uy;
    double dash_power_rate;
    double rmax;
    double turn_aprox, vel_aprox;
    double rand;

    Particula X;

    effort          = game_data->obs_handler.last_sense.stamina_effort;
    dash_power_rate = game_data->game_parameter.server_param.dash_power_rate;
    body_dis_vel    = game_data->obs_handler.last_sense.speed_amount;
    rand            = game_data->game_parameter.server_param.player_rand;


    if( U.turn_angle != 0.0 && body_dis_vel == 0.0)
      {
        X.set_x(  particula.get_x() );
        X.set_y(  particula.get_y() );
        X.set_theta( entre180( U.turn_angle + sample_normal_distribution(2.0)  ));
      }
    else if(U.turn_angle == 0.0 && body_dis_vel != 0.0)
      {
        act_power = effort * U.dash_power;
        ax = act_power * dash_power_rate * cos( grad2rad( particula.get_theta() ) );
        ay = act_power * dash_power_rate * sin( grad2rad( particula.get_theta() ) );

        x_vel = body_dis_vel*cos( grad2rad(particula.get_theta()) );
        y_vel = body_dis_vel*sin( grad2rad(particula.get_theta()) );

        rmax = rand *  sqrt(  ( x_vel + ax) * (x_vel + ax) +
                              ( y_vel + ay) * (y_vel + ay)
                            );
        r1 = ( drand48() * (2.0*rmax) ) - rmax;
        r2 = ( drand48() * (2.0*rmax) ) - rmax;

        ux = x_vel + ax + r1;
        uy = y_vel + ay + r2;

        X.set_x( particula.get_x() + sqrt(ux*ux + uy*uy) * cos(particula.get_theta()));
        X.set_y( particula.get_y() + sqrt(ux*ux + uy*uy) * sin(particula.get_theta()));
        X.set_theta( particula.get_theta() );
      }
    else if(U.turn_angle != 0.0 && body_dis_vel != 0.0 )
      {
        vel_aprox  = body_dis_vel + sample_normal_distribution(.01) ;
        turn_aprox = U.turn_angle + sample_normal_distribution(.001) ;
        X.set_x( particula.get_x() - (vel_aprox / turn_aprox) * sin(particula.get_theta()) + (vel_aprox / turn_aprox) * sin(particula.get_theta() + U.turn_angle) );
        X.set_y( particula.get_x() + (vel_aprox / turn_aprox) * cos(particula.get_theta()) - (vel_aprox / turn_aprox) * cos(particula.get_theta() + U.turn_angle) );
        X.set_theta( particula.get_theta() + U.turn_angle );
      }
*/


    double act_power;
    double act_ang = 0.0, act_ang_min, act_ang_max;
    double ax, ay;
    double r1, r2, rmax, r;
    double ux = 0.0, uy = 0.0;
    double effort;
    double dash_power_rate;
    double inertia_moment;
    double body_dis_vel;
    double body_dir_vel;
    double rand;
    double x_vel;
    double y_vel;
    Particula X;

    dash_power_rate = game_data->game_parameter.server_param.dash_power_rate;
    effort          = game_data->obs_handler.last_sense.effort;
    body_dis_vel    = game_data->obs_handler.last_sense.speed_amount;
    body_dir_vel    = game_data->obs_handler.last_sense.speed_direction;
    rand            = game_data->game_parameter.server_param.player_rand;
    inertia_moment = game_data->game_parameter.server_param.inertia_moment;



    if(  U.turn_angle != 0.0 && body_dis_vel == 0.0  )
    {
        r = ( drand48() * (2.0*rand) ) - rand;
        act_ang_min = ( (1.0 - r) * U.turn_angle );
        act_ang_max = ( (1.0 + r) * U.turn_angle );
        act_ang = drand48()*(act_ang_min-act_ang_max) + act_ang_min;
    }

    else if( U.turn_angle == 0.0 && body_dis_vel != 0.0 )
    {
        act_power = effort * U.dash_power;
    //    printf("effort:%lf\tact_power:%lf\ttheta:%lf\n", effort, act_power, particula.get_theta() );
        ax = act_power * dash_power_rate * cos( grad2rad( particula.get_theta() ) );
        ay = act_power * dash_power_rate * sin( grad2rad( particula.get_theta() ) );

     //   printf("angulos %lf cos() %lf sin() %lf\n",particula.get_theta(), cos( grad2rad( particula.get_theta() )),sin( grad2rad( particula.get_theta() ) ));
      //  printf("ax:%lf\tay:%lf\t\n", ax, ay );
        x_vel = body_dis_vel*cos( grad2rad(body_dir_vel) );
        y_vel = body_dis_vel*sin( grad2rad(body_dir_vel) );

        //printf("x_vel:%lf\ty_vel:%lf\t\n", x_vel, y_vel );
        rmax = rand *  sqrt(  ( x_vel + ax) * (x_vel + ax) +
                               ( y_vel + ay) * (y_vel + ay)
                            );
        r1 = ( drand48() * (2.0*rmax) ) - rmax;
        r2 = ( drand48() * (2.0*rmax) ) - rmax;
    //    printf("r1:%lf\tr2:%lf\n", r1, r2 );
        ux = x_vel + ax + r1;
        uy = y_vel + ay + r2;
    }

    else if( U.turn_angle != 0.0 && body_dis_vel != 0.0 )
    {
        r = ( drand48() * (2.0*rand) ) - rand;
        act_ang_min = ( (1.0 - r) * U.turn_angle ) / (1.0 + inertia_moment*body_dis_vel );
        act_ang_max = ( (1.0 + r) * U.turn_angle ) / (1.0 + inertia_moment*body_dis_vel );

        act_ang = drand48()*(act_ang_min-act_ang_max) + act_ang_min;
    }


   X.set_x( particula.get_x() + ux );
   X.set_y( particula.get_y() + uy );
   X.set_theta(entre180(U.turn_angle + particula.get_theta()));
   printf("theta: %d\n",  X.get_theta() );
    return X;
}

double MontecarloLocalization::prob_normal_distribution( double x,
                                                         double stdev
                                                        )
{
    double r1, r2, r;

    r1 = 1.0/ ( sqrt( 2*MONT_PI ) * stdev );

    r2 = exp( (-0.5)*x*x/(stdev*stdev) );

    r  = r1 * r2;
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
                                                     char side,
                                                     double effort,
                                                     double body_angle,
                                                     double pos_y
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
    double slownessOnTopForLeft = param.slowness_on_top_for_left_team;
    double slownessOnTopForRight = param.slowness_on_top_for_right_team;

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

    double effective_dash_power = std::fabs( effort
                                             * power
                                             * dir_rate
                                             * dashPowerRate );

    if ( pos_y < 0.0 )
    {
        effective_dash_power /= ( side == 'l'
                                  ? slownessOnTopForLeft
                                  : slownessOnTopForRight );
    }

    if ( back_dash )
    {
        dir += 180.0;
    }

    aceleracion = Vector2D::fromPolar( effective_dash_power,
                              normalize_angle( body_angle + Deg2Rad( dir ) ) );

    return aceleracion;

}
