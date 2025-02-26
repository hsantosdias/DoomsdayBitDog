#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "sensores.h"

// Constantes para simulação realista
#define SIMULACAO_INTERVALO_TEMP_MIN 18.0f
#define SIMULACAO_INTERVALO_TEMP_MAX 32.0f
#define SIMULACAO_INTERVALO_UMID_MIN 40.0f
#define SIMULACAO_INTERVALO_UMID_MAX 90.0f
#define SIMULACAO_VAR_TEMP 0.3f
#define SIMULACAO_VAR_UMID 1.0f

// Variáveis estáticas para manter estado entre chamadas
static float ultima_temp = 25.0f;
static float ultima_umid = 65.0f;
static float ultima_som = 45.0f;
static float ultima_lum = 500.0f;
static int contagem_mov = 0;
static int status_chama = 0;

// Função auxiliar para limitar valores entre mínimo e máximo
float limitar(float valor, float min, float max) {
    if (valor < min) return min;
    if (valor > max) return max;
    return valor;
}

// Função para gerar variação aleatória mais realista
float variacaoAleatoria(float base, float variacao) {
    // Gera um valor entre -variacao e +variacao
    return base + ((float)rand() / (float)RAND_MAX * 2.0f * variacao) - variacao;
}

// Função que simula a leitura da temperatura com variação realista
// Função para obter a temperatura do sensor interno do RP2040
float obterTemperatura() {
    const float VOLTAGE_REF = 3.3;      // Referência de tensão do ADC (3.3V)
    const float ADC_RESOLUTION = 4095.0; // Resolução do ADC de 12 bits (0 a 4095)
    const float TEMP_COEFFICIENT = 27.0; // Offset de temperatura (27°C a 0.706V)
    const float VOLTAGE_AT_27 = 0.706;  // Tensão de saída a 27°C
    const float SLOPE = 0.001721;       // Variação de tensão por grau Celsius

    adc_select_input(4); // Canal 4 é o sensor interno de temperatura
    uint16_t adc_raw = adc_read();
    float adc_voltage = (adc_raw / ADC_RESOLUTION) * VOLTAGE_REF;
    float temperatura = TEMP_COEFFICIENT - (adc_voltage - VOLTAGE_AT_27) / SLOPE;

    return temperatura;
}


// Função que simula a leitura da umidade com variação realista
float obterUmidade(void) {
    // Varia no máximo SIMULACAO_VAR_UMID pontos para mais ou para menos
    float nova_umid = variacaoAleatoria(ultima_umid, SIMULACAO_VAR_UMID);
    
    // Limita os valores dentro do intervalo realista
    nova_umid = limitar(nova_umid, SIMULACAO_INTERVALO_UMID_MIN, SIMULACAO_INTERVALO_UMID_MAX);
    
    // Correlação inversa com temperatura (quando temperatura sobe, umidade tende a cair)
    if (ultima_temp > 25.0f) {
        nova_umid -= 0.2f * (ultima_temp - 25.0f);
    }
    
    // Atualiza o último valor
    ultima_umid = nova_umid;
    
    return nova_umid;
}

// Função que exibe a temperatura no terminal.
void mostrarTemperatura(float tempMedida) {
    printf("Temperatura: %.2f°C\n", tempMedida);
}

// Função que exibe a umidade no terminal.
void mostrarUmidade(float umidMedida) {
    printf("Umidade: %.2f%%\n", umidMedida);
}

// Função que apresenta as medições de temperatura e umidade, chamando as funções específicas.
void apresentarMedicoes(float tempMedida, float umidMedida) {
    mostrarTemperatura(tempMedida);
    mostrarUmidade(umidMedida);
}

