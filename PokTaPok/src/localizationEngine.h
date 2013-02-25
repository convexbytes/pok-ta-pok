#ifndef LOCALIZATIONENGINE_H
#define LOCALIZATIONENGINE_H
#include "gameData.h"
#include "gameCommand.h"
#include "montecarloLocalization.h"

class LocalizationEngine
{
public:
    LocalizationEngine( GameData * game_data,
                        AgentCommand * command_commited );


    double x() const { return M_x; }
    double y() const { return M_y; }
    double angle() const { return M_angle; }

    //Método que usa el algoritmo de localización para actualizar la pose.
    void updatePos();


private:
    GameData                * game_data;
    AgentCommand            * command_commited; // Command_commited
    //LocalizationAlgorithm   * loc_algorithm;
    MontecarloLocalization  * montecarlo_loc;
    Particula                 particulas[NUM_PARTICULAS];
    Particula                 particulas_nuevas[NUM_PARTICULAS];
    Particula               * p;
    Particula               * p_nuevas;

    double M_x;
    double M_y;
    double M_angle;
    void inicializar_particulas();
};

#endif // LOCALIZATIONENGINE_H
