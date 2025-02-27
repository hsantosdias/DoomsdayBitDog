#include <stdio.h> // Inclui biblioteca padrão de E/S em C (printf, scanf, etc) 
#include <stdlib.h> // Inclui biblioteca padrão de funções em C (malloc, free, etc)
#include <string.h> // Inclui biblioteca de funções de strings em C (strlen, strcmp, etc)
#include <math.h> // Inclui biblioteca de funções matemáticas em C (sin, cos, etc)
#include "pico/stdlib.h" // Inclui biblioteca de funções do Pico (gpio_init, sleep_ms, etc)
#include "hardware/adc.h" // Inclui biblioteca de funções de ADC (adc_init, adc_gpio_init, etc)
#include "hardware/i2c.h" // Inclui biblioteca de funções de I2C (i2c_init, i2c_write_blocking, etc)
#include "hardware/irq.h" // Inclui biblioteca de funções de interrupção (IRQ)
#include "hardware/uart.h" // Inclui biblioteca de funções de UART (uart_init, uart_getc, etc)
#include "hardware/pwm.h" // Inclui biblioteca de funções de PWM (pwm_init, pwm_set_gpio_level, etc)
#include "hardware/gpio.h" // Inclui biblioteca de funções de GPIO (gpio_init, gpio_put, etc)

//bibliotecas adicionais fornecidas inicialmente pelo professor Wilson
#include "inc/ssd1306.h" // Inclui biblioteca de funções do display OLED SSD1306
#include "inc/font.h" // Inclui biblioteca de fontes para o display OLED SSD1306 numeros e letras maiusculas e minusculas
#include "inc/sensores.h" // Inclui biblioteca de funções de sensores (obterTemperatura, obterUmidade, etc)
#include "BitDog-SensorMatrix.h" // Inclui biblioteca de funções de sensores (obterTemperatura, obterUmidade, etc)

//bibliotecas adicional - para manipulação do display de matriz de leds
#include "led_matrix.h" // Inclui biblioteca de funções da matriz de LEDs 5x5

// Trecho para modo BOOTSEL com Botão B
#include "pico/bootrom.h"

// Macro para calcular o tamanho de um array
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

// Definições do display SSD1306 128x64 I2C OLED
// Configuração i2c para o display OLED
#define I2C_PORT i2c1 // Define a porta I2C utilizada - porta 1 da bitdoglab
#define I2C_SDA 14 // Define o pino SDA - GPIO 14
#define I2C_SCL 15 // Define o pino SCL - GPIO 15
#define ENDERECO 0x3C // Endereço do display OLED SSD1306

// Configuração dos Botões e Joystick
#define JOYSTICK_X_PIN 26  // GPIO para eixo X
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y
#define JOYSTICK_PB 22     // GPIO para botão do Joystick (Selecionar)
#define BOTAO_A 5          // GPIO para voltar ao Menu Principal
#define BOTAO_B 6          // GPIO para BOOTSEL


// Definições para UART (Comunicação Serial)
#define UART_ID uart0 // Define a porta UART utilizada - porta 0 da bitdoglab
#define BAUD_RATE 115200 // Define a taxa de transmissão de dados (baud rate) - 115200 bps
#define UART_TX_PIN 0 // Define o pino TX da UART - GPIO 0
#define UART_RX_PIN 1 // Define o pino RX da UART - GPIO 1

// GPIOs para S.O.S
#define BUZZER_PIN 21

// Frequências para notas altas
#define NOTE_A5 880   // Nota alta A5
#define NOTE_C6 1047  // Nota muito alta C6

// Parâmetros para o código Morse do S.O.S
#define DOT_DURATION 200   // Duração de um ponto (200 ms)
#define DASH_DURATION 600  // Duração de um traço (600 ms)
#define PAUSE_DURATION 200 // Pausa entre pontos e traços (200 ms)
#define LETTER_PAUSE 600   // Pausa entre letras (600 ms)

// Definições para LEDs e botões
#define LED_PIN_R 13    // LED Vermelho 
#define LED_PIN_B 12    // LED Azul
#define LED_PIN_G 11    // LED Verde

#define MENU_TIMEOUT_US 60000000  // 30 segundos
#define BUTTON_DEBOUNCE_US 50000  // 50 ms

// Número de opções no Menu Principal
#define NUM_OPCOES_PRINCIPAL 4

// Estrutura do OLED
ssd1306_t ssd;

int limite_som = 50; // Limite inicial de 50%
int historico_som[5] = {0}; // Buffer para média móvel
int indice_som = 0; // Índice do buffer de som

