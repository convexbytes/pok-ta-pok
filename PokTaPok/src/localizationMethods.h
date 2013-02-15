#ifndef METODOSUBICACION_H
#define METODOSUBICACION_H
#include "geometry.h"
#include "utilities.h"
#include <math.h>
#include <stdio.h>

Vector2D ubicacionBanderaBandera(float distanciaFlag1, float direccionFlag1, Vector2D coordGlobalFlag1, float distanciaFlag2, float direccionFlag2, Vector2D coordGlobalFlag2 );
Vector2D ubicacionLineaBandera(float direccionLinea, char lineaId,float distanciaFlag, float direccionFlag, Vector2D coordGlobalFlag );
double  orientacion_global(Vector2D coordGlobalFlag,Vector2D coordGlobalAgente, double angulo_relativo );
double  body_angle_loc(int head_angle, Vector2D global_agent,  Vector2D coordGlobalFlag, int sensed_flag_direction );
int orientation(char a);
#endif
