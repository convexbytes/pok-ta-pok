#ifndef GEOMETRY_H
#define GEOMETRY_H
#define M_PI 3.1415926535897932
#include <cmath>
#include <limits>
#include <iostream>
#include <vector>

class Vector2D;
class Point2D;
class Line2D;

class Vector2D
{
public:

    double x;
    double y;

    Vector2D();
    Vector2D( double x, double y );
    Vector2D( double a );

    void set( double x, double y );

    void 		operator=( Vector2D const & v);
    void 		operator=( double const a );
    Vector2D  	operator+(const Vector2D & v1) const;
    void 		operator+=(const Vector2D & v1);
    Vector2D 	operator-( const Vector2D & v ) const;
    Vector2D 	operator/( double r ) const;
    void 		operator/=( double r );
    Vector2D 	operator *( double r) const;
    void 		operator*=( double r );

    double 		normita	   () const;

    double 		prodPunto   ( Vector2D const & v ) const;
    //double 		distance    ( Vector2D const & v ) const; // Verificar fórmula!
    double 		angleBetween( Vector2D const & v ) const; // verificar fórmula!
    double 		angle	   () const;

    void 		normalize();

    void 		rotate( double angle_rad );

    double 	  	magSquared  () const; // Magnitud al cuadrado

    Vector2D  	projectionOver( Vector2D const & v ) const; // Proyección sobre v


    static Vector2D projection( Vector2D const & A, Vector2D const & B ); // Proyección de A sobre B

    static Vector2D fromPolar( const double & r, const double & ang );

    static Vector2D toPolar( const double x, const double y );

    friend std::ostream & operator <<( std::ostream & os, Vector2D const & v);
};

class Point2D
{
public:
    Point2D();
    Point2D( double x, double y);

    void set( double x, double y );

    double x() const;
    double y() const;

    bool operator==( Point2D const & p ) const;
    void operator= ( Point2D const & p );
    void operator /= ( double a);
    friend std::ostream & operator << ( std::ostream & os, Point2D const & p );

    Point2D & midPointWidh( Point2D const & p ) const;
    double slopeBetween( Point2D const & p ) const;

    double M_x;
    double M_y;

};


class Line2D
{
public:
	Line2D();
	Line2D( double a, double b, double c);
	Line2D( Point2D const & p1, Point2D const & p2 );

	double a() const;
	double b() const;
	double c() const;

    void    construct   ( double a, double b, double c);
    void    construct   ( Point2D const & p1, Point2D const & p2 );
    void    construct   ( Point2D const & p, double slope );
    double  eval        ( Point2D const & p );

    bool   operator == ( Line2D const & l ) const;

    friend
    std::ostream & operator << ( std::ostream & os, Line2D const & l);

	bool intersect( Line2D const & l, Point2D * p ) const;
	bool isParallelWith( Line2D const & l) const;
private:
	// ax + by + c = 0;
	double M_a;
	double M_b;
	double M_c;
};

bool
edgeIntersectLine( Line2D const & l,
                   Point2D const & edge_p1, // punto1 del segmento
                   Point2D const & edge_p2, // punto2 del segmento
                   Point2D * q );  // Regresa en q el punto de intersección (si lo hay)



void
intersectLinePolygon( std::vector<Point2D> const & polygon,
                           Line2D const & line,
                           std::vector<Point2D> * out1_container,
                           std::vector<Point2D> * out2_container
                           );



Line2D &
pointsBisector( Point2D const & p, Point2D const & q);



bool
insideConvexPolygon( Point2D const & p, std::vector<Point2D> const & polygon );



void
constructVoronoiCell( Point2D  & site,						// Sitio donde se genera la celda
                      std::vector<Point2D> & sites,			// Sitios generadores
                      std::vector<Point2D> & initial_cell,  // Celda inicial
                      std::vector<Point2D> * cell_container // Aquí se guarda la celda voronoi resultante
                      );



Vector2D // Regresa el punto en el segmento p1p2 que es más cercano a q
closestPointInSegment( Vector2D const & q,
					   Vector2D const & p1, // primer punto que define el segmento
					   Vector2D const & p2  // segundo punto que define el segmento
					   );

#endif