// Prototipagem de Funções para o Menu e Navegação do Menu Principal
void iniciar_oled();
void iniciar_joystick();
void animacao_inicial();
void mostrar_menu();
void navegar_menu();
void voltar_menu_principal();
void opcao_selecionada();
void desenhar_opcoes();
void desenhar_retangulo_selecao();
void desenhar_setas();
void exibir_mensagem(const char *linha1, const char *linha2);
void sinal_sos();

// Prototipação da Função pwm_set_frequency
void pwm_set_frequency(uint slice_num, uint channel, float frequency);


// Prototipação de funções de histórico do menu
typedef struct Menu Menu;
void push_menu(Menu *menu, int count);
void pop_menu();

// Prototipação das funções de ação
void mostrar_temperatura(void);
void mostrar_umidade(void);
void mostrar_luminosidade(void);
void mostrar_posicao(void);
void mostrar_direcao(void);
void mostrar_distancia(void);
void detectar_som(void);
void mostrar_mensagens(void);
void configurar_sistema(void);
void mostrar_informacoes(void);
void mostrar_direcao_joystick(void);
void mostrar_chama(void);
void mostrar_chama_joystick(void);
void mostrar_monoxido(void);

// Prototipação da função modificar_valor
int modificar_valor(int valor_atual, int min, int max);

// Estrutura do Menu – com ponteiro para ação e submenus
struct Menu {
    const char *titulo;
    Menu *submenus;
    int num_submenus;   // Para itens do menu principal, indica quantas opções há no submenu
    void (*acao)(void); // Adição de ponteiro para ação
};

// Histórico de navegação do menu
#define MAX_MENU_HISTORY 10
Menu *menu_history[MAX_MENU_HISTORY];
int menu_history_count[MAX_MENU_HISTORY]; // Armazena a quantidade de opções de cada nível
int menu_history_index = 0;

// Definição dos submenus

// Submenu para Monitoramento Ambiental
Menu submenu_monitoramento[] = {
    {"Temperatura", NULL, 0, mostrar_temperatura},
    {"Umidade",     NULL, 0, mostrar_umidade},
    {"Luminosidade",NULL, 0, mostrar_luminosidade},
    {"Voltar",      NULL, 0, voltar_menu_principal}
};

// Submenu para GeoLocalizacao
Menu submenu_navegacao[] = {
    {"Posicao",     NULL, 0, mostrar_posicao},
    {"Direcao",     NULL, 0, mostrar_direcao_joystick},
    {"Distancia",   NULL, 0, mostrar_distancia},
    {"Voltar",      NULL, 0, voltar_menu_principal}
};

// Submenu para Alertas e Mensagens
Menu submenu_alertas[] = {
    {"Mensagens",   NULL, 0, mostrar_mensagens},
    {"Deteccao Som", NULL, 0, detectar_som},
    {"Sinal S.O.S",   NULL, 0, sinal_sos},    
    {"Voltar",      NULL, 0, voltar_menu_principal}
};

// Submenu para Configurações do Sistema
Menu sistemas_criticos[] = {
    {"Deteccao Chama",     NULL, 0, mostrar_chama_joystick},
    {"Monoxido Carb.", NULL, 0, mostrar_monoxido},
    {"Informacoes",        NULL, 0, mostrar_informacoes},
    {"Voltar",      NULL, 0, voltar_menu_principal}
};

// Menu Principal – o campo num_submenus indica quantos itens o submenu terá
Menu menu_principal[] = {
    {"Info. Ambientais", submenu_monitoramento, ARRAY_SIZE(submenu_monitoramento), NULL},
    {"GPS e Rastreio", submenu_navegacao,     ARRAY_SIZE(submenu_navegacao),     NULL},
    {"Alertas Ativos",submenu_alertas,       ARRAY_SIZE(submenu_alertas),       NULL},
    {"Sistema Critico", sistemas_criticos, ARRAY_SIZE(sistemas_criticos), NULL}
};

// Variáveis globais para navegação
int opcao_atual = 0;
Menu *menu_atual = menu_principal;
int num_opcoes = NUM_OPCOES_PRINCIPAL;
static absolute_time_t last_interaction_time = 0;
const uint32_t TIMEOUT_US = 60000000; // 60 segundos

// Inicializa o OLED
void iniciar_oled() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&ssd, 128, 64, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Animação Inicial
void animacao_inicial() {
    ssd1306_fill(&ssd, false);
    for (int i = 0; i < 128; i += 4) {
        ssd1306_rect(&ssd, i, i, 128 - i * 2, 64 - i * 2, true, false);
        ssd1306_send_data(&ssd);
        sleep_ms(50);
    }
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
}

