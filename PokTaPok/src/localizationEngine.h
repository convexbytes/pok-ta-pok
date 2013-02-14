#ifndef LOCALIZATIONENGINE_H
#define LOCALIZATIONENGINE_H
#include "gameData.h"
#include "gameCommand.h"
#include "LocalizationAlgorithm.h"
#include "agentResponse.h"
#include "montecarloLocalization.h"

class LocalizationEngine
{
public:
    LocalizationEngine( GameData * game_data, AgentCommand * command_commited );
    void getNewPos( double & x, double & y, double & body_angle ); //Método que usa el algoritmo de localización para actualizar los datos de game_data
private:
    GameData                * game_data;
    AgentCommand            * command_commited; // Command_commited
    //LocalizationAlgorithm   * loc_algorithm;
    MontecarloLocalization  * montecarlo_loc;
    Particula                 particulas[NUM_PARTICULAS];
    Particula                 particulas_nuevas[NUM_PARTICULAS];
    Particula               * p;
    Particula               * p_nuevas;
    void inicializar_particulas();
};

#endif // LOCALIZATIONENGINE_H
