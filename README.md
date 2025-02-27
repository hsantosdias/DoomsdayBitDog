# **DoomsdayBitDog: Kit de Sobrevivência em Cenários de Catástrofes** utilizando o **RP2040 (Raspberry Pi Pico)** e a **BitDogLab**.

### Informações iniciais sobre o projeto e diretizes que serão seguidas - criterios e requisitos expressos pelo professor Wilton:

##### **1. Conformidade com o Escopo do Curso**

O projeto deve se restringir aos conceitos, características e tecnologias de sistemas embarcados que foram abordados durante o curso, utilizando exclusivamente a placa  **BitDogLab** . Não é permitido o uso de componentes físicos adicionais como sensores externos, mas **simular** sensores é permitido desde que a simulação seja feita usando os recursos da BitDogLab, como pushbuttons, potenciômetros do joystick, LEDs, PWM, etc.

##### **2. Simulação com Periféricos Disponíveis**

Para simular esses sensores, você deve utilizar os periféricos já disponíveis na  **BitDogLab**:

* **Pushbuttons:** Podem ser usados para simular sensores binários (0 e 1), como presença (PIR) ou sensores magnéticos.
* **Joystick (potenciômetros):** Podem ser usados para simular sensores analógicos, como temperatura, umidade, pressão e até GPS (variando valores como   coordenadas).
* **LEDs e PWM:** Podem simular atuadores ou indicar estados de sensores.
* **Display e Matriz de LED:** Podem representar graficamente os valores simulados, como coordenadas GPS ou variações de temperatura e pressão.

##### **3. Uso de Software para Simulação**

Projeto para criar um **módulo de software em C** que gere dados simulados para os sensores. Por exemplo:

* **GPS:** Criar coordenadas fictícias que mudam gradualmente para simular movimento - Joystick.
* **Temperatura e Umidade:** Use fórmulas matemáticas para gerar valores que variam dentro de um intervalo lógico - joystick.
* **Pressão:** Pode ser simulada com base em uma função especifica ou outro comportamento físico esperado - joystick.

##### **4. Documentação e Justificativa**

É crucial justificar na **documentação do projeto** o motivo pelo qual você está simulando os sensores e como fez isso utilizando os recursos disponíveis na BitDogLab. Você deve deixar claro que a simulação foi feita por meio de *software** e  **periféricos existentes**, sem adicionar componentes físicos externos.

##### **5. Validação e Testes**

Você também precisa detalhar no relatório como validou os dados simulados, explicando:

* Como os dados foram gerados (algoritmo utilizado).
* Como esses dados foram processados no sistema embarcado.
* Como os resultados foram exibidos (display, LEDs, etc.).

#### **Conclusão**

Usar um software para simular sensores:

* A simulação seja feita usando **exclusivamente a BitDogLab** e seus periféricos.
* A simulação seja **implementada em software** sem o uso de hardware adicional.
* **Tudo deverá ser bem documentado**, incluindo as justificativas e métodos de validação.

---

# **Estrutura do Projeto - DoomsdayBitDog: Kit de Sobrevivência em Cenários de Catástrofes**

**Utilizando o RP2040 (Raspberry Pi Pico) e a BitDogLab**

---

## **1. Motivação e Desafios do Projeto**

A ideia de criar o DoomsdayBitDog surgiu da necessidade de desenvolver um sistema de sobrevivência em cenários de catástrofes, onde é crucial  ter acesso a informações ambientais críticas e meios de comunicação de emergência.

Durante o desenvolvimento, enfrentei desafios técnicos significativos, como a implementação de comunicação UART, controle de LEDs RGB e Matrix WS2812B usando PIO e PLL (Transmissor de FM - apenas testes iniciais), além de simular sensores ambientais complexos utilizando apenas o hardware disponível na BitDogLab. A limitação de não poder usar componentes externos me forçou a ser criativo, utilizando o joystick para simular dados ambientais como gases tóxicos, radiação UV e coordenadas GPS.

Este projeto não foi apenas um exercício de programação, mas um teste de resiliência e criatividade ao trabalhar com limitações de hardware. Cada obstáculo técnico superado fortaleceu a solução, tornando-a mais robusta e eficiente para cenários críticos.

---

## **2. Estrutura do Projeto**

