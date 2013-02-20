#include "poktapokAgent.h"
#include "formation.h"
#include "analyticalAbility.h"
#include "localizationMethods.h"

PokStateV1::PokStateV1()
{
    play_mode = BEFORE_KICK_OFF;
    ball_is_visible = false;
    M_angle = 0.0;
    M_pos = 0.0;
    M_ball_pos = 0.0;
    M_ball_last_seen_time = 0.0;
}

void PokStateV1::update( const ObsHandler &obs,
                         const AgentCommand &command_commited)
{
    ObservationType obs_type = obs.last_obs_type;

    switch( obs_type )
    {
    case OBS_NONE:
        break;

    case OBS_SEE:
        updateOnSee( obs.last_see );
        break;

    case OBS_SENSE:
        updateOnSense( obs.last_sense );
        break;

    case OBS_HEAR:

        switch( obs.last_hear.sender )
        {
        last_upd_time = obs.last_hear.time;
        case REFEREE:
            play_mode = obs.last_hear_referee.play_mode;
            break;

        case COUCH:
            break;

        case SELF:
            break;

        case OUR:
            break;

        case OPP:
            break;
        }
        break;

    case OBS_MSG:
        break;

    case OBS_INIT:
        updateOnInit( obs.last_init );
        break;

    case OBS_ERROR:
        break;

    case OBS_OK:
        break;

    case OBS_SEE_GLOBAL:
        // No la ocupamos
        break;
    }

    updateOnCommandCommit( command_commited );
}

void PokStateV1::updateOnCommandCommit(const AgentCommand &command_commited)
{
    //
}

void PokStateV1::updateOnInit(const InitObs &init)
{
    play_mode       = init.play_mode;
    field_side      = init.side;
    uniform_number  = init.uniform_number;

}

void PokStateV1::updateOnSee(const SeeObs &see)
{
    int i = 0;
    int j = 0;
    int n = 0;
    int n_flags = see.flags.size();
    double theta = 0.0;
    Vector2D pos( 0.0, 0.0 );
    Vector2D pos_tmp( 0.0, 0.0 );
    Flag const * flag1 = NULL;
    Flag const * flag2 = NULL;
    last_upd_time = see.time;
    this->ball_is_visible = see.ball_is_visible();
    if( see.ball_is_visible() )
    {
        if( see.ball.distance <= 0.7 + 0.3 + 0.085 ) // Reemplazar con los parámetros del servidor
            this->ball_is_kickable = true;
        else
            this->ball_is_kickable = false;
    }


    if( n_flags )
    for( i=0; i<n_flags-1; i++ )
    {
        for( j=i+1; j<n_flags; j++ )
        {

            flag1 = &see.flags.at(i);
            flag2 = &see.flags.at(j);
            pos_tmp = ubicacionBanderaBandera( flag1->distance,
                                           flag1->direction - neckDirGrad(),
                                           flag1->get_global_coord(),
                                           flag2->distance,
                                           flag2->direction - neckDirGrad(),
                                           flag2->get_global_coord() );

            // el método bandera-bandera puede devolver nan
            if( !std::isnan(pos_tmp.x) && !std::isnan(pos_tmp.y) )
            {
                pos += pos_tmp;
                n++;
            }
        }
    }

    if( n )
    {
        pos /= (double)n;
        for( i=0; i<see.flags.size(); i++ )
        {
            flag1 = &see.flags.at(i);
            theta += orientacion_global( flag1->get_global_coord(),
                                        pos,
                                        flag1->direction - neckDirGrad() );
        }
        theta /= (double)see.flags.size();
    }

    this->M_pos = pos;
    this->M_angle = theta;

    if( ballIsVisible() )
    {
        M_ball_last_seen_time = see.time;
        M_ball_pos = pos + Vector2D::fromPolar( see.ball.distance,
                                                Deg2Rad( see.ball.direction - neckDirGrad() - bodyAngle() )
                                                );
    }
    //std::cout << pos.x << " " << pos.y << " " << theta << endl;
}



