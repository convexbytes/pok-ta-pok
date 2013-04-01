#ifndef GEOMETRY_H
#define GEOMETRY_H
#define M_PI 3.1415926535897932
#include <cmath>
#include <iostream>
class Vector2D
{
public:

    double x;
    double y;


    Vector2D()
    {
        this->x = 0.0;
        this->y = 0.0;
    }
    Vector2D( double x, double y )
    {
        this->x = x;
        this->y = y;
    }

    Vector2D( double a )
    {
    	this->x = this->y = a;
    }

    void operator=( Vector2D const & v)
    {
        x = v.x;
        y = v.y;
    }

    void operator=( double const a )
    {
        x = y = a;
    }

    void set( double x, double y )
    {
    	this->x = x;
    	this->y = y;
    }
    Vector2D & operator+(const Vector2D & v1) const
    {
        static Vector2D v;
        v.x = v1.x + this->x;
        v.y = v1.y + this->y;
        return v;
    }

    void operator+=(const Vector2D & v1)
    {
        this->x += v1.x;
        this->y += v1.y;
    }

    Vector2D & operator-( const Vector2D & v ) const
    {
        static Vector2D v1;
        v1.x = x - v.x;
        v1.y = y - v.y;
        return v1;
    }

    Vector2D & operator/( double r ) const
    {
        static Vector2D v;
        if( r == 0 )
        {
        	std::cerr << "Warning. Vector2D::operator/(): zero division." << std::endl;
            return v;
        }
        v.x = x/r;
        v.y = y/r;
        return v;
    }

    void operator/=( double r )
    {
        if( r == 0 )
            return;
        this->x /= r;
        this->y /= r;
    }

    Vector2D & operator *( double r) const
    {
    	static Vector2D v;
    	v.x = this->x*r;
    	v.y = this->y*r;
    	return v;
    }
    void operator*=( double r )
    {
        this->x *= r;
        this->y *= r;
    }

    double normita() const
    {
        return sqrt( x*x + y*y );
    }

    double prodPunto( Vector2D const & v ) const
    {
        return x * v.x + y * v.y;
    }

    double distance( Vector2D const & v ) const
    {
        return sqrt( prodPunto( v ) );
    }

    double angleBetween( Vector2D const & v ) const
    {

        return (*this-v).angle();
    }

    double angle() const
    {
        if( x != 0.0 || y != 0.0 )
            return std::atan2(y,x);
        else
            return 0.0;
    }


    void rotate( double angle_rad )
    {
    	double x_tmp = this->x;

        x = x*cos(angle_rad) - y*sin(angle_rad);
        y = x_tmp*sin(angle_rad) + y*cos(angle_rad);
    }

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
#endif
