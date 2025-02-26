#ifndef BITDOG_SENSORMATRIX_H
#define BITDOG_SENSORMATRIX_H

#include "sensores.h"

// Estrutura para armazenar as leituras e médias
typedef struct {
    float atual;
    float media;
} LeituraSensor;

// Estrutura para armazenar todas as leituras
typedef struct {
    LeituraSensor temperatura;
    LeituraSensor umidade;
    LeituraSensor som;
    LeituraSensor luminosidade;
} DadosSensores;

// Inicialização dos sensores e históricos
void inicializar_sensores();

// Obtém as leituras e calcula as médias móveis
DadosSensores obter_dados_sensores();

// Declaração das funções do joystick
void ler_joystick(char *direcao_x, char *direcao_y);

// Declaração da função haversine
double haversine(double lat1, double lon1, double lat2, double lon2);

#endif // BITDOG_SENSORMATRIX_H