void PokStateV1::updateOnSense(const SenseObs &sense)
{
    last_upd_time   = sense.time;
    M_neck_dir_grad = sense.head_angle;
}

void PokTaPokAgentV1::do_process( GameData *game_data,
                                  AgentCommand *agent_response,
                                  const AgentCommand *agent_response_commited)
{

    // Guardamos las referencias a los datos del juego

    this->command   = agent_response;
    this->command_c = agent_response_commited;
    this->obs_h     = &game_data->obs_handler;
    this->param     = &game_data->game_parameter;

    command->reset();

    //std::cout << "update... " << std::endl;

    state.update( *obs_h,
                  *command_c );

    // ya no llega a este switch
    //std::cout << "switch" << std::endl;

    switch( state.playMode() )
    {
    case GOALIE_CATCH_BALL_L: on_goalie_catch_ball_l(); break;
    case GOALIE_CATCH_BALL_R: on_goalie_catch_ball_r(); break;
    case BEFORE_KICK_OFF: on_before_kick_off(); break;
    case TIME_OVER: on_time_over(); break;
    case PLAY_ON: on_play_on(); break;
    case KICK_OFF_L: on_kick_off_l(); break;
    case KICK_OFF_R: on_kick_off_r(); break;
    case KICK_IN_L: on_kick_in_l(); break;
    case KICK_IN_R: on_kick_in_r(); break;
    case FREE_KICK_L: on_free_kick_l(); break;
    case FREE_KICK_R: on_free_kick_r(); break;
    case CORNER_KICK_L: on_corner_kick_l(); break;
    case CORNER_KICK_R: on_corner_kick_r(); break;
    case GOAL_KICK_L: on_goal_kick_l(); break;
    case GOAL_KICK_R: on_goal_kick_r(); break;
    case GOAL_L: on_goal_l(); break;
    case GOAL_R: on_goal_r(); break;
    case DROP_BALL: on_drop_ball(); break;
    case OFFSIDE_L: on_offside_l(); break;
    case OFFSIDE_R: on_offside_r(); break;
    case PENALTY_KICK_L: on_penalty_kick_l(); break;
    case PENALTY_KICK_R: on_penalty_kick_r(); break;
    case FIRST_HALF_OVER: on_first_half_over(); break;
    case PAUSE: on_pause(); break;
    case HUMAN_JUDGE: on_human_judge(); break;
    case FOUL_L: on_foul_l(); break;
    case FOUL_R: on_foul_r(); break;
    case FOUL_CHARGE_L: on_foul_charge_l(); break;
    case FOUL_CHARGE_R: on_foul_charge_r(); break;
    case FOUL_PUSH_L: on_foul_push_l(); break;
    case FOUL_PUSH_R: on_foul_push_r(); break;
    case FOUL_MULTIPLE_ATTACK_L: on_foul_multiple_attack_l(); break;
    case FOUL_MULTIPLE_ATTACK_R: on_foul_multiple_attack_r(); break;
    case FOUL_BALLOUT_L: on_foul_ballout_l(); break;
    case FOUL_BALLOUT_R: on_foul_ballout_r(); break;
    case BACK_PASS_L: on_back_pass_l(); break;
    case BACK_PASS_R: on_back_pass_r(); break;
    case FREE_KICK_FAULT_L: on_free_kick_fault_l(); break;
    case FREE_KICK_FAULT_R: on_free_kick_fault_r(); break;
    case CATCH_FAULT_L: on_catch_fault_l(); break;
    case CATCH_FAULT_R: on_catch_fault_r(); break;
    case INDIRECT_FREE_KICK_L: on_indirect_free_kick_l(); break;
    case INDIRECT_FREE_KICK_R: on_indirect_free_kick_r(); break;
    case PENALTY_SETUP_L: on_penalty_setup_l(); break;
    case PENALTY_SETUP_R: on_penalty_setup_r(); break;
    case PENALTY_READY_L: on_penalty_ready_l(); break;
    case PENALTY_READY_R: on_penalty_ready_r(); break;
    case PENALTY_TAKEN_L: on_penalty_taken_l(); break;
    case PENALTY_TAKEN_R: on_penalty_taken_r(); break;
    case PENALTY_MISS_L: on_penalty_miss_l(); break;
    case PENALTY_MISS_R: on_penalty_miss_r(); break;
    case PENALTY_SCORE_L: on_penalty_score_l(); break;
    case PENALTY_SCORE_R: on_penalty_score_r(); break;
    case PENALTY_ONFIELD_L: on_penalty_onfield_l(); break;
    case PENALTY_ONFIELD_R: on_penalty_onfield_r(); break;
    case PENALTY_FOUL_R: on_penalty_foul_r(); break;
    case PENALTY_FOUL_L: on_penalty_foul_l(); break;
    case PENALTY_WINNER_L: on_penalty_winner_l(); break;
    case PENALTY_WINNER_R: on_penalty_winner_r(); break;
    case PENALTY_DRAW: on_penalty_draw(); break;
    default:
        break;
    }

}

