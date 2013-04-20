#include "geometry.h"


Vector2D::Vector2D()
{
	this->x = 0.0;
	this->y = 0.0;
}
Vector2D::Vector2D( double x, double y )
{
	this->x = x;
	this->y = y;
}

Vector2D::Vector2D( double a )
{
	this->x = this->y = a;
}

void Vector2D::operator=( Vector2D const & v)
{
	x = v.x;
	y = v.y;
}

void Vector2D::operator=( double const a )
{
	x = y = a;
}

void Vector2D::set( double x, double y )
{
	this->x = x;
	this->y = y;
}
Vector2D  Vector2D::operator+(const Vector2D & v1) const
{
	Vector2D v;
	v.x = v1.x + this->x;
	v.y = v1.y + this->y;
	return v;
}

void Vector2D::operator+=(const Vector2D & v1)
{
	this->x += v1.x;
	this->y += v1.y;
}

Vector2D Vector2D::operator-( const Vector2D & v ) const
{
	Vector2D v1;
	v1.x = x - v.x;
	v1.y = y - v.y;
	return v1;
}

Vector2D Vector2D::operator/( double r ) const
{
	Vector2D v;
	if( r == 0 )
	{
		std::cerr << "Warning. Vector2D::operator/(): zero division." << std::endl;
		return v;
	}
	v.x = x/r;
	v.y = y/r;
	return v;
}

void Vector2D::operator/=( double r )
{
	if( r == 0 )
		return;
	this->x /= r;
	this->y /= r;
}

Vector2D Vector2D::operator *( double r) const
{
	Vector2D v;
	v.x = this->x*r;
	v.y = this->y*r;
	return v;
}
void Vector2D::operator*=( double r )
{
	this->x *= r;
	this->y *= r;
}

double Vector2D::normita() const
{
	return sqrt( x*x + y*y );
}

double Vector2D::prodPunto( Vector2D const & v ) const
{
	return x * v.x + y * v.y;
}

//double Vector2D::distance( Vector2D const & v ) const
//{
//	return sqrt( prodPunto( v ) );
//}

double Vector2D::angleBetween( Vector2D const & v ) const
{

	return (*this-v).angle();
}

double Vector2D::angle() const
{
	if( x != 0.0 || y != 0.0 )
		return std::atan2(y,x);
	else
		return 0.0;
}

void
Vector2D::normalize()
{
	double d = this->normita();
	if( d != 0 )
	{
		this->x /= d;
		this->y /= d;
	}
}

void Vector2D::rotate( double angle_rad )
{
	double x_tmp = this->x;

	x = x*cos(angle_rad) - y*sin(angle_rad);
	y = x_tmp*sin(angle_rad) + y*cos(angle_rad);
}

double
Vector2D::magSquared() const
{
	return x*x +y*y;
}

Vector2D
Vector2D::projectionOver( Vector2D const & v ) const
{
	return v * (this->prodPunto(v) / v.magSquared() );
}

Vector2D
Vector2D::projection( Vector2D const & A, Vector2D const & B )
{
	return B * ( A.prodPunto(B) / B.magSquared() );
}

std::ostream & operator <<( std::ostream & os, Vector2D const & v)
{
	os << "Vector2D(" << v.x << "," << v.y << ")";
	return os;
}

Vector2D
Vector2D::fromPolar( const double & r, const double & ang )
{
	Vector2D vec( r * std::cos( ang ), r * std::sin( ang ) );
	return vec;
}

Vector2D
Vector2D::toPolar( const double x, const double y )
    {
        Vector2D vec( sqrt( x*x + y*y ) , atan2( y, x ) );
        return vec;
    }

Point2D::Point2D()
{
	M_x = 0.0;
	M_y = 0.0;
}
Point2D::Point2D( double x, double y)
{
	M_x = x;
	M_y = y;
}

void Point2D::set( double x, double y )
{
	M_x = x;
	M_y = y;
}

bool Point2D::operator ==( Point2D const & p ) const
    		{
	return ( M_x == p.x() && M_y == p.y() );
    		}

void Point2D::operator =( Point2D const & p )
{
	M_x = p.x();
	M_y = p.y();
}
void Point2D::operator /= ( double a)
{
	if( a == 0 )
		return;
	this->M_x /= a;
	this->M_y /= a;
}
double Point2D::x() const { return M_x; }
double Point2D::y() const { return M_y; }


Point2D & Point2D::midPointWidh( const Point2D & p ) const
{
	static Point2D q;
	q.set( p.x() + this->x(), p.y() + this->y() );
	q /= 2.0;
	return q;
}

