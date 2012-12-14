#ifndef LOCALIZATIONALGORITHM_H
#define LOCALIZATIONALGORITHM_H
#include "gameData.h"
#include "geometry.h"

//Interface

//Esta clase contendrá todos los datos que generó el algoritmo de localizacion.
class LocalizationAlgorithmResult
{
public:
    Vector2D position;
};

class LocalizationAlgorithm
{
public:
    LocalizationAlgorithm( GameData * game_data) { this->game_data = game_data; }
    virtual LocalizationAlgorithmResult get_localization( int player_unum, char side) = 0;
private:
    virtual void initialize() = 0;

    GameData * game_data;
};

#endif // LOCALIZATIONALGORITHM_H