bool PokTaPokAgentV1::ballIsKickable()
{
    double kickableArea =
            param->server_param.kickable_margin +
            param->server_param.player_size +
            param->server_param.ball_size;

     return ( state.ballIsVisible() &&  obs_h->last_see.ball.distance < kickableArea );

}


bool PokTaPokAgentV1::iHaveTheBall()
{
    // Por ahora el único criterio que tenemos para determinar si tengo el balón
    // es saber si está dentro del área pateable
    return ballIsKickable();
}

void PokTaPokAgentV1::on_before_kick_off()
{
    Vector2D const * position;

        switch( state.unum() )
        {
        case 1:
            position = &Formation::BeforeKickOff::P1;
            break;
        case 2:
            position = &Formation::BeforeKickOff::P2;
            break;
        case 3:
            position = &Formation::BeforeKickOff::P3;
            break;
        case 4:
            position = &Formation::BeforeKickOff::P4;
            break;
        case 5:
            position = &Formation::BeforeKickOff::P5;
            break;
        case 6:
            position = &Formation::BeforeKickOff::P6;
            break;
        case 7:
            position = &Formation::BeforeKickOff::P7;
            break;
        case 8:
            position = &Formation::BeforeKickOff::P8;
            break;
        case 9:
            position = &Formation::BeforeKickOff::P9;
            break;
        case 10:
            position = &Formation::BeforeKickOff::P10;
            break;
        case 11:
            position = &Formation::BeforeKickOff::P11;
            break;
        }

    if( state.side() == 'l' )
        command->append_move( position->x,
                              position->y );
    else
        command->append_move( - position->x,
                              position->y );

}

void PokTaPokAgentV1::on_play_on()
{
    if( iHaveTheBall() )
    {
        attackBallStrategy();
    }
    else if( myTeamHasTheBall() )
    {
        attackNoBallStrategy();
    }
    else
        defendStrategy();

    executeActions();
}

void PokTaPokAgentV1::on_time_over(){}

void PokTaPokAgentV1::on_back_pass_l(){}

void PokTaPokAgentV1::on_back_pass_r(){}

void PokTaPokAgentV1::on_catch_fault_l(){}

void PokTaPokAgentV1::on_catch_fault_r(){}

void PokTaPokAgentV1::on_corner_kick_l(){}

void PokTaPokAgentV1::on_corner_kick_r(){}

void PokTaPokAgentV1::on_drop_ball(){}

void PokTaPokAgentV1::on_first_half_over(){}

void PokTaPokAgentV1::on_foul_ballout_l(){}

void PokTaPokAgentV1::on_foul_ballout_r(){}

void PokTaPokAgentV1::on_foul_charge_l(){}

