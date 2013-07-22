#ifndef ABILITY_H
#define ABILITY_H

#include "worldModel.h"

class Ability;
class PassToPoint;
class GoToPoint;
class FreezeBall;
class BallInterception;
class CenterBall;
class RunWithBall;
class Shoot;

// Habilidades exclusivas del portero
class GoToPoint2;
class GoalieLine;


class Ability
{
public:
	Ability( WorldModel   * world,
	 	 	  AgentCommand * command
	 	 	): m_world(world), m_command(command)
	{ }

	 ~Ability()
	 {
		 m_world = 0;
		 m_command = 0;
	 }

	 virtual void call() = 0;

protected:
	WorldModel   * m_world;
	AgentCommand * m_command;
};


class PassToPoint : public Ability
{
public:
	PassToPoint( WorldModel *w, AgentCommand *c)
				: Ability( w, c ),
				  m_target(0), m_desired_vel(1)
	{}
	virtual void call();

	void setTarget( Vector2D const & v );
	void setDesiredVel( double desired_vel );

private:
	Vector2D m_target;
	double 	 m_desired_vel;

	double kickSpeedToTravel	  ( double d, double e );
	double 	actualKickPowerRate( double  dir, double dist );
	double kickPowerForSpeed	  ( double speed, double actkpr);

};

class GoToPoint : public Ability
{
public:
	GoToPoint( WorldModel *w, AgentCommand *c)
				: Ability( w, c ),
				  m_radius(1), m_target(0), m_dash_override(false), m_dash_power(100)
	{	}
	virtual void call();

	void setTarget   ( const Vector2D & target );
	void setRadius   ( double radius );
	void setDashPower( double power );
	void setDashOverride( bool on );
private:
	double	 m_radius;
	Vector2D m_target;
	bool	 m_dash_override;
	double 	 m_dash_power;
};

class FreezeBall : public Ability
{
public:
	FreezeBall( WorldModel *w, AgentCommand *c)
				: Ability( w, c )
	{ }
	virtual void call();
private:

};

class SearchBall : public Ability
{
public:
	SearchBall( WorldModel *w, AgentCommand *c)
				: Ability( w, c )
	{ }
	virtual void call();
};

class BallInterception : public Ability
{
public:
	BallInterception( WorldModel *w, AgentCommand *c,
					    FreezeBall * a1, SearchBall * a2 )
					  : Ability( w, c ),
					    m_freeze_ball( a1 ),
					    m_search_ball( a2 ),
					    m_state(START), m_inter_point(0),
					    m_time_to_reach(0), m_start_time(0), m_last_call_time(0),
					    m_turn_requested(0), m_turn_param(0)
	{

	}
	virtual void call();
	enum State
	{
		START,
		SEARCH_BALL,
		COMPUTE_POINT_TURN,
		GO,
		FREEZE
	};
	// Para calcular la interceptación se velocidad promedio,
	// aunque este valor debería depender de la estamina y otros factores.
	double static const AVERAGE_VEL = 0.94;
private:
	FreezeBall * m_freeze_ball;
	SearchBall * m_search_ball;

	State		m_state;

	Vector2D	m_inter_point; // Punto de interceptación
	int 	m_time_to_reach;
	int		m_start_time;
	int		m_last_call_time;
	bool 	m_turn_requested;
	double	m_turn_param;

	void 	start();
	void	searchBall();
	void 	computePointTurn();
	void 	go();
	void 	freeze();

	Vector2D velToInterceptBall( int t );
};

class CenterBall : public Ability
{
public:
	CenterBall( WorldModel *w, AgentCommand *c)
			 : Ability( w, c ),
			   m_precision( 10 )
	{}
	virtual void call();

	void setPrecision( double p );
private:
	double m_precision;

	void 	align();
};

class RunWithBall : public Ability
{
public:
	RunWithBall( WorldModel *w, AgentCommand *c,
			     PassToPoint * a1, GoToPoint * a2 )
			  : Ability( w, c ),
			    m_pass_to_point( a1 ), m_go_to_point( a2 ),
			    m_target(0), m_radius(1), m_ball_final_vel(0.5), m_pass_distance(2)
	{	}
	virtual void call();

	void setTarget( const Vector2D & v );
	void setRadius( double r );
private:
	PassToPoint * m_pass_to_point;
	GoToPoint   * m_go_to_point;

	Vector2D	m_target;
	double		m_radius;
	double		m_ball_final_vel;
	double		m_pass_distance;
};

class GoToPoint2 : Ability
{
public:
	GoToPoint2( WorldModel *w, AgentCommand *c)
			  : Ability( w, c ),
		  	    m_target(0), m_radius(1)
	{}
	virtual void call();

	void setTarget( const Vector2D & v );
	void setRadius ( double r );
private:
	Vector2D m_target;
	double	 m_radius;
};

class GoalieLine : public Ability
{
public:
	GoalieLine( WorldModel * w, AgentCommand * c,
				 SearchBall * a1, CenterBall * a2, GoToPoint2 * a3 )
				: Ability( w, c ),
				  m_search_ball(a1), m_center_ball(a2), m_go_to_point2(a3),
				  m_x_target(-48)
	{ }
	virtual void call();

	void setXTarget( double x );
private:
	SearchBall * m_search_ball;
	CenterBall * m_center_ball;
	GoToPoint2 * m_go_to_point2;

	double m_x_target; // Línea donde se desplaza el portero
};

class Shoot: public Ability
{
public:
	Shoot( WorldModel *w, AgentCommand *c,
		   PassToPoint * a1 )
		 : Ability( w, c ),
		   m_pass_to_point( a1 ), m_distance_to_post( 1.0 )

	{	}
	virtual void call();
	int static const GOALIE_INFO_EXPIRE_TIME = 5;

	void setDistanceToPost( double d );
private:
	PassToPoint * m_pass_to_point;

	double		  m_distance_to_post;
};

#endif // ABILITY_H