// Exibe mensagem genérica no OLED
void exibir_mensagem(const char *linha1, const char *linha2) {
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, linha1, 10, 20);
    ssd1306_draw_string(&ssd, linha2, 10, 40);
    ssd1306_send_data(&ssd);
    sleep_ms(5000);
}

// Desenha as opções do menu
void desenhar_opcoes() {
    for (int i = 0; i < num_opcoes; i++) {
        ssd1306_draw_string(&ssd, menu_atual[i].titulo, 5, i * 16 + 4);
    }
}

// Desenha o retângulo de seleção no OLED
void desenhar_retangulo_selecao() {
    ssd1306_rect(&ssd, opcao_atual * 16, 0, 128, 16, true, false);
}

// Desenha setas de navegação
void desenhar_setas() {
    if (num_opcoes > 1) {
        ssd1306_draw_string(&ssd, "x", 125, opcao_atual * 16);
        if (opcao_atual < num_opcoes - 1) {
            ssd1306_draw_string(&ssd, "v", 60, 56);
        }
    }
}

// Mostra o menu atual
void mostrar_menu() {
    ssd1306_fill(&ssd, false);
    sleep_ms(50);

    // Debug para verificar o número de opções atual
    printf("Desenhando menu com %d opcoes\n", num_opcoes);

    // Desenha todas as opções do menu atual
    for (int i = 0; i < num_opcoes; i++) {
        if (menu_atual[i].titulo != NULL) {
            printf("Desenhando opcao %d: %s\n", i, menu_atual[i].titulo);
            ssd1306_draw_string(&ssd, menu_atual[i].titulo, 5, i * 16 + 4);
        }
    }

    // Desenha o retângulo de seleção
    desenhar_retangulo_selecao();

    // Desenha as setas de navegação, se necessário
    if (num_opcoes > 1) {
        if (opcao_atual > 0) {
            ssd1306_draw_string(&ssd, "^", 60, 0);
        }
        if (opcao_atual < num_opcoes - 1) {
            ssd1306_draw_string(&ssd, "v", 60, 56);
        }
    }

    ssd1306_send_data(&ssd);
}



void mostrar_temperatura() {
    absolute_time_t start_time = get_absolute_time();
    while (absolute_time_diff_us(start_time, get_absolute_time()) < 6000000) { // 5 segundos
        
        DadosSensores dados = obter_dados_sensores();
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "%.2f C", dados.temperatura.atual);

        ssd1306_fill(&ssd, false);  // Limpa a tela
        
        // Exibe Temperatura
        ssd1306_draw_string(&ssd, "Temperatura:", 0, 0);
        ssd1306_draw_string(&ssd, buffer, 0, 16);
        
        ssd1306_send_data(&ssd);

        sleep_ms(500); // Atualiza a cada 500 ms para mostrar variação
    }
   // voltar_menu_principal();   // Volta ao menu principal após 3 segundos
   mostrar_menu();  
}





void mostrar_umidade() {
    absolute_time_t start_time = get_absolute_time();
    while (absolute_time_diff_us(start_time, get_absolute_time()) < 1000000) { // 5 segundos
        
        DadosSensores dados = obter_dados_sensores();
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "Umid: %.2f%%", dados.umidade.atual);

        ssd1306_fill(&ssd, false);  // Limpa a tela
        ssd1306_draw_string(&ssd, "Umidade:", 0, 0);
        ssd1306_draw_string(&ssd, buffer, 0, 16);
        ssd1306_send_data(&ssd);

        sleep_ms(500); // Atualiza a cada 500 ms para mostrar variação
    }
    //voltar_menu_principal();   // Volta ao menu principal após 3 segundos
    mostrar_menu();
}


void mostrar_luminosidade() {
    absolute_time_t start_time = get_absolute_time();
    while (absolute_time_diff_us(start_time, get_absolute_time()) < 6000000) { // 3 segundos
        
        DadosSensores dados = obter_dados_sensores();
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "Luz: %.2flux", dados.luminosidade.atual);

        ssd1306_fill(&ssd, false);  // Limpa a tela
        ssd1306_draw_string(&ssd, "Luminosidade:", 0, 0);
        ssd1306_draw_string(&ssd, buffer, 0, 16);
        ssd1306_send_data(&ssd);

        sleep_ms(500); // Atualiza a cada 500 ms para mostrar variação
    }
    //voltar_menu_principal();   // Volta ao menu principal após 3 segundos
    mostrar_menu();
}


