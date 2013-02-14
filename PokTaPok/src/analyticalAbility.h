#ifndef ANALYTICAL_ABILITY_H
#define ANALYTICAL_ABILITY_H
#include "utilities.h"
#include "geometry.h"

#include <math.h>
#include <iostream>

Vector2D dePrimera( double vel_r_t1,
                      double vel_angle_t1,
                      double vel_r_t0,
                      double vel_angle_t0,
                      double ball_distance, // La dirección a la que vemos el balón
                      double ball_direction,
                      double neck_angle,
                      double kickable_margin,
                      double kick_power_rate,
                      double decay)

{
    Vector2D kick_vector;

    double dir_diff = fabs( ball_direction - neck_angle );

    double f = (1 - 0.25*(dir_diff/180.0) -0.25*(ball_distance/kickable_margin) );

    Vector2D vt1( vel_r_t1* cos( Deg2Rad( vel_angle_t1 ) ),
                  vel_r_t1* sin( Deg2Rad( vel_angle_t1 ) )  );

    Vector2D vt0( vel_r_t0* cos( Deg2Rad( vel_angle_t0 ) ),
                  vel_r_t0* sin( Deg2Rad( vel_angle_t0 ) )  );

    double pow_needed;
    double angle_needed;

    if( vt1.x == 0.0 && vt1.y == 0.0 && vt0.x == 0.0 && vt0.y == 0.0)
    {
        pow_needed = 0.0;
        angle_needed = 0.0;
    }
    else
    {
         pow_needed = (vt1/decay - vt0).normita() / ( kick_power_rate * f ) ;
         angle_needed = atan2( vt1.y/decay - vt0.y, vt1.x/decay - vt0.x );
    }

    kick_vector.x = pow_needed;
    kick_vector.y = Rad2Deg( angle_needed );

    return kick_vector;

}

Vector2D freezeBall( double vel_r_t0,
                     double vel_angle_t0,
                     double ball_distance,
                     double ball_direction,
                     double neck_angle,
                     double kickable_margin,
                     double kick_power_rate,
                     double decay)
{
    Vector2D kick_vector; // kick_vector.x = power, kick_vector.y = angle

    kick_vector = dePrimera( 0.0,
                             0.0,
                             vel_r_t0,
                             vel_angle_t0,
                              ball_distance,
                              ball_direction,
                              neck_angle,
                              kickable_margin,
                              kick_power_rate,
                              decay);
    /*
( double vel_r_t1,
                      double vel_angle_t1,
                      double vel_r_t0,
                      double vel_angle_t0,
                      double ball_distance, // La dirección a la que vemos el balón
                      double ball_direction,
                      double neck_angle,
                      double kickable_margin,
                      double kick_power_rate,
                      double decay)

      */
    return kick_vector;
}


Vector2D ballAbsoluteInterceptionModel( double x0_ball, // posición x del balón en el tiempo 0
                                          double y0_ball, // posición y del balón en el tiempo 0
                                          double x0_agent, // posición x del agente en el tiempo 0
                                          double y0_agent, // posición y del agente en el tiempo 0
                                          double vx0_ball, // velocidad inicial x del balón
                                          double vy0_ball, // velocidad inicial y del balón
                                          double decay, // constante de decaimiento
                                          int t
                                          )
{
    // Regresa la velocidad en x y en y necesarias para interceptar el baĺón en el tiempo t

    Vector2D vel_needed;
    // La constante de ajuste sirve para contrarrestar el tiempo que tarda en acelerar a su max. vel
    // con 0.9 de poder
    static double const acceleration_fitting_const = 0.72;
    double ln_decay = log( decay );
    //double pow_dec_t_ln;
    //for( i=1; )


        double vx_needed = ( x0_ball - x0_agent + vx0_ball * ( pow(decay, t) - 1 )  / ln_decay )
                /
                (double)t;
        double vy_needed = ( y0_ball - y0_agent + vy0_ball * ( pow(decay, t) - 1 )  / ln_decay )
                /
                (double)t;


        //vel_needed.x /= ( effort * dash_power_rate );
        //vel_needed.y /= ( effort * dash_power_rate );


        // Hacemos ajuste debido al ciclo que tarda en girar, y a los dos ciclos que tarda en "estabilizar"
        // la velocidad para llegar al máximo
        vel_needed.x = ( (t - 2*acceleration_fitting_const) * vx_needed ) / (t-3);
        vel_needed.y = ( (t - 2*acceleration_fitting_const) * vy_needed ) / (t-3);

        //vel_needed.x = vx_needed;
        //vel_needed.y = vy_needed;
        //std::cout << t << std::endl;

    return vel_needed;
}

Vector2D ballRelativeInterceptionModel( double ball_distance,
                                        double ball_direction,
                                        double neck_angle,
                                        double ball_dis_vel,
                                        double ball_dir_vel,
                                        double decay,
                                        int t)
{
    // Regresa el la magnitud y la dirección de la velocidad necesarias para interceptar
    // el balón al tiempo t

    Vector2D vel_needed;

    vel_needed = ballAbsoluteInterceptionModel( ball_distance * cos( ball_direction - neck_angle),
                                                ball_distance * sin( ball_direction - neck_angle),
                                                0.0,
                                                0.0,
                                                ball_dis_vel * cos( ball_dir_vel ),
                                                ball_dis_vel * sin( ball_dir_vel ),
                                                decay,
                                                t);

    return Vector2D::toPolar( vel_needed.x, vel_needed.y );


}



#endif //ANALYTICAL_ABILITY_H
    
