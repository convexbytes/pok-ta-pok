#ifndef ATTRACTOR_FIELD_H
#define ATTRACTOR_FIELD_H
#include "geometry.h"

static Vector2D const adelante( 1.0, 0.0 );
static Vector2D const centroPorteriaContraria( 45.0, 0.0 );
double const lineaIzquierda = -52.5;
double const lineaDerecha = 52.5;
double const lineaArriba = -30.5;
double const lineaAbajo = 30.5;
double const repulsionLinea = 5.0;
Vector2D & fieldVectorFieldL( Vector2D const & v )
{
    static Vector2D result;
    result = 0.0;
    double denominador;

    if( v.x != lineaIzquierda )
        result.x += repulsionLinea  / (v.x - lineaIzquierda );

    if( v.x != lineaDerecha )
        result.x += repulsionLinea / (v.x - lineaDerecha );





    denominador


}

Vector2D & fieldVectorFieldR( Vector2D const & v )
{

}

#endif

