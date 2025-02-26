#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "hardware/rtc.h"
#include "hardware/adc.h"
#include "BitDog-SensorMatrix.h"

// Definições de constantes para configuração
#define AMOSTRAGEM_MS 1000  // Intervalo entre medições em milissegundos
#define MAX_AMOSTRAS 10     // Número de amostras para média móvel

// Estrutura para armazenar histórico de leituras
typedef struct {
    float valores[MAX_AMOSTRAS];
    int indice;
    int preenchido;
} HistoricoLeituras;

// Instâncias dos históricos de leituras
static HistoricoLeituras hist_temp, hist_umid, hist_som, hist_luz;

// Inicializa um histórico de leituras
void inicializar_historico(HistoricoLeituras* historico) {
    for (int i = 0; i < MAX_AMOSTRAS; i++) {
        historico->valores[i] = 0.0f;
    }
    historico->indice = 0;
    historico->preenchido = 0;
}

// Adiciona uma leitura ao histórico e retorna a média
float adicionar_leitura(HistoricoLeituras* historico, float leitura) {
    historico->valores[historico->indice] = leitura;
    historico->indice = (historico->indice + 1) % MAX_AMOSTRAS;
    
    if (historico->indice == 0) {
        historico->preenchido = 1;
    }
    
    // Calcula a média
    float soma = 0.0f;
    int num_elementos = historico->preenchido ? MAX_AMOSTRAS : historico->indice;
    
    for (int i = 0; i < num_elementos; i++) {
        soma += historico->valores[i];
    }
    
    return soma / num_elementos;
}

// Inicializa os históricos e o ADC
void inicializar_sensores() {
    // Inicialização do ADC
    adc_init(); // Inicializa o ADC
    adc_set_temp_sensor_enabled(true);  // Habilita o sensor interno de temperatura
    adc_select_input(4);                // Canal 4 é o sensor interno de temperatura
    adc_gpio_init(28);  // Microfone - ADC 2

    inicializar_historico(&hist_temp);
    inicializar_historico(&hist_umid);
    inicializar_historico(&hist_som);
    inicializar_historico(&hist_luz);
}

// Obtém as leituras e calcula as médias móveis
DadosSensores obter_dados_sensores() {
    DadosSensores dados;

    dados.temperatura.atual = obterTemperatura();
    dados.temperatura.media = adicionar_leitura(&hist_temp, dados.temperatura.atual);

    dados.umidade.atual = obterUmidade();
    dados.umidade.media = adicionar_leitura(&hist_umid, dados.umidade.atual);

    dados.som.atual = obterNivelSom();
    dados.som.media = adicionar_leitura(&hist_som, dados.som.atual);

    dados.luminosidade.atual = obterLuminosidade();
    dados.luminosidade.media = adicionar_leitura(&hist_luz, dados.luminosidade.atual);

    return dados;
}


// Função para ler o estado do Joystick e determinar a direção
void ler_joystick(char *direcao_x, char *direcao_y) {
    // Leitura do eixo X
    adc_select_input(1); // ADC 1 é o eixo X (GPIO 26)
    uint16_t adc_x = adc_read();

    // Leitura do eixo Y
    adc_select_input(0); // ADC 0 é o eixo Y (GPIO 27)
    uint16_t adc_y = adc_read();

    // Determina a direção no eixo X
    if (adc_x > 3000) {
        snprintf(direcao_x, 10, "Leste");
    } else if (adc_x < 1000) {
        snprintf(direcao_x, 10, "Oeste");
    } else {
        snprintf(direcao_x, 10, "Centro");
    }

    // Determina a direção no eixo Y
    if (adc_y > 3000) {
        snprintf(direcao_y, 10, "Norte");
    } else if (adc_y < 1000) {
        snprintf(direcao_y, 10, "Sul");
    } else {
        snprintf(direcao_y, 10, "Centro");
    }
}
