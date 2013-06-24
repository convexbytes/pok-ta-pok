#ifndef UTILITIES_H
#define UTILITIES_H
#include "geometry.h"
#include <algorithm>
#define EPS 1.0e-10

const double TWO_PI  = 2.0*M_PI;
const double RAD2DEG = (180.0/M_PI);
const double DEG2RAD = (M_PI/180.0);

inline
int     signo    ( double x )
{
    if( x < 0.0 )
        return -1;
    else
        return 1;
}
inline
double  absoluto ( double x )
{
    if( x < 0.0 )
        return -x;
    else
        return x;
}
inline
double  grad2rad ( double grados )
{
    return grados*M_PI/180.0;
}
inline
double  rad2grad ( double radianes )
{
    return radianes*180.0/M_PI;
}

inline
int entre180(int angulo)
{
	int equivalente;

	equivalente = angulo;

	if( angulo<-180 || angulo>180 )
	{
		if(angulo<-360 || angulo>360)
		{
			equivalente = angulo % 360;
		}

		if(equivalente<-180 || equivalente>180)
		{
			equivalente = equivalente%180;
			if(equivalente<0)
				equivalente = 180 + equivalente;
			else
				equivalente = equivalente - 180;
		}
	}
	return equivalente;
}

inline
double entre180( double angulo )
{
	double equivalente;
	equivalente = angulo;
	if( angulo<-180 || angulo>180 )
	{
		if(angulo<-360 || angulo>360)
		{
			equivalente = std::fmod(angulo, 360.0);
		}

		if(equivalente<-180 || equivalente>180)
		{
			equivalente = std::fmod(angulo, 180.0);
			if(equivalente<0)
				equivalente = 180 + equivalente;
			else
				equivalente = equivalente - 180;
		}
	}
	return equivalente;
}

template < typename T >
const T &
bound( const T & low,
       const T & value,
       const T & high )
{
    return std::min( std::max( low, value ), high );
}


inline
double NormalizeDashPower( const double & p,
                           const double & minDashPower,
                           const double & maxDashPower
                          )
{
    return bound( minDashPower,
                  p,
                  maxDashPower );
}

inline
double
NormalizeDashAngle( const double & d,
                    const double & minDashAngle,
                    const double & maxDashAngle )
{
    return bound( minDashAngle,
                        d,
                        maxDashAngle );
}


inline
double
Rad2Deg( const double & a )
{
    return a * RAD2DEG;
}

inline
int
Rad2IDeg( const double & a )
{
    return static_cast< int >( a * RAD2DEG );
}

inline
int
Rad2IDegRound( const double & a )
{
    return static_cast< int >( rint( Rad2Deg( a ) ) );
}

inline
double
Deg2Rad( const double & a )
{
    return a * DEG2RAD;
}

inline
double
Quantize( const double & v,
          const double & q )
{
    return ( q < EPS
             ? v
             : rint( v / q ) * q );
}

inline
double
normalize_angle( double ang )
{
    if ( std::fabs( ang ) > TWO_PI )
    {
        ang = std::fmod( ang, TWO_PI );
    }

    if ( ang < -M_PI ) ang += TWO_PI;
    if ( ang > M_PI ) ang -= TWO_PI;

    return ang;
}
#endif // UTILITIES_H