void PokTaPokAgentV1::on_foul_charge_r(){}

void PokTaPokAgentV1::on_foul_l(){}

void PokTaPokAgentV1::on_foul_r(){}

void PokTaPokAgentV1::on_foul_multiple_attack_l(){}

void PokTaPokAgentV1::on_foul_multiple_attack_r(){}

void PokTaPokAgentV1::on_foul_push_l(){}

void PokTaPokAgentV1::on_foul_push_r(){}

void PokTaPokAgentV1::on_free_kick_fault_l(){}

void PokTaPokAgentV1::on_free_kick_fault_r(){}

void PokTaPokAgentV1::on_free_kick_l()
{

}

void PokTaPokAgentV1::on_free_kick_r()
{

}

void PokTaPokAgentV1::on_goalie_catch_ball_l()
{

}

void PokTaPokAgentV1::on_goalie_catch_ball_r()
{

}

void PokTaPokAgentV1::on_goal_kick_l()
{

}

void PokTaPokAgentV1::on_goal_kick_r()
{

}

void PokTaPokAgentV1::on_goal_l()
{

}

void PokTaPokAgentV1::on_goal_r()
{

}

void PokTaPokAgentV1::on_human_judge()
{

}

void PokTaPokAgentV1::on_indirect_free_kick_l()
{

}

void PokTaPokAgentV1::on_indirect_free_kick_r()
{

}

void PokTaPokAgentV1::on_kick_in_l()
{

}

void PokTaPokAgentV1::on_kick_in_r(){}

void PokTaPokAgentV1::on_kick_off_l(){}

void PokTaPokAgentV1::on_kick_off_r(){}

void PokTaPokAgentV1::on_offside_l(){}

void PokTaPokAgentV1::on_offside_r(){}

void PokTaPokAgentV1::on_pause(){}

void PokTaPokAgentV1::on_penalty_setup_l(){}
void PokTaPokAgentV1::on_penalty_setup_r(){}
void PokTaPokAgentV1::on_penalty_ready_l(){}
void PokTaPokAgentV1::on_penalty_ready_r(){}
void PokTaPokAgentV1::on_penalty_taken_l(){}
void PokTaPokAgentV1::on_penalty_taken_r(){}
void PokTaPokAgentV1::on_penalty_miss_l(){}
void PokTaPokAgentV1::on_penalty_miss_r(){}
void PokTaPokAgentV1::on_penalty_score_l(){}
void PokTaPokAgentV1::on_penalty_score_r(){}
void PokTaPokAgentV1::on_penalty_onfield_l(){}
void PokTaPokAgentV1::on_penalty_onfield_r(){}
void PokTaPokAgentV1::on_penalty_foul_r(){}
void PokTaPokAgentV1::on_penalty_foul_l(){}
void PokTaPokAgentV1::on_penalty_winner_l(){}
void PokTaPokAgentV1::on_penalty_winner_r(){}
void PokTaPokAgentV1::on_penalty_draw(){}
void PokTaPokAgentV1::on_penalty_kick_l(){}
void PokTaPokAgentV1::on_penalty_kick_r(){}


void PokTaPokAgentV1::attackBallStrategy()
{

}

bool PokTaPokAgentV1::myTeamHasTheBall()
{
    /*
    int i;
    bool loTenemos = false;
    Vector2D pos_player;
    Vector2D pos_ball;
    Player * player;
    double margin = 1.0;
    if( obs_h->last_see.ball_is_visible() )
    {
        Ball & ball = obs_h->last_see.ball;
        pos_ball = Vector2D::fromPolar( ball.distance, Deg2Rad( ball.direction) );
        for ( i=0; i < obs_h->last_see.players.size() && loTenemos == false; i++)
        {
            player = & obs_h->last_see.players.at(i);
            if( player->isMyMate( TEAM_NAME ) )
            {
                pos_player = Vector2D::fromPolar( player->distance, Deg2Rad(player->direction) );
                if( pos_player.distance( pos_ball ) < margin )
                    loTenemos = true;
            }
        }
    }
    return loTenemos;*/
    return false;
}