void mostrar_direcao_joystick() {
    char direcao_x[10];
    char direcao_y[10];

    absolute_time_t start_time = get_absolute_time();
    while (absolute_time_diff_us(start_time, get_absolute_time()) < 10000000) { // 3 segundos
        
        // Lê o estado do Joystick
        ler_joystick(direcao_x, direcao_y);

        ssd1306_fill(&ssd, false);  // Limpa a tela
        
        // Exibe a Direção do Joystick
        ssd1306_draw_string(&ssd, "Eixo X:", 0, 0);
        ssd1306_draw_string(&ssd, direcao_x, 64, 0);  // Leste/Oeste/Centro
        
        ssd1306_draw_string(&ssd, "Eixo Y:", 0, 16);
        ssd1306_draw_string(&ssd, direcao_y, 64, 16); // Norte/Sul/Centro
        
        ssd1306_send_data(&ssd);

        sleep_ms(500); // Atualiza a cada 500 ms para mostrar variação
    }
    voltar_menu_principal();   // Volta ao menu principal após 3 segundos
}




void mostrar_posicao() {
    absolute_time_t start_time = get_absolute_time();
    while (absolute_time_diff_us(start_time, get_absolute_time()) < 8000000) { // 3 segundos
        
        PosicaoGeografica posicao = obterPosicaoGeografica();

        char buffer_lat[20];
        char buffer_lon[20];
        snprintf(buffer_lat, sizeof(buffer_lat), "%d°%d'%.2f\"", 
                 posicao.latitude.graus, posicao.latitude.minutos, posicao.latitude.segundos);

        snprintf(buffer_lon, sizeof(buffer_lon), "%d°%d'%.2f\"", 
                 posicao.longitude.graus, posicao.longitude.minutos, posicao.longitude.segundos);

        ssd1306_fill(&ssd, false);  // Limpa a tela
        
        // Exibe Latitude
        ssd1306_draw_string(&ssd, "Latitude:", 0, 0);
        ssd1306_draw_string(&ssd, buffer_lat, 0, 16);
        
        // Exibe Longitude
        ssd1306_draw_string(&ssd, "Longitude:", 0, 32);
        ssd1306_draw_string(&ssd, buffer_lon, 0, 48);

        ssd1306_send_data(&ssd);

        sleep_ms(500); // Atualiza a cada 500 ms para mostrar variação
    }
   // voltar_menu_principal();   // Volta ao menu principal após 3 segundos
   mostrar_menu();
}


void mostrar_direcao() {
    char buffer[20];
    // Exemplo básico: Pegando a direção do movimento com base na variação de latitude e longitude
    PosicaoGeografica posicao_atual = obterPosicaoGeografica();
    static PosicaoGeografica posicao_anterior = {0, 0, 0, 0, 0, 0};

    float delta_lat = posicao_atual.latitude.segundos - posicao_anterior.latitude.segundos;
    float delta_lon = posicao_atual.longitude.segundos - posicao_anterior.longitude.segundos;

    if (fabs(delta_lat) > fabs(delta_lon)) {
        if (delta_lat > 0) {
            snprintf(buffer, sizeof(buffer), "Norte");
        } else {
            snprintf(buffer, sizeof(buffer), "Sul");
        }
    } else {
        if (delta_lon > 0) {
            snprintf(buffer, sizeof(buffer), "Leste");
        } else {
            snprintf(buffer, sizeof(buffer), "Oeste");
        }
    }

    posicao_anterior = posicao_atual;

    ssd1306_fill(&ssd, false);  // Limpa a tela
    ssd1306_draw_string(&ssd, "Direcao:", 0, 0);
    ssd1306_draw_string(&ssd, buffer, 0, 16);
    ssd1306_send_data(&ssd);

    sleep_ms(5000);
    voltar_menu_principal();
}




void mostrar_distancia() {
    char buffer[20];

    PosicaoGeografica posicao_atual = obterPosicaoGeografica();
    static PosicaoGeografica posicao_inicial = {14, 51, 56.0, 40, 49, 20.0}; // Coordenadas de referência

    double distancia = haversine(
        posicao_inicial.latitude.graus + posicao_inicial.latitude.minutos / 60.0 + posicao_inicial.latitude.segundos / 3600.0,
        posicao_inicial.longitude.graus + posicao_inicial.longitude.minutos / 60.0 + posicao_inicial.longitude.segundos / 3600.0,
        posicao_atual.latitude.graus + posicao_atual.latitude.minutos / 60.0 + posicao_atual.latitude.segundos / 3600.0,
        posicao_atual.longitude.graus + posicao_atual.longitude.minutos / 60.0 + posicao_atual.longitude.segundos / 3600.0
    );

    snprintf(buffer, sizeof(buffer), "%.2f km", distancia);

    ssd1306_fill(&ssd, false);  // Limpa a tela
    ssd1306_draw_string(&ssd, "Distancia:", 0, 0);
    ssd1306_draw_string(&ssd, buffer, 0, 16);
    ssd1306_send_data(&ssd);

    sleep_ms(4000);
    voltar_menu_principal();
}



