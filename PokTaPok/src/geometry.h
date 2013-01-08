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

    Vector2D & operator+(const Vector2D & v1)
    {
	static Vector2D v;
	v.x = v1.x + this->x;
	v.y = v1.y + this->y;
	return v;
    }

    double normita()
    {
        return sqrt( x*x + y*y );
    }

    static
    Vector2D fromPolar( const double & r,
                       const double & ang )
      {
          return Vector2D( r * std::cos( ang ),
                          r * std::sin( ang ) );
      }
};
#endif