### **2.1. Módulo - IHM (Interface Homem-Máquina)**

#### **2.1.1. Descrição do Projeto**

O BitDogLab-Menu é um sistema de navegação de menus hierárquicos desenvolvido para o RP2040 utilizando um joystick e um display OLED SSD1306. Permite navegar por submenus e executar ações associadas a cada opção do menu. Foi desenvolvido em C utilizando o Pico SDK, com suporte para navegação fluida e hierárquica.

#### **2.1.2. Funcionalidades Implementadas**

* **Navegação Hierárquica:** Navegação fluida entre submenus e retorno ao menu principal.
* **Execução de Ações:** Cada opção do menu executa uma função específica.
* **Histórico de Navegação:** Mantém o caminho percorrido, possibilitando retorno ao nível anterior.
* **Timeout do Menu:** Após 30 segundos de inatividade, retorna automaticamente ao menu principal.
* **Controle via Joystick:** Navegação e seleção com joystick analógico.
* **Display OLED SSD1306:** Exibição do menu utilizando a biblioteca SSD1306.

### **2.2. Daemon Simulator (Simulador de Variáveis)**

#### **2.2.1. Módulo de Comunicação e Alerta**

* Simulação de comunicação de emergência via  **UART (GPIO 0 e 1)** .
* Alerta sonoro com  **Buzzer (GPIO 21)** .
* Sinalização visual com **LEDs RGB (GPIO 13, 11, 12)** e  **Matrix de LED 5x5 WS2812B (GPIO 7)**  para testes e sem implementação.
* **Transmissão de Rádio Frequência** utilizando PIO e PLL para testes com base no https://github.com/hsantosdias/BitDogLab-FM .
* **Microfone (GPIO 28)** para detecção de som ambiente.

#### **2.2.2. Módulo de Sensoriamento Ambiental (Simulação)**

* Utilização do **Joystick** para simular sensores ambientais:
  * **X (GPIO 26):** Gases Tóxicos.
  * **Y (GPIO 27):** Radiação UV.
  * **Botão Push (GPIO 22):** Temperatura, Umidade. sensibilidade microfone.

#### **2.2.3. Módulo de Navegação e Localização (Simulação)**

* Utilização do **Joystick** para simular:
  * **X (GPIO 26):** Coordenadas GPS.
  * **Y (GPIO 27):** Orientação Bússola.
  * **Botão Push (GPIO 22):** Acelerômetro e Giroscópio.
  * **Botão A (GPIO 5)** para navegação rápida e **Botão B (GPIO 6)** bootsel.

#### **2.2.4. Módulo de Controle e Interface**

* **Joystick** para navegação no menu.
* **OLED SSD1306 (I2C em GPIO 15 SCL e 14 SDA)** para exibir informações de forma clara e objetiva.

---

## **3. Configuração do Projeto**

* Utilize o **Visual Studio Code** com a  **extensão Raspberry Pico SDK** .
* Estrutura de diretórios inicial:
```textplan
├── CMakeLists.txt
├── DoomsdayBitDog.c  // Programa principal
├── src/
│   ├── BitDog-SensorMatrix.c 
│   ├── ssd1306.c
│   └── sensores.c
└── inc/
    ├── BitDog-SensorMatrix.h
    ├── ssd1306.h
    └── sensores.h
```
Estrutura de Arquivos e Diretórios do Projeto **DoomsdayBitDog**

---

## **. Diretório Principal: DoomsdayBitDog**

**Arquivos no Diretório Principal:**

* **.gitignore** – Configurações para ignorar arquivos no controle de versão.
* **CMakeLists.txt** – Script de configuração para o CMake, definindo o build do projeto.
* **diagram.json** – Arquivo JSON contendo o diagrama do projeto (provavelmente usado para documentação ou simulação no Wokwi).
* **DoomsdayBitDog.c** – Arquivo principal do projeto contendo o código-fonte principal.
* **pico_sdk_import.cmake** – Configuração para importar o Raspberry Pi Pico SDK.
* **README.md** – Documentação detalhada do projeto.
* **wokwi-project.txt** – Configuração específica para o Wokwi (simulador online).
* **wokwi.toml** – Arquivo de configuração para o Wokwi.