void PokTaPokAgentV1::attackNoBallStrategy()
{

}

void PokTaPokAgentV1::defendStrategy()
{
    //double turn;

    // Determinamos si debemos buscar el balón
    // Por ahora solo nos basamos en la visibilidad del balón
    // En el caso de que alguna jugada esté en curso, puede ser no necesario buscarlo
    M_Search_Ball.M_on = !state.ballIsVisible();



    //if( M_go_to_point.M_on == false )
    // Determinamos si debemos ir tras el balón
    /*
    Player *player;
    bool imTheClosestMateToBall = true;
    double distance = obs_h->last_see.ball.distance;
    int i;
    //...
    Vector2D ball_pos = Vector2D::fromPolar( obs_h->last_see.ball.distance,
                                             Deg2Rad( obs_h->last_see.ball.direction
                                                      - state.neckDirGrad()
                                                      - state.bodyAngle()
                                                    )
                                           );
    Vector2D ball_pos_rel = Vector2D::fromPolar( obs_h->last_see.ball.distance,
                                             Deg2Rad( obs_h->last_see.ball.direction
                                                    )
                                           );

    for( i=0; i < obs_h->last_see.players.size() && imTheClosestMateToBall; i++ )
    {
        player = &obs_h->last_see.players.at(i);
        //cout << player->team << " " << TEAM_NAME;
        if( player->isMyMate( TEAM_NAME ) )
        {
            if( ball_pos_rel.distance(Vector2D::fromPolar(player->distance, Deg2Rad( player->direction ) ) ) < distance )
                imTheClosestMateToBall = false;
        }

    }


    if( imTheClosestMateToBall )
    {
        //printf( "going to ppoint!");
        if( !M_go_to_point.M_on )
        {
            //M_go_to_point.M_on = true;
            //M_go_to_point.M_point = ball_pos - state.pos();
            //M_go_to_point.M_start_time = obs_h->last_obs_time;
            //M_go_to_point.M_expire_time = state.pos().distance(ball_pos)/0.80;
            M_go_to_ball.M_on = true;
        }

    }
    else
    {
        // Find foe and mark it!
    }

    //Vector2D defenseField( 0.0, 0.0 );
    */
}

void PokTaPokAgentV1::searchBall()
{
    int last_see_time = obs_h->last_see.time;
    if( state.time() == last_see_time )
        command->append_turn( 90 );
}

void PokTaPokAgentV1::goToPoint()
{
    /*

    Falta definir los campos de atracción y repulsión

    double angle_between;
    if ( M_go_to_point.M_on == false)
        return;

    if( state.pos().distance( M_go_to_point.M_point ) < 1.2 )
        M_go_to_point.M_on = false;

    angle_between = state.pos().angleBetween( M_go_to_point.M_point );
    if( fabs(angle_between) < Deg2Rad(10) )
        command->append_dash( 80 );
    else
        command->append_turn( Rad2Deg( angle_between ) );
        */
}

void PokTaPokAgentV1::goToBall()
{
    Ball & ball = obs_h->last_see.ball;

    if( ball.distance < 1 )
        return;

    if( abs( ball.direction < 10) )
    {
        if( ball.distance > 2 )
            command->append_dash( 90 );
    }
    else
        command->append_turn( ball.direction );
}

void PokTaPokAgentV1::followBall()
{
    if( state.ballIsVisible() )
    {
        command->append_turn_neck( obs_h->last_see.ball.direction );
    }
}

void PokTaPokAgentV1::executeActions()
{
    /*if( M_go_to_point.M_on )
    {
        goToPoint();

    }

    else if( M_go_to_ball.M_on )
    {
        goToBall();
    }
    */
    if( M_Search_Ball.M_on )
    {
        searchBall();
    }


    followBall();

}
