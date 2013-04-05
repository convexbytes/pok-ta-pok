#include "utilities.h"
#include "potentialField.h"

PotentialField::PotentialField()
{
	int i;
	dest = 0.0;
	pos = 0.0;
	for( i=0; i<11; ++i )
	{
		ally_vec[i] = 0;
		ally_on [i] = false;

		foe_vec[i]  = 0;
		foe_on [i]  = false;

	}
}



Vector2D PotentialField::resultante()
{
	Vector2D res;
	int i;

	res = 0.0;

	res += ( dest - pos );

	for( i=0; i < 11; ++i)
	{
		// Sumamos cada vector con distancia de nuestra posición a cada contrario
		// en dirección a nuestra posición
		if( foe_on[i] )
			res += ( pos - this->foe_vec[i] );
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

void PotentialField::setTeammate( Vector2D const & v, int unum )
{
	if( unum < 1 || unum>10 )
		return;
	unum--;
	this->ally_vec[unum] = v;
}

void PotentialField::setOpponent( Vector2D const & v, int unum )
{
	if( unum < 1 || unum > 10 )
		return;
	unum--;
	this->foe_vec[unum] = v;
}

void PotentialField::setDest( Vector2D const & v)
{
	this->dest = v;
}

void PotentialField::turnOnOpponent( int unum )
{
	if( unum < 1 || unum > 10)
		return;
	--unum;
	foe_on[ unum ] = true;
}

void PotentialField::turnOffOpponent( int unum )
{
	if( unum < 1 || unum > 10)
		return;
	--unum;
	foe_on[ unum ] = false;
}
void PotentialField::turnOnTeammate( int unum )
{
	if( unum < 1 || unum > 10)
		return;
	--unum;
	ally_on[ unum ] = true;
}

void PotentialField::turnOffTeammate( int unum )
{
	if( unum < 1 || unum > 10)
		return;
	--unum;
	ally_on[ unum ] = false;
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