### **Subdiretórios:**

* **build/** – Diretório gerado pelo CMake contendo os arquivos de compilação.
* **inc/** – Diretório para arquivos de cabeçalho (`.h`) utilizados no projeto.
* **src/** – Diretório com os códigos-fonte (`.c`) do projeto.

---

## **. Diretório src/**

### **Arquivos no Diretório src:**

* **BitDog-SensorMatrix.c** – Implementação da matriz de sensores do projeto. Responsável por simular e gerenciar as variáveis dos sensores.
* **led_matrix.c** – Código para teste/controle e gerenciamento da Matrix de LED 5x5 WS2812B (testes de implementação)).
* **sensores.c** – Simulação de sensores ambientais, incluindo gases tóxicos, radiação UV, temperatura e umidade.
* **ssd1306.c** – Controle do display OLED SSD1306, utilizando a comunicação I2C.

---

## **. Diretório inc/**

Este diretório contém os arquivos de cabeçalho (`.h`) associados aos códigos-fonte presentes em `src/`. Eles definem as funções, estruturas e constantes usadas no projeto.

---

## **. Observações Gerais:**

* A estrutura está bem organizada, separando o código-fonte (`src/`) dos cabeçalhos (`inc/`) e dos arquivos de compilação (`build/`).
* A documentação (`README.md`) e os arquivos de configuração (`.gitignore`, `CMakeLists.txt`, `wokwi.toml`) evidenciam um projeto bem estruturado e preparado para versionamento e simulação.

---

Esse detalhamento oferece uma visão completa da organização e dos arquivos do projeto  **DoomsdayBitDog** , facilitando a compreensão e manutenção do código.

---

## **4. Compilação e Execução**

* Compile o projeto usando o  **Pico-SDK** .
* Envie o arquivo `.uf2` gerado para o  **Raspberry Pi Pico** .

---

## **5. Documentação e Próximos Passos**

* Documentar cada módulo separadamente, detalhando as funcionalidades e código.
* Incluir  **Diagramas de Blocos** , **Fluxogramas** e  **Explicações de Código** .
* Testar cada módulo individualmente antes da integração completa.
* Expandir o projeto para incluir mais funcionalidades críticas para cenários de emergência.

---

## **6. Validação e Testes**

* Testes em ambientes simulados de catástrofes utilizando o  **simulador interno** .
* Verificação das respostas dos alertas sonoros e visuais em condições críticas.
* Teste da navegação do menu com o joystick, avaliando usabilidade e fluidez.

---

## **7. Dificuldades e Lições Aprendidas**

* **Desafio na Simulação de Sensores:** A restrição de não utilizar sensores físicos exigiu soluções criativas. Utilizei o joystick para simular variações ambientais, o que foi um desafio em termos de precisão.
* **Otimização de Memória:** O gerenciamento de memória foi essencial para manter o desempenho do sistema, especialmente ao lidar com múltiplas variáveis de sensores simulados.
* **Integração e Testes:** A integração dos módulos apresentou problemas de sincronização e timing, que foram solucionados com ajustes nos temporizadores e interrupções.

---

## **8. Conclusão**

O projeto **DoomsdayBitDog** é um sistema robusto e eficiente para monitoramento ambiental e comunicação de emergência, projetado para operar em cenários de catástrofes. Foi desenvolvido utilizando apenas os recursos da  **BitDogLab** , sem componentes externos, o que representou um desafio considerável.

A superação das dificuldades técnicas e a inovação nas simulações garantiram um sistema confiável e adaptável para situações críticas. A jornada de desenvolvimento deste projeto não apenas aprimorou minhas habilidades técnicas, mas também desafiou minha criatividade e resiliência.

---

## **9. Créditos e Agradecimentos**

Projeto desenvolvido por **Hugo Santos Dias** no âmbito do programa EmbarcaTech. Agradecimentos ao Professor Wilton, Professor Ricardo e a mentora Aline pelo suporte e diretrizes essenciais para o desenvolvimento do DoomsdayBitDog.

---

Este README detalha a jornada de desenvolvimento do  **DoomsdayBitDog** , incluindo as dificuldades enfrentadas, as soluções implementadas e o impacto do projeto no contexto de sistemas embarcados para cenários de emergência.
