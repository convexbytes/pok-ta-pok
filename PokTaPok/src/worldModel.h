
#ifndef WORLD_MODEL_H
#define WORLD_MODEL_H
#include "sensorHandler.h"
#include "sensor.h"
#include "gameCommand.h"
#include "localizationEngine.h"
#include "utilities.h"

#include <deque>

#define  TAM_BITACORA 10

#ifndef TEAM_NAME
#define TEAM_NAME "PokTaPok"
#endif

enum PossessionBall
{

    PROPIA = 0,     // balón en poder del agente
    EQUIPO,         // balón en poder del equipo propio
    RIVAL,          // balón en poder del rival
    DESCONOCIDA,    // balón en poder desconocida
    SUELTA,         // balón suelto
    PERDIDA         // balón sin ver
};

class AgentStateV1
{
public:
	AgentStateV1();
	// principales
    Vector2D pos;
    double   angle;
    char 	 side;
    int 	 unum;
    double 	 head_angle;
    double   angleRad() const { return DEG2RAD*angle; }
    double   angleDeg() const { return angle; }
    double   headAngleRad() const { return DEG2RAD*head_angle; }
    double   headAngleDeg() const { return head_angle; }
    // Acerca de la vision
    ViewModeQuality view_mode_q;
    ViewModeWidth   view_mode_w;

    bool 			synch_see_on;

    int 			last_recv_visual_time;
    ViewModeQuality	last_recv_visual_q;
    ViewModeWidth	last_recv_visual_w;

    PossessionBall  possession_ball;

    // Acerca del cuerpo
    double stamina;
    double effort;

};

class ObjetoBitacora
{
public:
	string   name;
	int      num;
	Vector2D pos;
	double   dis;
	double   dir;
	int      ciclo;
	Vector2D vel;
};

class WorldModelV1
{
public:
    WorldModelV1( GameData * game_data );
    void update( GameData * game_data );


    // Para guardar las referencias de que se reciben en update
    GameData  	  * game_data;
    AgentCommand  * command_commited;


    AgentStateV1         me;
    LocalizationEngine * loc_engine;

    PlayModeHearable     play_mode;
    int                  time;

    // Guardamos info de los objetos
    deque <ObjetoBitacora>  bitacoraRivales[11]; // 11 rivales
    deque <ObjetoBitacora>  bitacoraAmigos[11];  // 10 amgios, el lugar nuestro se queda vacío
    deque <ObjetoBitacora>  bitacoraBalon;       // balon
    void 	 actualizarBitacora();
    Vector2D predictVel      ( deque <ObjetoBitacora> const & fila );
    Vector2D predictPose     ( deque <ObjetoBitacora> const & fila );
    double   predictDistance ( deque <ObjetoBitacora> const & fila , int n );
    int      predictCycles   ( deque <ObjetoBitacora> const & fila , double distanciaObjetivo );

    bool 	predictBallCurrentVel( Vector2D * v );

private:


    void updateOnBody   ();
    void updateOnInit   ();
    void updateOnSee    ();
    void updateOnHear   ();
    void updateOnOk		();
    void updateOnCommandSent();





};


Vector2D
fromChgToVel(  Vector2D const & player_pos, // Posición del jugador, absoluta.
						Vector2D const & player_vel, // Velocidad del jugador, absoluta.
						Vector2D const & obj_pos, // Posición del objeto que ve, absoluta.
						double obj_dist,
						double dist_chg,
						double dir_chg
						);
#endif // WORLD_MODEL_H
