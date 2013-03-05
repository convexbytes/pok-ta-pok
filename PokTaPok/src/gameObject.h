#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H
#include "geometry.h"
#include <string>
#include <iostream>
using namespace std;


#define FTL50_X -50
#define FTL40_X -40
#define FTL30_X -30
#define FTL20_X -20
#define FTL10_X -10
#define FT0_X 0
#define FTR10_X 10
#define FTR20_X 20
#define FTR30_X 30
#define FTR40_X 40
#define FTR50_X 50
#define FRT30_X 57.5
#define FRT20_X 57.5
#define FRT10_X 57.5
#define FR0_X 57.5
#define FRB10_X 57.5
#define FRB20_X 57.5
#define FRB30_X 57.5
#define FBR50_X 50
#define FBR40_X 40
#define FBR30_X 30
#define FBR20_X 20
#define FBR10_X 10
#define FB0_X 0
#define FBL10_X -10
#define FBL20_X -20
#define FBL30_X -30
#define FBL40_X -40
#define FBL50_X -50
#define FLB30_X -57.5
#define FLB20_X -57.5
#define FLB10_X -57.5
#define FL0_X -57.5
#define FLT10_X -57.5
#define FLT20_X -57.5
#define FLT30_X -57.5
#define FLT_X -52.5
#define FCT_X 0
#define FRT_X 52.5
#define FGRT_X 52.5
#define GR_X 52.5
#define FGRB_X 52.5
#define FRB_X 52.5
#define FCB_X 0
#define FLB_X -52.5
#define FGLB_X -52.5
#define GL_X -52.5
#define FGLT_X -52.5
#define FPLT_X -36
#define FPRT_X 36
#define FPRC_X 36
#define FPRB_X 36
#define FPLB_X -36
#define FPLC_X -36
#define FC_X 0

#define FTL50_Y -39
#define FTL40_Y -39
#define FTL30_Y -39
#define FTL20_Y -39
#define FTL10_Y -39
#define FT0_Y -39
#define FTR10_Y -39
#define FTR20_Y -39
#define FTR30_Y -39
#define FTR40_Y -39
#define FTR50_Y -39
#define FRT30_Y -30
#define FRT20_Y -20
#define FRT10_Y -10
#define FR0_Y 0
#define FRB10_Y 10
#define FRB20_Y 20
#define FRB30_Y 30
#define FBR50_Y 39
#define FBR40_Y 39
#define FBR30_Y 39
#define FBR20_Y 39
#define FBR10_Y 39
#define FB0_Y 39
#define FBL10_Y 39
#define FBL20_Y 39
#define FBL30_Y 39
#define FBL40_Y 39
#define FBL50_Y 39
#define FLB30_Y 30
#define FLB20_Y 20
#define FLB10_Y 10
#define FL0_Y 0
#define FLT10_Y -10
#define FLT20_Y -20
#define FLT30_Y -30
#define FLT_Y -34
#define FCT_Y -34
#define FRT_Y -34
#define FGRT_Y -7
#define GR_Y 0
#define FGRB_Y 7
#define FRB_Y 34
#define FCB_Y 34
#define FLB_Y 34
#define FGLB_Y 7
#define GL_Y 0
#define FGLT_Y -7
#define FPLT_Y -20
#define FPRT_Y -20
#define FPRC_Y 0
#define FPRB_Y 20
#define FPLB_Y 20
#define FPLC_Y 0
#define FC_Y 0


using namespace std;
enum EFlag
{
    FLAG_UNKNOWN = 0
    , FTL50
    , FTL40
    , FTL30
    , FTL20
    , FTL10
    , FT0
    , FTR10
    , FTR20
    , FTR30
    , FTR40 //10
    , FTR50
    , FRT30
    , FRT20
    , FRT10
    , FR0
    , FRB10
    , FRB20
    , FRB30
    , FBR50
    , FBR40 //20
    , FBR30
    , FBR20
    , FBR10
    , FB0
    , FBL10
    , FBL20
    , FBL30
    , FBL40
    , FBL50
    , FLB30 //30
    , FLB20
    , FLB10
    , FL0
    , FLT10
    , FLT20
    , FLT30
    , FLT
    , FCT
    , FRT
    , FGRT //40
    , GR
    , FGRB
    , FRB
    , FCB
    , FLB
    , FGLB
    , GL
    , FGLT
    , FPLT
    , FPRT //50
    , FPRC
    , FPRB
    , FPLB
    , FPLC
    , FC
};
enum ELine
{
    LINE_TOP
    , LINE_RIGHT
    , LINE_BOTTOM
    , LINE_LEFT
};


class GameObject
{
public:
    double dis;
    double dir;
};


class MobileObject : public GameObject
{
public:
    double dir_chg;
    double dis_chg;
};

class Line : public GameObject
{
public:
    ELine id;
    double dir_chg;
    double dis_chg;
    void print();
};

class Flag : public GameObject
{
public:
    EFlag id;
    Vector2D get_global_coord() const;
    double dir_chg;
    double dis_chg;
    void print();
};

class Ball : public MobileObject
{
public:
    void print();
};

class Player : public MobileObject
{
public:

    bool isMyMate( string team_name )
    {
        if( team.empty() )
            return false;
        return team.compare( team_name ) == 0;
    }

    string team;
    int unum;
    double body_dir;
    double head_dir;
    double point_dir;
    bool is_goalie;

    bool on_tackle;
    bool on_kick;

    void print();

};

// Clases para el coach

class AbsoluteMobileObject
{
public:
	double 	x;
	double 	y;
	double	vx;
	double	vy;
};

class AbsPlayer : public AbsoluteMobileObject
{
public:
	AbsPlayer( );
	AbsPlayer( string team,
				int unum,
				double x,
				double y,
				double vx,
				double vy,
				int body_angle,
				int neck_angle
				);
	string 	team;
	int		unum;
	int 	body_angle;  
	int  	neck_angle; 
	
};

class AbsBall : public AbsoluteMobileObject
{
public:	
};

#endif //GAME_OBJECT_H
