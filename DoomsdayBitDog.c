#include <stdio.h> // Inclui biblioteca padrão de E/S em C (printf, scanf, etc) 
#include <stdlib.h> // Inclui biblioteca padrão de funções em C (malloc, free, etc)
#include <string.h> // Inclui biblioteca de funções de strings em C (strlen, strcmp, etc)
#include "pico/stdlib.h" // Inclui biblioteca de funções do Pico (gpio_init, sleep_ms, etc)
#include "hardware/adc.h" // Inclui biblioteca de funções de ADC (adc_init, adc_gpio_init, etc)
#include "hardware/i2c.h" // Inclui biblioteca de funções de I2C (i2c_init, i2c_write_blocking, etc)
#include "hardware/irq.h" // Inclui biblioteca de funções de interrupção (IRQ)
#include "hardware/uart.h" // Inclui biblioteca de funções de UART (uart_init, uart_getc, etc)

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

// Definições para LEDs e botões
#define LED_PIN_R 13    // LED Vermelho 
#define LED_PIN_B 12    // LED Azul
#define LED_PIN_G 11    // LED Verde

#define MENU_TIMEOUT_US 30000000  // 30 segundos
#define BUTTON_DEBOUNCE_US 50000  // 50 ms

// Número de opções no Menu Principal
#define NUM_OPCOES_PRINCIPAL 4

// Estrutura do OLED
ssd1306_t ssd;


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

// Prototipação de funções de histórico do menu
typedef struct Menu Menu;
void push_menu(Menu *menu, int count);
void pop_menu();

// Prototipação das funções de ação
void mostrar_temperatura(void);
void mostrar_umidade(void);
void mostrar_posicao(void);
void mostrar_mensagens(void);
void configurar_sistema(void);
void mostrar_informacoes(void);

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
    {"Voltar",      NULL, 0, voltar_menu_principal}
};

// Submenu para GeoLocalizacao
Menu submenu_navegacao[] = {
    {"Posição",     NULL, 0, mostrar_posicao},
    {"Voltar",      NULL, 0, voltar_menu_principal}
};

// Submenu para Alertas e Mensagens
Menu submenu_alertas[] = {
    {"Mensagens",   NULL, 0, mostrar_mensagens},
    {"Voltar",      NULL, 0, voltar_menu_principal}
};

// Submenu para Configurações do Sistema
Menu submenu_configuracoes[] = {
    {"Ajustes",     NULL, 0, configurar_sistema},
    {"Informações", NULL, 0, mostrar_informacoes},
    {"Voltar",      NULL, 0, voltar_menu_principal}
};

// Menu Principal – o campo num_submenus indica quantos itens o submenu terá
Menu menu_principal[] = {
    {"Info Ambiental", submenu_monitoramento, ARRAY_SIZE(submenu_monitoramento), NULL},
    {"GeoLocalizacao", submenu_navegacao,     ARRAY_SIZE(submenu_navegacao),     NULL},
    {"Alert Mensagems",submenu_alertas,       ARRAY_SIZE(submenu_alertas),       NULL},
    {"Config Sistema", submenu_configuracoes, ARRAY_SIZE(submenu_configuracoes), NULL}
};

// Variáveis globais para navegação
int opcao_atual = 0;
Menu *menu_atual = menu_principal;
int num_opcoes = NUM_OPCOES_PRINCIPAL;
static absolute_time_t last_interaction_time = 0;
const uint32_t TIMEOUT_US = 30000000; // 30 segundos

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
    sleep_ms(2000);
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
    DadosSensores dados = obter_dados_sensores();
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "Temp: %.2f C", dados.temperatura.media);

    ssd1306_fill(&ssd, false);  // Limpa a tela
    ssd1306_draw_string(&ssd, "Temperatura:", 0, 0);
    ssd1306_draw_string(&ssd, buffer, 0, 16);
    ssd1306_send_data(&ssd);
    sleep_ms(2000);
    voltar_menu_principal();   // Volta ao menu principal após exibir a mensagem
}



void mostrar_umidade() {
    DadosSensores dados = obter_dados_sensores();
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "Umid: %.2f%%", dados.umidade.media);

    ssd1306_fill(&ssd, false);  // Limpa a tela
    ssd1306_draw_string(&ssd, "Umidade:", 0, 0);
    ssd1306_draw_string(&ssd, buffer, 0, 16);
    ssd1306_send_data(&ssd);
    sleep_ms(2000);
    voltar_menu_principal();
}

void mostrar_posicao() {
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Lat: -23.5", 10, 20);
    ssd1306_draw_string(&ssd, "Long: -46.6", 10, 40);
    ssd1306_send_data(&ssd);
    sleep_ms(2000);
}

void mostrar_mensagens() {
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Sem mensagens", 10, 20);
    ssd1306_send_data(&ssd);
    sleep_ms(2000);
}

void configurar_sistema() {
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Config. Sistema", 10, 20);
    ssd1306_draw_string(&ssd, "Ajustes feitos", 10, 40);
    ssd1306_send_data(&ssd);
    sleep_ms(2000);
}

void mostrar_informacoes() {
    ssd1306_fill(&ssd, false);
    ssd1306_draw_string(&ssd, "Info. Sistema", 10, 20);
    ssd1306_draw_string(&ssd, "Versao 1.0", 10, 40);
    ssd1306_send_data(&ssd);
    sleep_ms(2000);
}

// Inicializa o Joystick e Botões
void iniciar_joystick() {
    printf("Inicializando Joystick...\n");
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
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

        // Processa movimento do joystick (eixo X)
        if (adc_value_x < 1000) {  // Direita
            opcao_atual = (opcao_atual + 1) % num_opcoes;
            printf("Navegando para Esquerda - Opcao: %d\n", opcao_atual);
        }
        if (adc_value_x > 3000) {  // Esquerda
            opcao_atual = (opcao_atual - 1 + num_opcoes) % num_opcoes;
            printf("Navegando para Direita - Opcao: %d\n", opcao_atual);
        }
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

int main() {
    stdio_init_all();
    printf("Inicializando o sistema...\n");

    iniciar_joystick();
    iniciar_oled();
    // animacao_inicial(); // Fase de testes
    inicializar_sensores(); // Inicializa os sensores e os históricos
    
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

