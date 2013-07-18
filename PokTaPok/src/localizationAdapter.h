#ifndef LOCALIZATIONADAPTER_H
#define LOCALIZATIONADAPTER_H
#include "gameData.h"
#include "gameCommand.h"
//#include "LocalizationAlgorithm.h"
#include "montecarloLocalization.h"

class LocalizationAdapter
{
public:
    LocalizationAdapter( GameData * game_data );
    void updatePos(); //Método que usa el algoritmo de localización para actualizar los datos de game_data

    double getX() const;
    double getY() const;
    double getAngle() const;

private:
    GameData                * game_data;
    AgentCommand            * command_commited; // Command_commited
    //LocalizationAlgorithm   * loc_algorithm;
    MontecarloLocalization  * montecarlo_loc;
    Particula                 particulas[NUM_PARTICULAS];
    Particula                 particulas_nuevas[NUM_PARTICULAS];
    Particula               * p;
    Particula               * p_nuevas;

    double x;
    double y;
    double angle;

    void inicializar_particulas();
};

#endif // LOCALIZATIONADAPTER_H
