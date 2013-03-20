#ifndef BITACORABALL_H
#define BITACORABALL_H

#include "gameData.h"
#include "gameCommand.h"
#include "worldModel.h"
#include "geometry.h"
#include <deque>
#define  tamBitacora 10

class objetoBalon 
{
	public:             
	    Vector2D pos;
	    double   dis;
	    double   dir;	
	    int      ciclo;    
};


class BitacoraBall
{
public:
    BitacoraBall( GameData * game_data, WorldModelV1 * world_model );
    void insertData();
	deque <objetoBalon>  bitacoraBalon;
    
private:
    GameData                * game_data;
    AgentCommand            * command_commited; // Command_commited
    WorldModelV1			* world_model;
    
    Vector2D predictVel      ();
    Vector2D predictPose     ();
    int      predictCycles   ();
    double   predictDistance (int n);
};


#endif // BITACORABALL_H
