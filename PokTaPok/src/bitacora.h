#ifndef BITACORA_H
#define BITACORA_H

#include "gameData.h"
#include "gameCommand.h"
#include "worldModel.h"
#include "gameObject.h"
#include <deque>
#define  tamBitacora 10

class objetoBitacora
{
public:
	string   name;
	int      num;
	Vector2D pos;
	double   dis;
	double   dir;	
	int      ciclo;
};

class Bitacora
{
public:
    Bitacora( GameData * game_data, WorldModelV1 * world_model );
    void insertData();          
	deque <objetoBitacora>  bitacoraRivales[11]; // 11 rivales
	deque <objetoBitacora>  bitacoraAmigos[11];  // 10 amgios, el lugar nuestro se queda vacío
 	deque <objetoBitacora>  bitacoraBalon;       // balon               
	
// deque <MobileObject> bitacora[22]  // 	<----  O bien
    
private:
    GameData                * game_data;
    AgentCommand            * command_commited; // Command_commited
    WorldModelV1			* world_model;
    
};

Vector2D predictVel      ( deque <objetoBitacora> const & fila );
Vector2D predictPose     ( deque <objetoBitacora> const & fila );
double   predictDistance ( deque <objetoBitacora> const & fila , int n );
int      predictCycles   ( deque <objetoBitacora> const & fila , double distanciaObjetivo );



#endif // BITACORA_H
