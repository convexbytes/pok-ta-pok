
#ifndef WORLD_MODEL_H
#define WORLD_MODEL_H
#include "sensorHandler.h"
#include "sensor.h"
#include "gameCommand.h"
#include "localizationAdapter.h"
#include "utilities.h"

#include <deque>

#define  TAM_BITACORA 10

#ifndef TEAM_NAME
#define TEAM_NAME "PokTaPok"
#endif

class AgentState;
class WorldModel;
class ObjetoBitacora;


class AgentState
{
public:
	AgentState();
	// Principales
    Vector2D  pos;
    double   angle;
    char 	  side;
    int 	  unum;
    double 	  head_angle;

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

    // Acerca del cuerpo
    double stamina;
    double effort;
    double speed_amount;
    double speed_dir;

    // Aural
    int 		  attention_unum;
    AttentionTeam attention_team;

};

class ObjetoBitacora
{
public:
	string   name;
	int      num;
	Vector2D pos;
	double   dis;
	double   dir;
	double	 dir_from_body;
	int      ciclo;
	Vector2D vel;
	bool     goalie;
};

class WorldModel
{
public:
    WorldModel( GameData * game_data );
    void update( GameData * game_data );

    const GameData & gameData();

    // Guardamos info de los objetos
    deque <ObjetoBitacora>  bitacoraRivales[11]; // 11 rivales
    deque <ObjetoBitacora>  bitacoraAmigos[11];  // 10 amgios, el lugar nuestro se queda vacío
    deque <ObjetoBitacora>  bitacoraBalon;       // balon

    void 	 actualizarBitacora();

    Vector2D estBallPosition() const; // Estimate current ball position
    Vector2D estBallVelocity() const; // Estimate current ball velocity

    const AgentState & me() const;
    PlayModeHearable 	playMode() const;
    int					time() const;

private:
    GameData  			* m_game_data;
    PlayModeHearable      m_play_mode;
    int                   m_time;
    AgentState	          m_me;
    LocalizationAdapter * m_loc_adapter;

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
