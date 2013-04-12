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

double Vector2D::distance( Vector2D const & v ) const
{
	return sqrt( prodPunto( v ) );
}

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


void Vector2D::rotate( double angle_rad )
{
	double x_tmp = this->x;

	x = x*cos(angle_rad) - y*sin(angle_rad);
	y = x_tmp*sin(angle_rad) + y*cos(angle_rad);
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

	// Para construir la dirección de la línea
	/*
        if( p1.x() == p2.x() )
        {
            if( p1.y() < p2.y() )
            {
                // La línea va con dirección hacia arriba
                // El lado positivo es a la izquierda
            }
            else  //p1.y() > p2.y()
            {
                // La linea va hacia abajo
                // El lado positivo es a la izquierda
                M_a *= -1.0;
                M_b *= -1.0;
                M_b *= -1.0;
            }
        }
        else if( p1.x() > p2.x() )
        {

        }
        else // p1.x() < p2.x()
        {

        }*/
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


