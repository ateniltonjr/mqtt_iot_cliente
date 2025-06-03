# BitHome MQTT: Automação Residencial Inteligente com BitDogLab

## Descrição
Este projeto implementa um sistema IoT de automação residencial utilizando a placa BitDogLab (Raspberry Pi Pico W) como cliente MQTT. O sistema permite o controle e monitoramento remoto de LEDs, matriz de LEDs, servo motor, temperatura interna e display OLED, tudo via protocolo MQTT.

## Funcionalidades
- **Controle de LEDs RGB**: Ligar/desligar LEDs individuais via comandos MQTT.
- **Matriz de LEDs 5x5**: Acionar LEDs específicos da matriz por comandos como `led1` a `led25`.
- **Leitura de Temperatura**: Publicar a temperatura interna do microcontrolador sob demanda.
- **Controle de Servo Motor**: Posicionar o servo em ângulos de 0° a 180° via comando MQTT.
- **Display OLED (I2C)**: Exibe status, comandos recebidos e feedback visual local.
- **Botão físico**: Permite ações locais, como reset ou modo BOOTSEL.
- **Feedback visual e debug**: Mensagens de status e debug via MQTT e serial.

## Recursos Utilizados
- **ADC**: Leitura da temperatura interna.
- **PIO**: Controle da matriz de LEDs WS2812B.
- **PWM**: Controle do servo motor.
- **I2C**: Comunicação com o display OLED.
- **Wi-Fi**: Conexão com o broker MQTT.
- **UART**: Debug e logs via serial.
- **Interrupções**: Tratamento de botões físicos.

## Tópicos MQTT
- `/led` — Controle dos LEDs RGB (ex: `led_red_on`, `led_green_off`)
- `/matriz` — Controle da matriz de LEDs (ex: `led1`, `led25`, `desliga`)
- `/sensores` — Leitura de sensores (ex: `temperatura`, `temperatura off`)
- `/servo` — Controle do servo motor (ex: `90` para posicionar em 90°)
- `/print` — Mensagens para o display OLED
- `/ping`, `/exit` — Utilitários

## Como Usar
1. **Configure o Wi-Fi e MQTT**: Edite as definições de `WIFI_SSID`, `WIFI_PASSWORD`, `MQTT_SERVER`, `MQTT_USERNAME`, `MQTT_PASSWORD` em `mqtt_iot_cliente.c`.
2. **Compile e grave o firmware** na BitDogLab.
3. **Configure o broker MQTT** (ex: Mosquitto no Android via Termux) ou MQTT Broker.
4. **Utilize um app MQTT** (ex: IoT MQTT Panel ou MQTT Explorer) ou um cliente MQTT no PC para enviar comandos e visualizar os dados.
5. **Envie comandos MQTT** para os tópicos conforme as funcionalidades desejadas.

## Exemplo de Comandos
- Ligar LED vermelho: envie `led_red_on` para `/led`
- Desligar LED azul: envie `led_blue_off` para `/led`
- Acender LED 7 da matriz: envie `led7` para `/matriz`
- Desligar matriz: envie `desliga` para `/matriz`
- Ler temperatura: envie `temperatura` para `/sensores`
- Parar leitura de temperatura: envie `temperatura off` para `/sensores`
- Mover servo para 45°: envie `45` para `/servo`

## Estrutura do Projeto
- `mqtt_iot_cliente.c` — Arquivo principal
- `lib/` — Bibliotecas para cada periférico e funcionalidade
- `inc/` — Headers e drivers de display
- `CMakeLists.txt` — Configuração de build

## Requisitos
- Placa BitDogLab (Raspberry Pi Pico W)
- Broker MQTT (Mosquitto)
- App MQTT (IoT MQTT Panel) ou cliente MQTT no PC
- Termux (opcional, para rodar Mosquitto no Android)

## Créditos
Desenvolvido por Atenilton Santos de souza Júnior — Polo Bom Jesus da Lapa

## Demonstração
- [Link para o vídeo de demonstração](https://youtu.be/seuvideo) **Falta o vídeo**

---
