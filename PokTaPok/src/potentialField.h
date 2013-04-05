#ifndef POTENTIAL_FIELD_H
#define POTENTIAL_FIELD_H
#include "geometry.h"
#include "vector"

class PotentialField
{

public:
	PotentialField();
	Vector2D resultante();

	void setPos			( Vector2D const & v );
	void setTeammate	( Vector2D const & v, int unum );
	void setOpponent 	( Vector2D const & v, int unum );
	void setDest		( Vector2D const & v );
	void addAttractor	( Vector2D const & v );
	void addRepulsor	( Vector2D const & v );

	void turnOnOpponent	( int unum );
	void turnOffOpponent( int unum );
	void turnOnTeammate	( int unum );
	void turnOffTeammate( int unum );

//private: Pendiente de implementar accesadores, etc

	Vector2D pos; // Posición del jugador

	Vector2D dest;

	Vector2D ally_vec[11];
	bool	 ally_on [11];

	Vector2D foe_vec [11];
	bool 	 foe_on  [11];



	std::vector<Vector2D> extra_repulsor;
	std::vector<Vector2D> extra_attractor;

};


#endif