void detectar_som() {
    absolute_time_t start_time = get_absolute_time();
    while (absolute_time_diff_us(start_time, get_absolute_time()) < 15000000) { // 15 segundos
        

        // Lê o nível de som do microfone interno
        adc_select_input(2); // Microfone no ADC 2 (GPIO 28)
        uint16_t resultado = adc_read();
        const float conversao = 3.3f / (1 << 12);
        float tensao = resultado * conversao;
        float nivel_som = tensao * 100.0f;  // Ajuste de escala conforme necessário

        // Ajusta o limite de som com o Joystick X
        limite_som = modificar_valor(limite_som, 0, 100);

        // Média móvel para suavização
        historico_som[indice_som] = nivel_som;
        indice_som = (indice_som + 1) % 5;

        float soma = 0;
        for (int i = 0; i < 5; i++) {
            soma += historico_som[i];
        }
        float media_som = soma / 5;

        char buffer_som[20];
        snprintf(buffer_som, sizeof(buffer_som), "Som: %.2f %%", media_som);

        char buffer_limite[20];
        snprintf(buffer_limite, sizeof(buffer_limite), "Limite: %d %%", limite_som);

        ssd1306_fill(&ssd, false);  // Limpa a tela
        ssd1306_draw_string(&ssd, "Deteccao de Som:", 0, 0);
        ssd1306_draw_string(&ssd, buffer_som, 0, 16);
        ssd1306_draw_string(&ssd, buffer_limite, 0, 32);

        // Verifica se o som ultrapassou o limite (com histerese)
        if (media_som > limite_som + 5) {
            ssd1306_draw_string(&ssd, "ALERTA: SOM ALTO!", 0, 48);
        }

        ssd1306_send_data(&ssd);

        sleep_ms(100); // Atualiza a cada 100 ms
    }
    voltar_menu_principal();   // Volta ao menu principal após 10 segundos
}



// Função para inicializar o Buzzer com PWM na maior intensidade
void inicializar_buzzer() {
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint channel = pwm_gpio_to_channel(BUZZER_PIN);

    pwm_set_wrap(slice_num, 4095); // Define o ciclo máximo para alta frequência
    pwm_set_chan_level(slice_num, channel, 2048); // 50% duty cycle para som inicial
    pwm_set_enabled(slice_num, true);
}


// Função para gerar som com a maior intensidade possível
void tocar_buzzer(int duracao) {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_gpio_level(BUZZER_PIN, 1250); // 100% do ciclo de trabalho para máximo volume
    sleep_ms(duracao);
    pwm_set_gpio_level(BUZZER_PIN, 0);    // Desliga o som
}

// Função para piscar o LED
void piscar_led(int duracao) {
    gpio_put(LED_PIN_R, 1);
    sleep_ms(duracao);
    gpio_put(LED_PIN_R, 0);
}

// Função para ponto (.) com tom alto
void ponto() {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint channel = pwm_gpio_to_channel(BUZZER_PIN);

    // Toca um tom agudo para ponto (Nota A5)
    pwm_set_frequency(slice_num, channel, NOTE_A5);
    piscar_led(DOT_DURATION);
    sleep_ms(PAUSE_DURATION);

    // Para o som
    pwm_set_frequency(slice_num, channel, 0);
}

// Função para traço (-) com tom ainda mais alto
void traco() {
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    uint channel = pwm_gpio_to_channel(BUZZER_PIN);

    // Toca um tom ainda mais agudo para traço (Nota C6)
    pwm_set_frequency(slice_num, channel, NOTE_C6);
    piscar_led(DASH_DURATION);
    sleep_ms(PAUSE_DURATION);

    // Para o som
    pwm_set_frequency(slice_num, channel, 0);
}


// Função principal para o sinal S.O.S em Código Morse
void sinal_sos() {
    // Mensagem no OLED
    exibir_mensagem("Enviando S.O.S", "Aguarde ...");

    // Loop para repetir o S.O.S 3 vezes
    for (int i = 0; i < 3; i++) {
        // Letra S: ...
        ponto();
        sleep_ms(PAUSE_DURATION); // Pausa curta entre pontos
        ponto();
        sleep_ms(PAUSE_DURATION); // Pausa curta entre pontos
        ponto();
        sleep_ms(LETTER_PAUSE);   // Pausa maior entre letras

        // Letra O: ---
        traco();
        sleep_ms(PAUSE_DURATION); // Pausa curta entre traços
        traco();
        sleep_ms(PAUSE_DURATION); // Pausa curta entre traços
        traco();
        sleep_ms(LETTER_PAUSE);   // Pausa maior entre letras

        // Letra S: ...
        ponto();
        sleep_ms(PAUSE_DURATION); // Pausa curta entre pontos
        ponto();
        sleep_ms(PAUSE_DURATION); // Pausa curta entre pontos
        ponto();
        sleep_ms(LETTER_PAUSE * 2); // Pausa maior entre as palavras
    }

    // Finaliza o som e LED
    pwm_set_frequency(pwm_gpio_to_slice_num(BUZZER_PIN), pwm_gpio_to_channel(BUZZER_PIN), 0);
    gpio_put(LED_PIN_R, 0);

    voltar_menu_principal();
}



