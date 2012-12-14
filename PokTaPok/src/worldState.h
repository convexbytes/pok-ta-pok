#ifndef WORLDSTATE_H
#define WORLDSTATE_H
#include "gameObject.h"
#include "observation.h"
#include "geometry.h"
#include "localizationMethods.h"

class WorldPlayer
{
public:
    Vector2D global_position;
    short body_angle;
    char side;
    short unum;
};

class BallState // : public Ball
{
public:
    Vector2D global_position;
};

class WorldState
{
public:
    WorldPlayer players[21];
    WorldPlayer me;
    BallState ball;
    //PlayMode play_mode;
};

#endif // WORLDSTATE_H
