#ifndef MONTECARLOLOCALIZATION_H
#define MONTECARLOLOCALIZATION_H
#define NUM_PARTICULAS 300
//#define MONT_PI 3.141592653589793
#include "gameObject.h"
#include "vector"
#include "gameData.h" //arreglar esto...

class Particula
{
public:
    double x;
    double y;
    double theta;
};

class Control
{    
public:
    double dash_power;
    double turn_angle;
};

class MontecarloLocalization
{
public:
    MontecarloLocalization( GameData * game_data );


    //Particula heaviest_particle;

    void montecarlo_prediction( Particula   * particulas,
                                Control       U,
                                Particula   * particulas_nuevas

                                );

    void montecarlo_correction( Particula       * particulas,
                                vector<Flag>    * banderas,
                                Particula       * particulas_nuevas
                                );

private:
    GameData * game_data;
    Particula Sample_Motion_Model       ( Control const &   U,
                                          Particula const & particula
                                          );

    double  Landmark_Model_Known_Corresponce( Flag      bandera,
                                              Particula pose
                                              );

    double  prob_normal_distribution     ( double x,
                                           double stdev
                                           );

    double  sample_normal_distribution   ( double var
                                           ); //Recibe una varianza y regresa un valor del dominio de la normal.


    Vector2D modelo_aceleracion( double power,
                                                         double dir,
                                                         double stamina,
                                                         double effort,
                                                         double body_angle
                                                         );
    Vector2D modelo_aceleracion_gomez( double dash_power,
                                       double stamina,
                                       double effort,
                                       double body_angle);




};

#endif // MONTECARLOLOCALIZATION_H
