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

#endif // BITDOG_SENSORMATRIX_H
