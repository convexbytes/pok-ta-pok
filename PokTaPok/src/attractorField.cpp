#include "attractorField.h"

Vector2D & fieldPorteriaIzquierda( Vector2D const & v )
{
    static Vector2D result;
    double denominador;
    result = 0.0;
    if( v.x != 0.0 && v.y != 0.0 )
    {
        denominador = sqrt(   ( v.x - centroPorteriaIzquierda.x ) * ( v.x - centroPorteriaIzquierda.x )
                            + ( v.y - centroPorteriaIzquierda.y ) * ( v.y - centroPorteriaIzquierda.y )
                          );

        result.x = repulsionMeta * (v.x - centroPorteriaIzquierda.x ) / denominador;
        result.y = repulsionMeta * (v.y - centroPorteriaIzquierda.y ) / denominador;
    }

    return result;
}


Vector2D & fieldPorteriaDerecha( Vector2D const & v )
{
    static Vector2D result;
    double denominador;
    result = 0.0;
    if( v.x != 0.0 && v.y != 0.0 )
    {
        denominador = sqrt(   ( v.x - centroPorteriaDerecha.x ) * ( v.x - centroPorteriaDerecha.x )
                            + ( v.y - centroPorteriaDerecha.y ) * ( v.y - centroPorteriaDerecha.y )
                          );

        result.x = repulsionMeta * (v.x - centroPorteriaDerecha.x ) / denominador;
        result.y = repulsionMeta * (v.y - centroPorteriaDerecha.y ) / denominador;
    }

    return result;
}

Vector2D & fieldVectorFieldL( Vector2D const & v )
{
    static Vector2D result;
    result = 0.0;
    double denominador;

    // Para permanecer dentro de la cancha
    if( v.x != lineaIzquierda )
        result.x += repulsionLinea  / (v.x - lineaIzquierda );

    if( v.x != lineaDerecha )
        result.x += repulsionLinea / (v.x - lineaDerecha );

    if( v.y != lineaArriba )
        result.y += repulsionLinea / (v.y - lineaArriba );

    if( v.y != lineaAbajo )
        result.y += repulsionLinea / (v.y - lineaAbajo );

    // Para ir hacia la portería contraria
    result += fieldPorteriaDerecha( v );

    return result;
}

Vector2D & fieldVectorFieldR( Vector2D const & v )
{
    static Vector2D result;
    Vector2D aux;
    result = 0.0;
    double denominador;

    // Para permanecer dentro de la cancha
    if( v.x != lineaIzquierda )
        result.x += repulsionLinea  / (v.x - lineaIzquierda );

    if( v.x != lineaDerecha )
        result.x += repulsionLinea / (v.x - lineaDerecha );

    if( v.y != lineaArriba )
        result.y += repulsionLinea / (v.y - lineaArriba );

    if( v.y != lineaAbajo )
        result.y += repulsionLinea / (v.y - lineaAbajo );

    // Para ir hacia la portería contraria
    result += fieldPorteriaIzquierda( v );

    return result;
}




Vector2D & fieldRepulsorRival( Vector2D const & v,
                       Vector2D const & rival)
{
    static Vector2D result;
    result = 0.0;
    if( v.x != rival.x && v.y != rival.y )
    {
        double denominador = pow(   (v.x - rival.x)*(v.x - rival.x)
                                  + (v.y - rival.y)*(v.y - rival.y)
                                  , 1.5);
        result.x = repulsionRival * (v.x - rival.x) / denominador;
        result.y = repulsionRival * (v.y - rival.y) / denominador;

        return result;
    }
}

Vector2D & fieldAttractorRival( Vector2D const & v,
                       Vector2D const & rival)
{
    static Vector2D result;
    result = 0.0;
    if( v.x != rival.x && v.y != rival.y )
    {
        double denominador = pow(   (v.x - rival.x)*(v.x - rival.x)
                                  + (v.y - rival.y)*(v.y - rival.y)
                                  , 1.5);
        result.x = repulsionRival * (v.x - rival.x) / denominador;
        result.y = repulsionRival * (v.y - rival.y) / denominador;

        result *= -1.0;

        return result;
    }
}

Vector2D & fieldAttractorBall( Vector2D const & v,
                               Vector2D const & ball)
{

    static Vector2D result;
    result = 0.0;
    if( v.x != ball.x && v.y != ball.y )
    {
        double denominador = pow(   (v.x - ball.x)*(v.x - ball.x)
                                  + (v.y - ball.y)*(v.y - ball.y)
                                  , 1.5);
        result.x = repulsionBall * (v.x - ball.x) / denominador;
        result.y = repulsionBall * (v.y - ball.y) / denominador;

        result *= -1.0;

        return result;
    }
}
