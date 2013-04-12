#include "localizationMethods.h"
#include "utilities.h"
#include "geometry.h"

Vector2D ubicacionBanderaBandera(double distanciaFlag1, double direccionFlag1, Vector2D coordGlobalFlag1, double distanciaFlag2, double direccionFlag2, Vector2D coordGlobalFlag2)
{
    // Devuelve un vector con la coordenada global en x
    // y la coordenada global y
    //
    double distancia, p1[2], a,b, dx, dy, h;
    Vector2D posicion;
    dx = coordGlobalFlag2.x - coordGlobalFlag1.x;
    dy = coordGlobalFlag2.y - coordGlobalFlag1.y;

    distancia = sqrt( dx*dx + dy*dy );


    a = ( distanciaFlag1*distanciaFlag1 - distanciaFlag2*distanciaFlag2 + distancia*distancia )/ ( 2.0*distancia );

    // cos() = dx/distancia;
    // sen() = dy/distancia;

    p1[0] = coordGlobalFlag1.x+ ( a*(dx/distancia) ) ;
    p1[1] = coordGlobalFlag1.y + ( a*(dy/distancia) ) ;


    h = sqrt( distanciaFlag1*distanciaFlag1 - a*a );

    if( isnan(h) )
    {
    	//a = ( -distanciaFlag1*distanciaFlag1 + distanciaFlag2*distanciaFlag2 + distancia*distancia )/ (-2.0*distancia );
    	//h = sqrt( distanciaFlag1*distanciaFlag1 - a*a );
    	h = 0.0;
    }

    posicion.x = ( p1[0] - (h*signo(direccionFlag2 - direccionFlag1)*(dy/distancia))   );
    posicion.y = ( p1[1] + (h*signo(direccionFlag2 - direccionFlag1)*(dx/distancia))   );
    //printf("Posición: %lf %lf\n",posicion.x,posicion.y);
    return posicion;
}

Vector2D ubicacionLineaBandera(double direccionLinea,
                               char lineaId,
                               double distanciaFlag,
                               double direccionFlag,
                               Vector2D coordGlobalFlag)
{
    // Devuelve un vector, con la coordenada global X
    // y la coordenada global Y
    // Linea puede tomar los valores r,l,t,b (right, left, top, bottom)
    //
    double B,O,angulox,anguloy;
    double coor[2];
    Vector2D p;
    B = (-1.0)*( signo(direccionLinea) )*( 90.0 - absoluto(direccionLinea) ); // Ángulo de la bisectriz a la
                                                                               // perpendicular de la linea
    O = orientation(lineaId) - B;              // Obtenemos el ángulo Global
    angulox = Deg2Rad( direccionFlag + O);      // Conversion de grados a radianes
    anguloy = Deg2Rad( direccionFlag + O);
    coor[0] = coordGlobalFlag.x - ( distanciaFlag * cos( angulox ) );    // Conversion de polares a cartesianas
    coor[1] = coordGlobalFlag.y - ( distanciaFlag * sin( anguloy ) );

    p.x = coor[0];
    p.y = coor[1];

    return p;
}


double orientacion_global(Vector2D coordGlobalFlag, Vector2D coordGlobalAgente, double angulo_relativo )
{

    double   theta;
    double  numerador, denominador;
    numerador   = coordGlobalFlag.y - coordGlobalAgente.y;
    denominador = coordGlobalFlag.x - coordGlobalAgente.x;

    theta = atan2(numerador,denominador);
   /* if ( denominador > 0.0 )
        theta = atan( numerador/denominador );

    else if ( denominador < 0.0 )
        theta = signo( numerador ) * ( M_PI - atan( absoluto( numerador/denominador ) ) );

    else if ( denominador == 0.0 && numerador != 0.0 )
        theta = signo( numerador ) * ( M_PI / 2.0 );

    else
        theta = 0.0;
    */

    theta = entre180( Rad2Deg( theta ) - angulo_relativo );

    return theta;
}

double orientation(char a)
{
    double r;

  switch(a)
    {
      case 'r': r=0.0;   break;  //right
      case 'b': r=90.0;  break;  //bottom
      case 'l': r=180.0; break;  //left
      case 't': r=-90.0; break;  //top
      default:  printf("Error en id de linea.\n"); r=0; break;
    }
   return r;
}

