#ifndef ATTRACTOR_FIELD_H
#define ATTRACTOR_FIELD_H

#include "geometry.h"

Vector2D const adelanteL( 1.0, 0.0 );
Vector2D const adelanteR( -1.0, 0.0 );

Vector2D const centroPorteriaDerecha( 45.0, 0.0 );
Vector2D const centroPorteriaIzquierda( -45.0, 0.0 );

double const lineaIzquierda = -52.5;
double const lineaDerecha = 52.5;
double const lineaArriba = -30.5;
double const lineaAbajo = 30.5;

double const repulsionLinea = 5.0;
double const repulsionRival = 50.0;
double const repulsionMeta = -5.0;
double const repulsionBall = -5.0;

Vector2D & fieldPorteriaIzquierda( Vector2D const & v );

Vector2D & fieldPorteriaDerecha( Vector2D const & v );

Vector2D & fieldVectorFieldL( Vector2D const & v );

Vector2D & fieldVectorFieldR( Vector2D const & v );

Vector2D & fieldRepulsorRival( Vector2D const & v,
                       Vector2D const & rival);

Vector2D & fieldAttractorRival( Vector2D const & v,
                       Vector2D const & rival);

Vector2D & fieldAttractorBall( Vector2D const & v,
                               Vector2D const & ball);

#endif

