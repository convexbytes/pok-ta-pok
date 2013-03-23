#ifndef POTENTIAL_FIELD_H
#define POTENTIAL_FIELD_H
#include "geometry.h"
#include "vector"

namespace potentialField
{

class PotentialField
{

public:
	PotentialField();
	Vector2D & resultante();

	void setPos		( Vector2D const & v );
	void setAlly	( Vector2D const & v, int unum );
	void setFoe 	( Vector2D const & v, int unum );
	void setDest	( Vector2D const & v );
	void addAttractor	( Vector2D const & v );
	void addRepulsor	( Vector2D const & v );


//private: Pendiente de implementar accesadores, etc

	Vector2D pos; // Posición del jugador

	Vector2D dest;

	Vector2D vec_ally[10];
	Vector2D vec_foe[10];

	std::vector<Vector2D> extra_repulsor;
	std::vector<Vector2D> extra_attractor;

};



}


#endif
