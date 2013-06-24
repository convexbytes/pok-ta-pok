#ifndef METODOSUBICACION_H
#define METODOSUBICACION_H
#include "geometry.h"
#include "utilities.h"
#include <math.h>
#include <stdio.h>

Vector2D ubicacionBanderaBandera(double distanciaFlag1, double direccionFlag1, Vector2D coordGlobalFlag1, double distanciaFlag2, double direccionFlag2, Vector2D coordGlobalFlag2 );
Vector2D ubicacionLineaBandera(double direccionLinea, char lineaId,double distanciaFlag, double direccionFlag, Vector2D coordGlobalFlag );
double  orientacion_global(Vector2D coordGlobalFlag,Vector2D coordGlobalAgente, double angulo_relativo );
double  body_angle_loc(int head_angle, Vector2D global_agent,  Vector2D coordGlobalFlag, int sensed_flag_direction );
double orientation(char a);
#endif // METODOSUBICACION_H
