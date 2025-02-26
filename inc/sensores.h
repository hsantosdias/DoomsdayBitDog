#ifndef SENSORES_H
#define SENSORES_H

#define ADC_CANAL_MICROFONE 2  // Microfone interno no GPIO 28 (ADC 2)

// Funções de medição e exibição de temperatura e umidade
float obterTemperatura(void);
float obterUmidade(void);
void mostrarTemperatura(float tempMedida);
void mostrarUmidade(float umidMedida);
void apresentarMedicoes(float tempMedida, float umidMedida);

// Tipos e funções para posição geográfica
typedef struct {
    int graus;
    int minutos;
    float segundos;
} Coordenada;

typedef struct {
    Coordenada latitude;
    Coordenada longitude;
} PosicaoGeografica;

PosicaoGeografica obterPosicaoGeografica(void);
void exibirPosicaoGeografica(PosicaoGeografica pos);

// Funções auxiliares
float limitar(float valor, float min, float max);
float variacaoAleatoria(float base, float variacao);

// Sensores ambientais

// Sensor de som: retorna nível em decibéis (dB)
float obterNivelSom(void);
// Sensor de movimento: retorna 1 se movimento detectado, 0 caso contrário
int detectarMovimento(void);
// Detector de chama: retorna 1 se chama detectada, 0 caso contrário
int detectarChama(void);
// Sensor de luminosidade: retorna valor em lux
float obterLuminosidade(void);

// Funções de exibição dos sensores
void mostrarNivelSom(float nivelSom);
void mostrarMovimento(int movimento);
void mostrarChama(int chama);
void mostrarLuminosidade(float luminosidade);

// Função que agrupa a apresentação dos sensores
void apresentarNovosSensores(void);

#endif // SENSORES_H