// Função que simula a obtenção de uma posição geográfica com variação muito sutil
PosicaoGeografica obterPosicaoGeografica(void) {
    PosicaoGeografica pos;
    
    // Coordenadas para a região de Vitória da Conquista - BA
    // 14° 51′ 57″ S, 40° 50′ 20″ O
    pos.latitude.graus = 14;
    pos.latitude.minutos = 51;
    pos.latitude.segundos = 57.0f + (((float)rand() / (float)RAND_MAX) * 0.2f - 0.1f);
    
    pos.longitude.graus = 40;
    pos.longitude.minutos = 50;
    pos.longitude.segundos = 20.0f + (((float)rand() / (float)RAND_MAX) * 0.2f - 0.1f);
    
    return pos;
}

// Função que exibe a posição geográfica no terminal.
void exibirPosicaoGeografica(PosicaoGeografica pos) {
    printf("Posição: %d°%d'%.2f\" S, %d°%d'%.2f\" O\n", 
           pos.latitude.graus, pos.latitude.minutos, pos.latitude.segundos,
           pos.longitude.graus, pos.longitude.minutos, pos.longitude.segundos);
}

// NOVOS SENSORES COM COMPORTAMENTO MAIS REALISTA


// Função para obter o nível de som (Microfone Interno)
float obterNivelSom() {
    adc_select_input(ADC_CANAL_MICROFONE);
    uint16_t resultado = adc_read();
    const float conversao = 3.3f / (1 << 12);
    float tensao = resultado * conversao;
    float nivel_som = tensao * 100.0f;  // Ajuste de escala conforme necessário
    return nivel_som;
}

// Simula um sensor de movimento com padrão mais realista
int detectarMovimento(void) {
    // Comportamento padrão: o movimento tende a continuar por um tempo
    // e depois parar por um período maior
    
    if (contagem_mov > 0) {
        // Se estiver contando movimento, decrementa o contador
        contagem_mov--;
        return 1;
    } else if (rand() % 20 == 0) {
        // 5% de chance de detectar movimento quando não há
        contagem_mov = 2 + (rand() % 5); // Movimento por 2-6 ciclos
        return 1;
    }
    
    return 0;
}

// Simula um detector de chama com comportamento realista
int detectarChama(void) {
    // Uma chama, se detectada, tende a permanecer por algum tempo
    
    if (status_chama > 0) {
        // Se a chama está ativa, há 10% de chance de apagar
        if (rand() % 10 == 0) {
            status_chama = 0;
            return 0;
        }
        return 1;
    } else {
        // Se não há chama, há 0.5% de chance de surgir uma
        if (rand() % 200 == 0) {
            status_chama = 1;
            return 1;
        }
        return 0;
    }
}

// Simula um sensor de luminosidade com variação baseada no tempo
float obterLuminosidade(void) {
    // Iluminação ambiente normal: 300-700 lux
    // Variação natural suave
    float variacao = ((float)rand() / (float)RAND_MAX) * 20.0f - 10.0f;
    float nova_lum = ultima_lum + variacao;
    
    // Limites realistas
    nova_lum = limitar(nova_lum, 100.0f, 1000.0f);
    
    ultima_lum = nova_lum;
    return nova_lum;
}

// Exibe o nível de som no terminal.
void mostrarNivelSom(float nivelSom) {
    printf("Nível de som: %.2f dB\n", nivelSom);
}

// Exibe se há movimento detectado.
void mostrarMovimento(int movimento) {
    printf("Movimento detectado: %s\n", (movimento) ? "Sim" : "Não");
}

// Exibe se há chama detectada.
void mostrarChama(int chama) {
    printf("Chama detectada: %s\n", (chama) ? "Sim" : "Não");
}

// Exibe o valor de luminosidade no terminal.
void mostrarLuminosidade(float luminosidade) {
    printf("Luminosidade: %.2f lux\n", luminosidade);
}

// Agrupa a apresentação dos novos sensores
void apresentarNovosSensores(void) {
    float nivelSom = obterNivelSom();
    int movimento = detectarMovimento();
    int chama = detectarChama();
    float luminosidade = obterLuminosidade();

    mostrarNivelSom(nivelSom);
    mostrarMovimento(movimento);
    mostrarChama(chama);
    mostrarLuminosidade(luminosidade);
}