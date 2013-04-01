#ifndef ANALYTICAL_ABILITY_H
#define ANALYTICAL_ABILITY_H
#include "utilities.h"
#include "geometry.h"
#include "gameCommand.h"


#include <cstdio>
#include <cmath>
#include <iostream>

Vector2D dePrimera(  double vel_r_t1,
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
                     double kickable_margin = 0.7,
                     double kick_power_rate = 0.027,
                     double decay = 0.94)
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

    return kick_vector;
}


Vector2D velToInterceptBall( Vector2D const & b, // Posición del balón
							 Vector2D const & p, // Posición del jugador
							 Vector2D const & v, // Velocidad del balón
							 int t,
							 double ball_decay = 0.94 // constante de decaimiento
							)
{
    // Regresa la velocidad en x y en y necesarias para interceptar el baĺón en el tiempo t

    Vector2D vel_needed;
    double ln_decay = log( ball_decay );
    double decay_ala_t = std::pow(ball_decay, t);

        vel_needed.x = ( b.x - p.x + v.x * ( decay_ala_t - 1 )  / ln_decay )
                		/
                		(double)t;
        vel_needed.y = ( b.y - p.y + v.y * ( decay_ala_t - 1 )  / ln_decay )
                		/
                		(double)t;

    return vel_needed;
}

double ActualKickPowerRate(int dir,double dist)
{
    double kick_power_rate = 0.027,kickable_margin = 1.0;
    double act_kpr = kick_power_rate * (( 1.0 - ((0.25 * dir) / 180.0)) - ((0.25 * dist) / kickable_margin));

    return act_kpr;
}

double KickPowerForSpeed(double speed,double actkpr)
{
    double kickPower = speed / actkpr;

    return kickPower;
}

//     KickSpeedToTravel(distancia Player-Objetivo , Vel final Deseada)
double KickSpeedToTravel(double d,double e)
{
    if( e == 0 )
       {
        e = 0.01;
       }
    double ball_decay = 0.94;
    double r = 1.0 / ball_decay;
    double n = log(d*(r - 1.0)/e + 1.0 ) / log(r);
    double s = e / pow(ball_decay,n);

    return s;   // Vel inicial necesaria para que cumpla la Vel final deseada
}





Vector2D PasePunto( double xTarget,    // punto final
                    double yTarget,
                    double x,          // coordenadas agente
                    double y,
                    int    angle,      // angulo del cuerpo
                    double velDes,     // velocidad final
                    double xBall,      // coordenadas de la pelota
                    double yBall)
{
Vector2D Kick;
double disToPoint;
double disBP;
double speed;
double anguloRel;
double momBP;
double act_kpr;
                                     //Distancia del jugador al punto final del balón
disToPoint = sqrt(  (xTarget - x)*(xTarget - x) +
                    (yTarget - y)*(yTarget - y)
                 );

                                    //Distancia del jugador al balón
disBP      = sqrt((xBall - x)*(xBall - x) +
                  (yBall - y)*(yBall - y));

speed      = KickSpeedToTravel(disToPoint,velDes);    //velocidad inicial necesaria

anguloRel  = Rad2Deg( atan2(yTarget - y,xTarget - x) )- angle;  //Ángulo para kick

momBP      = Rad2Deg( atan2(yBall - y,xBall - x) ) - angle; //Ángulo entre posicíon del balon y jugador

act_kpr    = ActualKickPowerRate(momBP,disBP);        //valor real del poder del kick

Kick.x  = rint(KickPowerForSpeed(speed,act_kpr));  //Kick.power  = Kick.x

if(Kick.x > 100.0)
    Kick.x = 100.0;

Kick.y = rint( anguloRel );   //Kick.moment = Kick.y

return Kick;
}


void GoToXY ( double   xTarget ,
              double   yTarget ,
              double   x,
              double   y,
              double   angle,
              double   radio,
              Vector2D velocidad,
              AgentCommand * command
            )
{

    double  disToPoint;
    double  dirToPoint;
    double  angPermisible;
    double  turnParameter;
    double  dashParameter;
    double const dash_power_rate = 0.006, effort = 0.8;
    double const inertia_moment = 5.0;

   //anguloCuerpo =  angle + angleCuelloCuerpo; // angleCuelloCuerpo se obtiene del sense_body
   //consideramos el angle como el angulo del cuerpo

    disToPoint = sqrt( (xTarget - x)*(xTarget - x)  + (yTarget - y)*(yTarget - y) );
    dirToPoint = Rad2Deg(atan2( yTarget - y , xTarget - x));

    if( disToPoint > radio )  // el agente no ha llegado al punto
       {

        angPermisible = Rad2Deg( atan2( radio , disToPoint ) );
        turnParameter = dirToPoint - angle;
        turnParameter = entre180( turnParameter );

        if( angPermisible < turnParameter * 0.1  &&  disToPoint > 15.0 )
            angPermisible = 25.0;

        if( fabs(turnParameter) > angPermisible )  // el agente no esta bien alineado al punto
           {

            velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y) );
            turnParameter =  turnParameter *(1.0 + (inertia_moment*velocidad.normita()));
            turnParameter = entre180(turnParameter);
            command->append_turn(turnParameter);

           }
        else
           {

            velocidad     = Vector2D::fromPolar( velocidad.x, Deg2Rad(velocidad.y - angle) );
            dashParameter = (disToPoint - velocidad.x)  / ( dash_power_rate * effort );

            if( dashParameter > 100.0)
                dashParameter = 100.0;

            command->append_dash( dashParameter );
           }
       }
    else
       {
         // el agente llego al punto
       }

}

void runWithBall (double   xTarget ,
                  double   yTarget ,
                  double   x,
                  double   y,
                  double   angle,
                  double   xBall,
                  double   yBall,
                  double   disBall,
                  Vector2D velocidad,
                  double   radio,
                  AgentCommand * command
                 )
{
    double angToPoint;
    double auxX,auxY;
    Vector2D Kick;

    if( disBall <= radio )
    {

        angToPoint = Rad2Deg( atan2(  yTarget   - y,   xTarget   - x) )- angle;

        /*Este 5.0 es similar a lo que queremos que adelante la pelota
        para valores grandes la adelanta mucho*/

        auxX = 5.0 * cos( Deg2Rad( angToPoint ) );
        auxY = 5.0 * sin( Deg2Rad( angToPoint ) );

        Kick = PasePunto(x+auxX,y+auxY,x,y,angle,0.0, xBall , yBall );

        Kick.y = angToPoint;

        command->append_kick(Kick.x,Kick.y);
    }
    else
    {
        GoToXY(xBall,yBall,x,y,angle,radio,velocidad,command );  // vamos por la pelota
    }
}

#endif //ANALYTICAL_ABILITY_H
    
