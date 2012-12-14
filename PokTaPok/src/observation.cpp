#include "observation.h"
#include "gameObject.h"
#include <vector>
#include <string>
#include <stdio.h>


SenseObs::SenseObs()
{
    type = OBS_SEE ;
    time = UNDEFINED_NUMBER;
    view_mode_quality = (ViewModeQuality)UNDEFINED_NUMBER;
    view_mode_width = (ViewModeWidth)UNDEFINED_NUMBER;
    stamina = UNDEFINED_NUMBER;
    effort = UNDEFINED_NUMBER;
    stamina_capacity = UNDEFINED_NUMBER;
    speed_amount = UNDEFINED_NUMBER;
    speed_direction = UNDEFINED_NUMBER;
    head_angle = UNDEFINED_NUMBER;
    kick = UNDEFINED_NUMBER;
    dash = UNDEFINED_NUMBER;
    turn = UNDEFINED_NUMBER;
    say = UNDEFINED_NUMBER;
    turn_neck = UNDEFINED_NUMBER;
    catchh = UNDEFINED_NUMBER;
    move = UNDEFINED_NUMBER;
    change_view = UNDEFINED_NUMBER;
    arm_movable = UNDEFINED_NUMBER;
    arm_expires = UNDEFINED_NUMBER;
    arm_target_1 = UNDEFINED_NUMBER;
    arm_target_2 = UNDEFINED_NUMBER;
    arm_count = UNDEFINED_NUMBER;
    focus_target_side = 'n';
    focus_target_number = UNDEFINED_NUMBER;
    focus_count = UNDEFINED_NUMBER;
    tackle_expires = UNDEFINED_NUMBER;
    tackle_count = UNDEFINED_NUMBER;
    collision = COLLISION_NONE;
    foul_charged = UNDEFINED_NUMBER;
    foul_card = FCARD_NONE;
}

void SenseObs::print()
{
    printf("\narm_count: %d",  this->arm_count );
    printf("\narm_expires: %d",  this->arm_expires );
    printf("\narm_movable: %d",  this->arm_movable );
    printf("\narm_target_1: %d",  this->arm_target_1 );
    printf("\narm_target_2: %d",  this->arm_target_2 );
    printf("\ncatchh: %d",  this->catchh );
    printf("\nchange_view: %d",  this->change_view );
    printf("\ncollision: %d",  this->collision );
    printf("\ndash: %d",  this->dash );
    printf("\nfocus_count: %d",  this->focus_count );
    printf("\nfocus_target_number: %d",  this->focus_target_number );
    printf("\nfocus_target_side: %c",  this->focus_target_side );
    printf("\nfoul_card: %d",  this->foul_card );
    printf("\nfoul_charged: %d",  this->foul_charged );
    printf("\nhead_angle: %d",  this->head_angle );
    printf("\nkick: %d",  this->kick );
    printf("\nmove: %d",  this->move );
    printf("\nsay: %d",  this->say );
    printf("\nspeed_amount: %f",  this->speed_amount );
    printf("\nspeed_direction: %f",  this->speed_direction );
    printf("\nstamina_capacity: %d",  this->stamina_capacity );
    printf("\nstamina_effort: %f",  this->effort );
    printf("\nstamina_stamina: %f",  this->stamina );
    printf("\ntackle_count: %d",  this->tackle_count );
    printf("\ntackle_expires: %d",  this->tackle_expires );
    printf("\ntime: %d",  this->time );
    printf("\nturn: %d",  this->turn );
    printf("\nturn_neck: %d",  this->turn_neck );
    printf("\ntype: %d",  this->type );
    printf("\nview_mode_quality: %d",  this->view_mode_quality );
    printf("\nview_mode_width: %d",  this->view_mode_width );
}

SeeObs::SeeObs()
{
    type = OBS_SEE;
    this->ball_visible = false;
    this->time = UNDEFINED_NUMBER;
    this->players.clear();
    this->flags.clear();
    this->recognized_flags.clear();
}

void SeeObs::add_player(Player player)
{
    players.push_back( player);
}

void SeeObs::add_flag(Flag flag)
{
    flags.push_back( flag );
}
void SeeObs::add_line(Line line)
{
    lines.push_back( line );
}
void SeeObs::add_ball(Ball ball)
{
    this->ball = ball;
    this->ball_visible = true;
}


bool SeeObs::ball_is_visible()
{
    return ball_visible;
}
int SeeObs::num_recognized_flags()
{
    int count, i, size;
    size = flags.size();
    count = 0;
    for(i = 0; i < size; i++)
    {
        if( flags[i].id != FLAG_UNKNOWN )
            count++;
    }
    return count;
}
int SeeObs::num_recognized_lines()
{
    return lines.size();
}
