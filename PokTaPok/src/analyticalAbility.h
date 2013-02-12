#ifndef ANALYTICAL_ABILITY_H
#define ANALYTICAL_ABILITY_H
#include "utilities.h"
#include "geometry.h"

#include <cmath>
#include <iostream>

Vector2D dePrimera( double vel_r_t1,
                      double vel_angle_t1,
                      double vel_r_t0,
                      double vel_angle_t0,
                      double ball_direction, // La dirección a la que vemos el balón
                      double ball_distance,
                      double neck_angle,
                      double kickable_margin,
                      double kick_power_rate,
                      double decay)

{
    static Vector2D angle_power;
    double & k1 = kickable_margin;
    double & k2 = kick_power_rate;
    double & k3 = decay;

    double dir_diff = fabs( ball_direction - neck_angle );

    double tan_upper = vel_r_t1 * sin( vel_angle_t1 ) - k3 * vel_r_t0 * sin( vel_angle_t0 );
    double tan_lower = vel_r_t1 * cos( vel_angle_t1 ) - k3 * vel_r_t0 * cos( vel_angle_t0 );


    double power;
    double kick_angle;

    kick_angle = atan2( tan_upper, tan_lower );



    if( kick_angle != 0 )
    {
        angle_power.x = kick_angle;

        power = tan_upper /
                ( ( 1 - 0.25 * ( dir_diff/180.0 + ball_distance/k1 ) ) * sin( kick_angle) * k2 * k3 );
    }
    else
    {
        power = kick_angle = 1000000; // Temporalmente manejamos este número como indicador de error
    }

    angle_power.x = power;
    angle_power.y = kick_angle;

    return angle_power;

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
        std::cout << t << std::endl;

    return vel_needed;
}


#endif //ANALYTICAL_ABILITY_H
    // Ajuste de aceleración, el modelo original supone una velocidad constante