double Point2D::slopeBetween( Point2D const & p) const
{
	double slope;
	if( M_x == p.x() )
		slope = std::numeric_limits<double>::infinity();
	else
		slope = ( p.y() - M_y ) / ( p.x() - M_x );
	return slope;

}

std::ostream & operator <<( std::ostream & os, Point2D const & p )
{
	os << "Point2D(" << p.x() << "," << p.y() << ")";
	return os;
}

Line2D::Line2D()
{
	//identidad
	M_a = -1.0;
	M_b = 1.0;
	M_c = 0.0;
}
Line2D::Line2D( double a, double b, double c)
{
	construct( a, b, c );
}

Line2D::Line2D( Point2D const & p1, Point2D const & p2  )
{
	construct( p1, p2 );
}

void Line2D::construct( double a, double b, double c )
{
	M_a = a;
	M_b = b;
	M_c = c;
}

void Line2D::construct( Point2D const & p1, Point2D const & p2  )
{
	// Construye una linea en la dirección de p2
	if ( p1 == p2 )
	{
		construct( -1, 1, 0 ); // Identidad
		std::cerr << "Line2D::construct( Point2D const &, Point2D const &): "
				<< "Two equal points given, constructing identity instead."
				<< std::endl;
		return;
	}

	M_a = p1.y() - p2.y();
	M_b = p2.x() - p1.x();
	M_c = p1.y()*( p1.x() - p2.x() ) +
			p1.x()*( p2.y() - p1.y() );

}

void Line2D::construct( Point2D const & p, double slope )
{
	// Basado en y-y1 = m( x-x1 )
	if( slope == std::numeric_limits<double>::infinity() )
	{
		// Construimos una recta que pase por p, x = p.x
		M_a = 1.0;
		M_b = 0.0;
		M_c = - p.x();
		return;
	}
	M_a = -slope;
	M_b = 1;
	M_c = -p.y()+slope*p.x();
}

double Line2D::a() const { return M_a; }
double Line2D::b() const { return M_b; }
double Line2D::c() const { return M_c; }

bool Line2D::operator ==( Line2D const & l ) const
		{
	double k;
	if( l.a() == 0 )
	{
		k = M_b / l.b();
		if( ( l.c() / k ) == M_c )
			return true;
		else
			return false;
	}
	else if( l.b() == 0 )
	{
		k = M_a / l.a();
		if( ( l.c() / k ) == M_c )
			return true;
		else
			return false;
	}
	k = M_a / l.a();
	return l.b()/k == M_b  && l.c()/k == M_c;
		}

std::ostream & operator <<(  std::ostream & os, Line2D const & l )
{
	os << "Line2D("
			<< l.a() << "x+"
			<< l.b() << "y+"
			<< l.c() << "=0)";
	return os;
}

double
Line2D::eval(const Point2D & p )
{
	return (M_a*p.x() + M_b*p.y() + M_c);
}

bool
Line2D::intersect( Line2D const & l, Point2D * p ) const
{
	double den = M_a*l.b() - l.a()*M_b;
	double xi;
	double yi;
	if( den == 0 )
		return false;
	xi = ( M_b*l.c() - l.b()*M_c ) / den; // b1*c2 -b2*c1
	yi = ( M_c*l.a() - l.c()*M_a ) / den; // c1*a2 - c2*a1
	p->set( xi, yi );
	return true;
}

bool
Line2D::isParallelWith( Line2D const & l) const
{
	double den = M_a*l.b() - l.a()*M_b;
	return den == 0;
}


bool
insideConvexPolygon( Point2D const & p, std::vector<Point2D> const & polygon )
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

void
intersectLinePolygon(   std::vector<Point2D> const & polygon,
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


Vector2D
closestPointInSegment( Vector2D const & q,
					   Vector2D const & p1, // primer punto que define el segmento
					   Vector2D const & p2 // segundo punto que define el segmento
					   )
{
	Vector2D closest_point;
	Vector2D P1_P2 = p2 - p1;
	Vector2D P1_Q  = q - p1;
	Vector2D proj;
	double   prod_punto = P1_Q.prodPunto( P1_P2 );
	double 	 proj_mag;
	if( prod_punto < 0 )
	{
		closest_point = p1;
	}
	else
	{
		proj = P1_Q.projectionOver( P1_P2 );
		proj_mag = proj.normita();

		if( proj_mag < P1_P2.normita() )
		{
			closest_point = p1 + proj;
		}
		else
		{
			closest_point = p2;
		}
	}

	return closest_point;
}
