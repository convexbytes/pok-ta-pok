#include "gameObject.h"
#include "geometry.h"
#include <string>
#include <iostream>
using namespace std;

Vector2D Flag::get_global_coord()
{
    Vector2D coord;
    switch (this->id)
    {
        case FTL50: coord.x = FTL50_X; coord.y = FTL50_Y; break;
        case FTL40: coord.x = FTL40_X; coord.y = FTL40_Y; break;
        case FTL30: coord.x = FTL30_X; coord.y = FTL30_Y; break;
        case FTL20: coord.x = FTL20_X; coord.y = FTL20_Y; break;
        case FTL10: coord.x = FTL10_X; coord.y = FTL10_Y; break;
        case FT0: coord.x = FT0_X; coord.y = FT0_Y; break;
        case FTR10: coord.x = FTR10_X; coord.y = FTR10_Y; break;
        case FTR20: coord.x = FTR20_X; coord.y = FTR20_Y; break;
        case FTR30: coord.x = FTR30_X; coord.y = FTR30_Y; break;
        case FTR40: coord.x = FTR40_X; coord.y = FTR40_Y; break;
        case FTR50: coord.x = FTR50_X; coord.y = FTR50_Y; break;
        case FRT30: coord.x = FRT30_X; coord.y = FRT30_Y; break;
        case FRT20: coord.x = FRT20_X; coord.y = FRT20_Y; break;
        case FRT10: coord.x = FRT10_X; coord.y = FRT10_Y; break;
        case FR0: coord.x = FR0_X; coord.y = FR0_Y; break;
        case FRB10: coord.x = FRB10_X; coord.y = FRB10_Y; break;
        case FRB20: coord.x = FRB20_X; coord.y = FRB20_Y; break;
        case FRB30: coord.x = FRB30_X; coord.y = FRB30_Y; break;
        case FBR50: coord.x = FBR50_X; coord.y = FBR50_Y; break;
        case FBR40: coord.x = FBR40_X; coord.y = FBR40_Y; break;
        case FBR30: coord.x = FBR30_X; coord.y = FBR30_Y; break;
        case FBR20: coord.x = FBR20_X; coord.y = FBR20_Y; break;
        case FBR10: coord.x = FBR10_X; coord.y = FBR10_Y; break;
        case FB0: coord.x = FB0_X; coord.y = FB0_Y; break;
        case FBL10: coord.x = FBL10_X; coord.y = FBL10_Y; break;
        case FBL20: coord.x = FBL20_X; coord.y = FBL20_Y; break;
        case FBL30: coord.x = FBL30_X; coord.y = FBL30_Y; break;
        case FBL40: coord.x = FBL40_X; coord.y = FBL40_Y; break;
        case FBL50: coord.x = FBL50_X; coord.y = FBL50_Y; break;
        case FLB30: coord.x = FLB30_X; coord.y = FLB30_Y; break;
        case FLB20: coord.x = FLB20_X; coord.y = FLB20_Y; break;
        case FLB10: coord.x = FLB10_X; coord.y = FLB10_Y; break;
        case FL0: coord.x = FL0_X; coord.y = FL0_Y; break;
        case FLT10: coord.x = FLT10_X; coord.y = FLT10_Y; break;
        case FLT20: coord.x = FLT20_X; coord.y = FLT20_Y; break;
        case FLT30: coord.x = FLT30_X; coord.y = FLT30_Y; break;
        case FLT: coord.x = FLT_X; coord.y = FLT_Y; break;
        case FCT: coord.x = FCT_X; coord.y = FCT_Y; break;
        case FRT: coord.x = FRT_X; coord.y = FRT_Y; break;
        case FGRT: coord.x = FGRT_X; coord.y = FGRT_Y; break;
        case GR: coord.x = GR_X; coord.y = GR_Y; break;
        case FGRB: coord.x = FGRB_X; coord.y = FGRB_Y; break;
        case FRB: coord.x = FRB_X; coord.y = FRB_Y; break;
        case FCB: coord.x = FCB_X; coord.y = FCB_Y; break;
        case FLB: coord.x = FLB_X; coord.y = FLB_Y; break;
        case FGLB: coord.x = FGLB_X; coord.y = FGLB_Y; break;
        case GL: coord.x = GL_X; coord.y = GL_Y; break;
        case FGLT: coord.x = FGLT_X; coord.y = FGLT_Y; break;
        case FPLT: coord.x = FPLT_X; coord.y = FPLT_Y; break;
        case FPRT: coord.x = FPRT_X; coord.y = FPRT_Y; break;
        case FPRC: coord.x = FPRC_X; coord.y = FPRC_Y; break;
        case FPRB: coord.x = FPRB_X; coord.y = FPRB_Y; break;
        case FPLB: coord.x = FPLB_X; coord.y = FPLB_Y; break;
        case FPLC: coord.x = FPLC_X; coord.y = FPLC_Y; break;
        case FC: coord.x = FC_X; coord.y = FC_Y; break;
           default:
               coord.x = 0; coord.y = 0;
           break;
    }
    return coord;
}

void Player::print()
{
    cout << endl;
    cout << "team " << this->team << " ";
    cout << "unum " << this->uniform_number << " ";
    cout << "dis " << this->distance << " ";
    cout << "dir " << this->direction << " ";
    cout << "dis_chg " << this->distance_change << " ";
    cout << "dir_chg " << this->direction_change << " ";
    cout << "body_dir " << this->body_direction << " ";
    cout << "neck_dir " << this->neck_direction << " ";
}

void Flag::print()
{
    cout << endl;
    cout << "id " << this->id << " ";
    cout << "dis " << this->distance << " ";
    cout << "dir " << this->direction << " ";
    cout << "dis_chg " << this->distance_change << " ";
    cout << "dir_chg " << this->direction_change;
}

void Line::print()
{
    cout << endl;
    cout << "id " << this->id << " ";
    cout << "dis " << this->distance << " ";
    cout << "dir " << this->direction << " ";
    cout << "dis_chg " << this->distance_change << " ";
    cout << "dir_chg " << this->direction_change;
}

void Ball::print()
{
}

AbsPlayer::AbsPlayer( )
{
	x = y = vx = vy = body_angle = neck_angle = unum = 0;
	team = "";
}
AbsPlayer::	AbsPlayer( string team,
				int unum,
				double x,
				double y,
				double vx,
				double vy,
				int body_angle,
				int neck_angle
				)
{
	this->team.assign( team );
	this->unum = unum;
	this->x = x;
	this->y = y;
	this->vx = vx;
	this->vy = vy;
	this->body_angle = body_angle;
	this->neck_angle = neck_angle;
}
