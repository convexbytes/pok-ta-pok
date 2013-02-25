#include "sensor.h"
#include "gameObject.h"
#include <vector>
#include <string>
#include <stdio.h>


BodySensor::BodySensor()
{
    time = 0;
    view_mode_quality = HIGH;
    view_mode_width = NORMAL;
    stamina = 8000.0;
    effort = 1.0;
    stamina_capacity = 130600;
    speed_amount = 0.0;
    speed_direction = 0.0;
    head_angle = 0.0;
    kick = 0;
    dash = 0;
    turn = 0;
    say = 0;
    turn_neck = 0;
    catchh = 0;
    move = 0;
    change_view = 0;
    arm_movable = 0;
    arm_expires = 0;
    arm_target_1 = 0;
    arm_target_2 = 0;
    arm_count = 0;
    focus_target_side = 'n';
    focus_target_number = 0;
    focus_count = 0;
    tackle_expires = 0;
    tackle_count = 0;
    collision = COLLISION_NONE;
    foul_charged = 0;
    foul_card = FCARD_NONE;
}

void BodySensor::print()
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
    printf("\nview_mode_quality: %d",  this->view_mode_quality );
    printf("\nview_mode_width: %d",  this->view_mode_width );
}

SeeSensor::SeeSensor()
{
    this->ball_visible = false;
    this->time = 0;
    this->players.clear();
    this->flags.clear();
    this->recognized_flags.clear();
}

void SeeSensor::add_player(Player player)
{
    players.push_back( player);
}

void SeeSensor::add_flag(Flag flag)
{
    flags.push_back( flag );
}
void SeeSensor::add_line(Line line)
{
    lines.push_back( line );
}
void SeeSensor::add_ball(Ball ball)
{
    this->ball = ball;
    this->ball_visible = true;
}


bool SeeSensor::ball_is_visible() const
{
    return ball_visible;
}
int SeeSensor::num_recognized_flags()
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
int SeeSensor::num_recognized_lines()
{
    return lines.size();
}
