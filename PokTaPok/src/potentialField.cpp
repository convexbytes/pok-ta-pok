#include "utilities.h"
#include "potentialField.h"

using namespace potentialField;

PotentialField::PotentialField()
{
	int i;
	dest = 0.0;
	pos = 0.0;
	for( i=0; i<11; ++i )
	{
		vec_ally[i] = 0;
		vec_foe[i] = 0;
	}
}

Vector2D & PotentialField::resultante()
{
	static Vector2D res;
	int i;

	res = 0.0;

	res += dest;

	for( i=0; i < 11; ++i)
	{
		// Sumamos cada vector con distancia de nuestra posición a cada contrario
		// en dirección a nuestra posición
		res += ( pos - this->vec_foe[i] );
	}

	return res;
}

void PotentialField::setPos( Vector2D const & v )
{
	this->pos = v;
}

void PotentialField::addAttractor( Vector2D const & v )
{
	this->extra_attractor.push_back( v );
}

void PotentialField::addRepulsor( Vector2D const & v )
{
	this->extra_repulsor.push_back( v );
}

void PotentialField::setAlly( Vector2D const & v, int unum )
{
	if( unum < 1 || unum>10 )
		return;
	unum--;
	this->vec_ally[unum] = v;
}

void PotentialField::setFoe( Vector2D const & v, int unum )
{
	if( unum < 1 || unum>10 )
		return;
	unum--;
	this->vec_foe[unum] = v;
}

void PotentialField::setDest( Vector2D const & v)
{
	this->dest = v;
}


/* Para probar
int main()
{
	
	PotentialField pot_f;
	
	Vector2D v;
	Vector2D res;
	
	Vector2D pos ( 5, 5 );
	Vector2D dest( 50.0, 0);
	
	pot_f.setDest( dest );
	pot_f.setPos( pos );
	
	v.x = 10;
	v.y = 0;
	pot_f.setFoe( v, 1 );
	
	v.x = 12.5;
	v.y = 3.5;
	pot_f.setFoe( v, 2 );
	
	
	res = pot_f.resultante();
	
	std::cout << "res: " << res << " angle: " << Rad2Deg( res.angle() );
	return 0;
}
*/
