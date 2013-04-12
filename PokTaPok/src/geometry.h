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

    double normita	   () const;
    double prodPunto   ( Vector2D const & v ) const;
    double distance	   ( Vector2D const & v ) const;
    double angleBetween( Vector2D const & v ) const;
    double angle	   () const;

    void rotate( double angle_rad );

    friend std::ostream & operator <<( std::ostream & os, Vector2D const & v)
    {
    	os << "Vector2D(" << v.x << "," << v.y << ")";
    	return os;
    }

    static
    Vector2D fromPolar( const double & r,
                       const double & ang )
      {
        Vector2D vec( r * std::cos( ang ), r * std::sin( ang ) );

        return vec;
      }

    static
    Vector2D toPolar( const double x,
                      const double y)
    {
        Vector2D vec( sqrt( x*x + y*y ) , atan2( y, x ) );
        return vec;
    }
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

inline
bool
edgeIntersectLine( Line2D const & l,
                   Point2D const & edge_p1, // punto1 del segmento
                   Point2D const & edge_p2, // punto2 del segmento
                   Point2D * q ) // Punto de intersección (si lo hay)
{
    Line2D edge_line;
    Point2D p;

    edge_line.construct( edge_p1, edge_p2 );
    if( l.intersect( edge_line, &p ) == false )
    	return false;

    // A partir de aquí ya sabemos que no son paralelas

    if( edge_p1.x() == edge_p2.x() )
    {
        if( edge_p1.y() == edge_p2.y())
        {
            // Caso 1
            // El segmento es un punto
            if( p == edge_p1 )
            {
                *q = p;
                return true;
            }
            else
                return false;
        }
        else if( edge_p1.y() > edge_p2.y() )
        {
            // Caso 2, el segmento tiene esta forma: |
            // edge_p1 es el punto de arriba
            if(

                p.y() <= edge_p1.y() && p.y() >= edge_p2.y() )
            {
                *q = p;
                return true;
            }
            else
                return false;

        }
        else // edge_p1.y() < edge_p2.y()
        {
            // Caso 3, el segmento tiene esta forma: |
            // edge_p1 es el punto de abajo
            if(

                p.y() >= edge_p1.y() && p.y() <= edge_p2.y() )
            {
                *q = p;
                return true;
            }
            else
                return false;
        }

    }
    else if( edge_p1.x() < edge_p2.x() )
    {
        if( edge_p1.y() == edge_p2.y())
        {
            // Caso 4, el segmento tiene esta forma: _
            // edge_p1 es el punto de la izquierda
            if( p.x() >= edge_p1.x() && p.x() <= edge_p2.x()

                 )
            {
                *q = p;
                return true;
            }
            else
                return false;
        }

        else if( edge_p1.y() > edge_p2.y() )
        {
            // Caso 5, el segmento tiene esta forma: \
            // edge_p1 es el punto de la izquierda
            if( p.x() >= edge_p1.x() && p.x() <= edge_p2.x()
                &&
                p.y() <= edge_p1.y() && p.y() >= edge_p2.y() )
            {
                *q = p;
                return true;
            }
            else
                return false;
        }

        else
        {
            // Caso 6, el segmento tiene esta forma: /
            // edge_p1 es el punto de la izquierda, caso base
            if( p.x() >= edge_p1.x() && p.x() <= edge_p2.x()
                &&
                p.y() >= edge_p1.y() && p.x() <= edge_p2.y() )
            {
                *q = p;
                return true;
            }
            else
                return false;
        }
    }
    else // edge_p1.x() > edge_p2.x()
    {
        if( edge_p1.y() == edge_p2.y())
        {
            // Caso 4, el segmento tiene esta forma: _
            // edge_p1 es el punto de la derecha
            if( p.x() <= edge_p1.x() && p.x() >= edge_p2.x()

                 )
            {
                *q = p;
                return true;
            }
            else
                return false;
        }

        else if( edge_p1.y() > edge_p2.y() )
        {
            // Caso 5, el segmento tiene esta forma: /
            // edge_p1 es el punto de la derecha
            if( p.x() <= edge_p1.x() && p.x() >= edge_p2.x()
                &&
                p.y() <= edge_p1.y() && p.y() >= edge_p2.y() )
            {
                *q = p;
                return true;
            }
            else
                return false;
        }

        else
        {
            // Caso 6, el segmento tiene esta forma:
            // edge_p1 es el punto de la derecha
            if( p.x() <= edge_p1.x() && p.x() >= edge_p2.x()
                &&
                p.y() >= edge_p1.y() && p.y() <= edge_p2.y() )
            {
                *q = p;
                return true;
            }
            else
                return false;
        }
    }
}


