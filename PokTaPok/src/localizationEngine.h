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
    LocalizationEngine( GameData * game_data, AgentCommand * agent_response );
    void update_world(); //Método que usa el algoritmo de localización para actualizar los datos de game_data
private:
    GameData                * game_data;
    AgentCommand            * agent_response;
    //LocalizationAlgorithm   * loc_algorithm;
    MontecarloLocalization  * montecarlo_loc;
    Particula                 particulas[NUM_PARTICULAS];
    Particula                 particulas_nuevas[NUM_PARTICULAS];
    Particula               * p;
    Particula               * p_nuevas;
    void inicializar_particulas();
};

#endif // LOCALIZATIONENGINE_H