void mostrar_chama() {
    absolute_time_t start_time = get_absolute_time();
    while (absolute_time_diff_us(start_time, get_absolute_time()) < 5000000) { // 5 segundos
        
        int chama_detectada = detectarChama();
        char buffer[20];
        
        if (chama_detectada) {
            snprintf(buffer, sizeof(buffer), "Chama: SIM");
        } else {
            snprintf(buffer, sizeof(buffer), "Chama: NAO");
        }

        ssd1306_fill(&ssd, false);  // Limpa a tela
        
        // Exibe o estado da chama
        ssd1306_draw_string(&ssd, "Detectando Chama:", 0, 0);
        ssd1306_draw_string(&ssd, buffer, 0, 16);
        
        ssd1306_send_data(&ssd);

        sleep_ms(500); // Atualiza a cada 500 ms para mostrar variação
    }
    voltar_menu_principal();   // Volta ao menu principal após 3 segundos
}


void mostrar_chama_joystick() {
    absolute_time_t start_time = get_absolute_time();
    char buffer[20];
    
    while (absolute_time_diff_us(start_time, get_absolute_time()) < 5000000) { // 5 segundos
        
        // Lê o valor do eixo X do Joystick
        adc_select_input(1); // Joystick X é o ADC 1 (GPIO 26)
        uint16_t adc_value_x = adc_read();
        printf("Joystick X: %d\n", adc_value_x);

        // Verifica a direção do eixo X para indicar chama
        if (adc_value_x > 3000) {
            snprintf(buffer, sizeof(buffer), "Chama: SIM");
        } else if (adc_value_x < 1000) {
            snprintf(buffer, sizeof(buffer), "Chama: NAO");
        } else {
            snprintf(buffer, sizeof(buffer), "Aguardando...");
        }

        ssd1306_fill(&ssd, false);  // Limpa a tela
        
        // Exibe o estado da chama com base na posição do joystick
        ssd1306_draw_string(&ssd, "Detectando Chama:", 0, 0);
        ssd1306_draw_string(&ssd, buffer, 0, 16);
        
        ssd1306_send_data(&ssd);

        sleep_ms(500); // Atualiza a cada 500 ms para mostrar variação
    }
    voltar_menu_principal();   // Volta ao menu principal após 3 segundos
}


void mostrar_monoxido() {
    absolute_time_t start_time = get_absolute_time();
    char buffer_monoxido[20];
    char buffer_risco[20];
    
    while (absolute_time_diff_us(start_time, get_absolute_time()) < 5000000) { // 5 segundos
        
        // Detecta monóxido de carbono
        int monoxido_detectado = detectarMonoxido();
        
        // Lê o valor do eixo X do Joystick
        adc_select_input(1); // Joystick X é o ADC 1 (GPIO 26)
        uint16_t adc_value_x = adc_read();
        printf("Joystick X: %d\n", adc_value_x);

        // Verifica a presença de monóxido
        if (monoxido_detectado) {
            snprintf(buffer_monoxido, sizeof(buffer_monoxido), "Monoxido: SIM");
        } else {
            snprintf(buffer_monoxido, sizeof(buffer_monoxido), "Monoxido: NAO");
        }

        // Verifica a direção do eixo X para indicar risco de vida
        if (adc_value_x > 3000) {
            snprintf(buffer_risco, sizeof(buffer_risco), "Risco Vida: SIM");
        } else if (adc_value_x < 1000) {
            snprintf(buffer_risco, sizeof(buffer_risco), "Risco Vida: NAO");
        } else {
            snprintf(buffer_risco, sizeof(buffer_risco), "Aguardando...");
        }

        ssd1306_fill(&ssd, false);  // Limpa a tela
        
        // Exibe o estado do monóxido e o risco de vida
        ssd1306_draw_string(&ssd, buffer_monoxido, 0, 0);
        ssd1306_draw_string(&ssd, buffer_risco, 0, 16);
        
        ssd1306_send_data(&ssd);

        sleep_ms(500); // Atualiza a cada 500 ms para mostrar variação
    }
    voltar_menu_principal();   // Volta ao menu principal após 3 segundos
}


