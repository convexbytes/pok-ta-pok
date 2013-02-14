#ifndef GEOMETRY_H
#define GEOMETRY_H
#define M_PI 3.1415926535897932
#include <cmath>
class Vector2D
{
public:
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


    double x;
    double y;

    void operator=( Vector2D const & v)
    {
        x = v.x;
        y = v.y;
    }

    void operator=( double const a )
    {
        x = y = a;
    }

    Vector2D & operator+(const Vector2D & v1)
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

    Vector2D & operator-( const Vector2D & v )
    {
        static Vector2D v1;
        v1.x = x - v.x;
        v1.y = y - v.y;
        return v1;
    }

    Vector2D & operator/( double r )
    {
        static Vector2D v;
        if( r == 0 )
            return *this;
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


    double normita()
    {
        return sqrt( x*x + y*y );
    }

    double prodPunto( Vector2D const & v )
    {
        return x * v.x + y * v.y;
    }

    double distance( Vector2D const & v )
    {
        return sqrt( prodPunto( v ) );
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