inline
void intersectLinePolygon( std::vector<Point2D> const & polygon,
                           Line2D const & line,
                           std::vector<Point2D> * out1_container,
                           std::vector<Point2D> * out2_container
                           )
{
    // Entrada: Una linea y un polígono convexo, el último punto es igual al primero
    // Salida: A lo más dos polígonos, resultado de la intersección

    Line2D edge_line;

    // Puntos de intersección
    Point2D q[4];
    int q_i[4];
    Point2D p;
    Point2D const * p1 = 0;
    Point2D const * p2 = 0;

    bool edge_intersect = false;

    int i    = 0;
    int size = polygon.size();
    int intersects = 0;

    if( size < 3 )
    {
        return;
    }
    // Calculamos las intersecciones
    for( i=1; i<size; i++ )
    {
        p1 = &polygon[i-1];
        p2 = &polygon[i];

        edge_line.construct( *p1, *p2 );

        if( line == edge_line )
        {
        	*out1_container = polygon;
        	out2_container->clear();
            //std::cout << "intersectLinePolygon(): segment equal with segment_line" << std::endl;
        	return;
        }
        edge_intersect = edgeIntersectLine( line, *p1, *p2, &p );
        //std::cout << "Edge " << *p1 << *p2 << " intersect " << line << " :" << edge_intersect;
        //std::cout << std::endl;
        if( edge_intersect )
        {
            q[intersects] = p;
            q_i[intersects] = i;
            //std::cout << "Intersection: " << q[intersects] << std::endl;
            intersects ++;
        }
    }
    switch( intersects )
    {
    case 0:
        *out1_container = polygon;
        out2_container->clear();
        break;
    case 1:
        // Sólo hay una intersección cuando intersecta con un vértice
        *out1_container = polygon;
        out2_container->clear();
        break;
    case 2: // Intersección en dos aristas o en un solo vértice
    	for( i=0; i<size; i++ )
    	{
            if( polygon[i] == q[0] )
    		{
                //std::cout << "Interscet vertex" << std::endl;
    			*out1_container = polygon;
    			out2_container->clear();
    			return;
    		}
    	}
    	// Construimos el primer polígono
    	out1_container->clear();
        for( i=0; i<q_i[0]; i++ )
    		out1_container->push_back( polygon[i] );
        out1_container->push_back( q[0] );
        out1_container->push_back( q[1] );
        for( i=q_i[1]; i<size; i++ )
    		out1_container->push_back( polygon[i] );

    	// construimos el segundo
    	out2_container->clear();
        out2_container->push_back( q[0] );
        for( i=q_i[0]; i<q_i[1]; i++ )
    		out2_container->push_back( polygon[i] );
        out2_container->push_back( q[1]);
        out2_container->push_back( q[0] );

        break;
    case 3: // Intersecta en un vértice y en una arista
    	// Construimos el primer polígono
        //std::cout << " Cut a vertex " << std::endl;
        if( q[0] == q[1] )
        {

            out1_container->clear();
            for( i=0; i<q_i[0]; i++)
                out1_container->push_back( polygon[i] );
            out1_container->push_back( q[0] );
            out1_container->push_back( q[2] );
            for( i=q_i[2]; i<size; i++ )
                out1_container->push_back( polygon[i] );

            out2_container->clear();
            out2_container->push_back( q[1] );
            for( i=q_i[1]; i<q_i[2]; i++ )
                out2_container->push_back( polygon[i] );
            out2_container->push_back( q[2] );
            out2_container->push_back( q[1] );

        }
        else if( q[1] == q[2] )
        {
            out1_container->clear();
            for( i=0; i<=q_i[0]; i++ )
                out1_container->push_back( q[0] );
            out1_container->push_back( q[0] );
            out1_container->push_back( q[2] );
            for( i=q_i[2]; i<size; i++ )
                out1_container->push_back( polygon[i] );


            out2_container->clear();
            out2_container->push_back( q[0] );
            for( i=q_i[0]; i<q_i[1]; i++ )
                out1_container->push_back( polygon[i] );
            out2_container->push_back( q[1] );
            out2_container->push_back( q[0] );
        }

    	break;
    case 4: // Intersecta en dos vértices del polígono
    	// Construimos el primer polígono
    	out1_container->clear();


    	// construimos el segundo
    	out2_container->clear();

    	break;
    default:
        out1_container->clear();
        out2_container->clear();
        break;
    }
    //std::cout << "intersects: " << intersects << std::endl;
}


inline
Line2D &
pointsBisector( Point2D const & p, Point2D const & q)
{
	static Line2D l;
	double slope = p.slopeBetween( q );
	if( slope == std::numeric_limits<double>::infinity() )
		slope = 0;
	else if( slope == 0 )
		slope = std::numeric_limits<double>::infinity();
	else
		slope = -1.0/slope;
	l.construct( p.midPointWidh(q), slope );
	return l;
}


inline
bool
insideConvexPolygon( Point2D const & p, std::vector<Point2D> & polygon )
{
    // Indica si un punto está dentro del polígono
    // Usa un algoritmo basado en la dirección de la linea
    // Los polígonos deben estar construidos angular-negativamente
    // ( en dirección de las manecillas del reloj )
    Line2D  l;
    Point2D const * p1;
    Point2D const * p2;
    bool    is_inside = true;
    for( std::vector<Point2D>::const_iterator it = polygon.begin();
         (it+1) != polygon.end();
         ++it )
    {
        p1 = & (*it);
        p2 = & (*(it+1));
        l.construct( *p1, *p2 );
        if( l.eval( p )  > 0 )
            is_inside = false;
    }
    return is_inside;
}


inline
void
constructVoronoiCell( Point2D  & site,
                      std::vector<Point2D> & sites,
                      std::vector<Point2D> & initial_cell,
                      std::vector<Point2D> * cell_container )
{
    std::vector<Point2D> cell_1;
    std::vector<Point2D> cell_2;
    Line2D bisector;

    *cell_container = initial_cell;

    for( std::vector<Point2D>::iterator it = sites.begin();
         it != sites.end();
         ++it )
    {
        //l.construct( *it, *(it+1) );
        bisector = pointsBisector( site, *it );
        //std::cout << "Bisector for  " << site << " and " << *it << ":" << bisector << std::endl;
        intersectLinePolygon( *cell_container, bisector, &cell_1, &cell_2 );
        if( insideConvexPolygon( site, cell_1) )
        {
            //std::cout << "Took cell_1 " << std::endl;
            *cell_container = cell_1;
        }
        else
        {
            //std::cout << "Took cell_2 " << std::endl;
            *cell_container = cell_2;
        }
    }
}


#endif
