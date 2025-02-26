cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================

# Especifica a placa a ser utilizada
set(PICO_BOARD pico CACHE STRING "Board type")

# Importa o Raspberry Pi Pico SDK
include(pico_sdk_import.cmake)

# Define o projeto
project(DoomsdayBitDog C CXX ASM)

# Inicializa o Raspberry Pi Pico SDK
pico_sdk_init()

# Define o executável principal
add_executable(DoomsdayBitDog 
    DoomsdayBitDog.c 
    src/BitDog-SensorMatrix.c 
    src/ssd1306.c 
    src/sensores.c 
    led_matrix.c
)

# Inclui diretórios de cabeçalhos
target_include_directories(DoomsdayBitDog PRIVATE 
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/inc
)

# Define o nome do programa e a versão
pico_set_program_name(DoomsdayBitDog "DoomsdayBitDog")
pico_set_program_version(DoomsdayBitDog "0.1")

# Configurações de saída UART/USB
pico_enable_stdio_uart(DoomsdayBitDog 0)
pico_enable_stdio_usb(DoomsdayBitDog 1)

# Gera o cabeçalho PIO para controle de LEDs WS2812b
pico_generate_pio_header(DoomsdayBitDog ${CMAKE_CURRENT_LIST_DIR}/ws2812b.pio)

# Linka as bibliotecas necessárias
target_link_libraries(DoomsdayBitDog
    pico_stdlib
    hardware_i2c
    hardware_adc
    hardware_uart
    hardware_pwm
    hardware_pio
    hardware_rtc  
)

# Gera os arquivos binários para upload na placa
pico_add_extra_outputs(DoomsdayBitDog)
