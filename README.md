# **DoomsdayBitDog: Kit de Sobrevivência em Cenários de Catástrofes** utilizando o **RP2040 (Raspberry Pi Pico)** e a **BitDogLab** .

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

# **1. Estrutura do Projeto - DoomsdayBitDog:**

## 1.0 - Daemon Simulator (simulador de variaveis)

## 1.1 **Módulo de Comunicação e Alerta** :

* Simulação de comunicação de emergência via  **UART (GPIO 0 e 1)** .
* Alerta sonoro com  **Buzzer (GPIO 21)**.
* Sinalização visual com **LEDs RGB (GPIO 13,11,12)** e **Matrix de LED 5x5 WS2812B (GPIO 7)**.
* Transmissão de Radio Frequencia - Utilizando o PIO e PLL
* **Microfone (GPIO 28)**

## 1.2 **Módulo de Sensoriamento Ambiental (Simulação)** :

* Utilização do **Joystick** para simular sensores:
  * **X (GPIO 26)** para Gases Tóxicos.
  * **Y (GPIO 27)** para Radiação UV.
  * **Botão Push (GPIO 22)** para Temperatura e Umidade.

## 1.3 **Módulo de Navegação e Localização (Simulação)** :

* Utilização do **Joystick** para simular:
  * **X (GPIO 26)** para Coordenadas GPS.
  * **Y (GPIO 27)** para Orientação Bússola.
  * **Botão Push (GPIO 22)** para Acelerômetro e Giroscópio.
  * **Botão A (GPIO 5)**
  * **Botão A (GPIO 6)**

## 1.4 **Módulo de Controle e Interface** :

* **Joystick** para navegação no menu.
* **OLED SSD1306** (I2C em GPIO 15 SCL e 14 SDA) para exibir informações.

# **2. Configuração do Projeto:**

* Utilize o **Visual Studio Code** com a  **extensão Raspberry Pico SDK** .
* Estrutura de diretórios inicial:

```
├── CMakeLists.txt 
  ├── DoomsdayBitDog.c //Programa principal
  └── lib/
  └── ssd1306/
  ├── ssd1306.c
  └── ssd1306.h
```

# **3. Módulo de Comunicação e Alerta (UART, LEDs e Buzzer):**

## 3.1. **Configuração UART para Comunicação de Emergência:**

Utilize a UART para simulação de mensagens de alerta:

## 3.2. **Controle de LEDs RGB e Matrix de LED WS2812B:**

Utilize a biblioteca **Pio-SDK** para controlar LEDs WS2812B.
Exemplo para  **Matrix de LED 5x5** :

## 3.3. **Buzzer para Alerta Sonoro:**

Controle o Buzzer com PWM para emitir alertas sonoros:

# **4. Módulo de Sensoriamento Ambiental (Simulação com Joystick):**

### **Configuração de ADC para Leitura Analógica:**

# **5. Módulo de Navegação e Localização (Simulação com Joystick):**

* Utilize o **Joystick X e Y** para simular coordenadas e orientação.
* **Botão Push** para simular acelerômetro e giroscópio.
* Dados exibidos no  **OLED SSD1306** .

# **6. Módulo de Controle e Interface (OLED SSD1306 e Joystick):**

### **Configuração I2C para Display OLED SSD1306:**

# **7. Menu de Interface com Joystick:**

* Navegação utilizando o  **Joystick X e Y** .
* Seleção com o  **Botão Push** .
* Itens do Menu (Ideia Inicial):
  1. Monitoramento Ambiental
  2. Navegação e Localização
  3. Alertas de Emergência
  4. Configurações do Sistema

# **8. Compilação e Execução:**

* Compile o projeto usando o  **Pico-SDK** :
* Envie o arquivo `.uf2` gerado para o  **Raspberry Pi Pico** .

# **9. Documentação e Próximos Passos:**

* Documentar cada módulo separadamente.
* Inclua  **Diagramas de Blocos** , **Fluxogramas** e  **Explicações de Código** .
* Teste cada módulo individualmente antes da integração completa.

# **10. Validação e Testes:**

* Teste em ambientes simulados de catástrofes - simulador.
* Verifique a resposta dos alertas sonoros e visuais.
* Teste a navegação do menu com o joystick.