// Função para mostrar mensagens aleatórias de socorro
void mostrar_mensagens() {
    // Array com mensagens de socorro (máximo 15 caracteres por linha)
    const char *mensagens[] = {
        "Ajuda Necessaria",
        "S.O.S Recebido",
        "Perigo! Alerta",
        "Preciso de Ajuda",
        "Socorro Imediato",
        "Envie Reforcos",
        "Preciso Resgate",
        "Alerta de Perigo",
        "Chamando Socorro",
        "Suporte Necessario"
    };

    // Obtém um índice aleatório
    int indice = rand() % (sizeof(mensagens) / sizeof(mensagens[0]));
    
    // Exibe a mensagem aleatória no OLED
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Mensagem SOS:", 0, 0);
    ssd1306_draw_string(&ssd, mensagens[indice], 0, 20);
    ssd1306_send_data(&ssd);

    sleep_ms(3000); // Exibe a mensagem por 2 segundos

    voltar_menu_principal();
}


void configurar_sistema() {
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Config. Sistema", 10, 20);
    ssd1306_draw_string(&ssd, "Ajustes feitos", 10, 40);
    ssd1306_send_data(&ssd);
    sleep_ms(5000);
}

void mostrar_informacoes() {
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Info. Sistema", 10, 10);
    ssd1306_draw_string(&ssd, "Versao 1.0", 10, 30);
    ssd1306_draw_string(&ssd, "Autor:", 10, 50);
    ssd1306_draw_string(&ssd, "Hugo S. Dias", 60, 50);
    ssd1306_send_data(&ssd);
    sleep_ms(5000); // Aumentado para 3 segundos para melhor leitura
}


// Inicializa o Joystick e Botões
void iniciar_joystick() {
    printf("Inicializando Joystick...\n");
    adc_init();
    //adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);

    gpio_init(BOTAO_A);
    gpio_set_dir(BOTAO_A, GPIO_IN);
    gpio_pull_up(BOTAO_A);
}

// Navega pelo menu usando o joystick e botões
void navegar_menu() {
    static absolute_time_t last_joystick_time = 0;
    static absolute_time_t last_button_time = 0;
    absolute_time_t now = get_absolute_time();

    // Aplica debounce para o joystick
    if (absolute_time_diff_us(last_joystick_time, now) > BUTTON_DEBOUNCE_US) {
        last_joystick_time = now;

        // Leitura do joystick
        adc_select_input(0); // Eixo Y para Navegação
        uint16_t adc_value_y = adc_read();
        printf("Joystick Y: %d\n", adc_value_y);

        adc_select_input(1); // Eixo X para Navegação
        uint16_t adc_value_x = adc_read();
        printf("Joystick X: %d\n", adc_value_x);

        // Processa movimento do joystick (eixo Y)
        if (adc_value_y < 1000) {
            opcao_atual = (opcao_atual + 1) % num_opcoes;
            printf("Navegando para Baixo - Opcao: %d\n", opcao_atual);
            mostrar_menu();
        }
        if (adc_value_y > 3000) {
            opcao_atual = (opcao_atual - 1 + num_opcoes) % num_opcoes;
            printf("Navegando para Cima - Opcao: %d\n", opcao_atual);
            mostrar_menu();
        }

      /*
        // Processa movimento do joystick (eixo X)
        if (adc_value_x < 1000) {  // Direita
            opcao_atual = (opcao_atual + 1) % num_opcoes;
            printf("Navegando para Esquerda - Opcao: %d\n", opcao_atual);
        }
        if (adc_value_x > 3000) {  // Esquerda
            opcao_atual = (opcao_atual - 1 + num_opcoes) % num_opcoes;
            printf("Navegando para Direita - Opcao: %d\n", opcao_atual);
        } */
    }

    // Aplica debounce para o botão do joystick
    if (absolute_time_diff_us(last_button_time, now) > BUTTON_DEBOUNCE_US) {
        last_button_time = now;
        if (!gpio_get(JOYSTICK_PB)) {
            printf("Botao Joystick Pressionado - Opcao: %d\n", opcao_atual);
            opcao_selecionada();
        }
    }

    // Verifica se o botão A foi pressionado para voltar ao menu principal
    if (!gpio_get(BOTAO_A)) {
        printf("Botao A Pressionado - Voltando ao Menu Principal\n");
        voltar_menu_principal();
    }
}


// Função para modificar valores usando o Joystick X
int modificar_valor(int valor_atual, int min, int max) {
    adc_select_input(1); // Joystick X é o ADC 1 (GPIO 26)
    uint16_t adc_value_x = adc_read();
    printf("Joystick X: %d\n", adc_value_x);

    if (adc_value_x < 1000 && valor_atual > min) {
        valor_atual--;
        sleep_ms(200);  // Debounce para evitar múltiplos decrementos
    }
    if (adc_value_x > 3000 && valor_atual < max) {
        valor_atual++;
        sleep_ms(200);  // Debounce para evitar múltiplos incrementos
    }
    
    return valor_atual;
}


// Retorna ao Menu Principal (limpa o histórico se necessário)
void voltar_menu_principal() {
    menu_atual = menu_principal;
    num_opcoes = NUM_OPCOES_PRINCIPAL;
    opcao_atual = 0;
    menu_history_index = 0;  // Opcional: limpa o histórico
    mostrar_menu();
}

// Função de seleção de opção do menu
void opcao_selecionada() {
    printf("Opcao Selecionada: %s\n", menu_atual[opcao_atual].titulo);

    // Se a opção for "Voltar", retorna ao menu anterior
    if (strcmp(menu_atual[opcao_atual].titulo, "Voltar") == 0) {
        pop_menu();
        mostrar_menu();
        return;
    }

    // Se houver ação associada, executa-a
    if (menu_atual[opcao_atual].acao) {
        printf("Executando acao para: %s\n", menu_atual[opcao_atual].titulo);
        menu_atual[opcao_atual].acao();
        return;
    }

    // Se houver submenus, entra no submenu correspondente
    if (menu_atual[opcao_atual].submenus != NULL) {
        push_menu(menu_atual[opcao_atual].submenus, menu_atual[opcao_atual].num_submenus);
        mostrar_menu();
        return;
    }

    // Caso não haja ação ou submenu, exibe mensagem genérica
    exibir_mensagem("Opcao Selecionada:", menu_atual[opcao_atual].titulo);
    mostrar_menu();
}

// Gerencia o histórico – empilha o menu atual e altera para o novo nível
void push_menu(Menu *menu, int count) {
    if (menu_history_index < MAX_MENU_HISTORY) {
        menu_history[menu_history_index] = menu_atual;
        menu_history_count[menu_history_index] = num_opcoes;
        menu_history_index++;
    }
    menu_atual = menu;
    num_opcoes = count;
    opcao_atual = 0;
}

// Gerencia o histórico – retorna ao menu anterior
void pop_menu() {
    if (menu_history_index > 0) {
        menu_history_index--;
        menu_atual = menu_history[menu_history_index];
        num_opcoes = menu_history_count[menu_history_index];
        opcao_atual = 0;
    }
}

// Função para resetar o sistema para o modo BOOTSEL
void gpio_irq_handler(uint gpio, uint32_t events) {
    reset_usb_boot(0, 0);
}


// Função para configurar o PWM no pino especificado pra gerar som s.o.s
void pwm_set_frequency(uint slice_num, uint channel, float frequency) {
    if (frequency == 0) {
        pwm_set_enabled(slice_num, false); // Desativa o PWM
    } else {
        uint32_t clock_freq = 125000000; // Frequência do clock do Pico (125 MHz)
        uint32_t wrap = clock_freq / frequency - 1;

        if (wrap > 65535) wrap = 65535;
        pwm_set_wrap(slice_num, wrap);
        pwm_set_chan_level(slice_num, channel, wrap / 2); // 50% duty cycle
        pwm_set_enabled(slice_num, true);
    }
}


int main() {
    stdio_init_all();
    printf("Inicializando o sistema...\n");

    iniciar_joystick();
    iniciar_oled();
    // animacao_inicial(); // Fase de testes
    inicializar_sensores(); // Inicializa os sensores e os históricos
    inicializar_buzzer();

    // Inicializa o LED
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);

    menu_atual = menu_principal;
    num_opcoes = NUM_OPCOES_PRINCIPAL;
    last_interaction_time = get_absolute_time();

    // Configuração do botão B para modo BOOTSEL
    gpio_init(BOTAO_B);
    gpio_set_dir(BOTAO_B, GPIO_IN);
    gpio_pull_up(BOTAO_B);
    gpio_set_irq_enabled_with_callback(BOTAO_B, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    while (true) {
        // Verifica timeout para voltar ao menu principal
        if (absolute_time_diff_us(last_interaction_time, get_absolute_time()) > TIMEOUT_US) {
            voltar_menu_principal();
            last_interaction_time = get_absolute_time();
        }

        // Atualiza o menu periodicamente
        static absolute_time_t last_update_time = 0;
        if (absolute_time_diff_us(last_update_time, get_absolute_time()) > 200000) {
            last_update_time = get_absolute_time();
            navegar_menu();
            mostrar_menu();
        }
    }
}

