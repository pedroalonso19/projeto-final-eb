# Projeto Final - Controle de LEDs e Display com Raspberry Pi Pico - Simulação Instrumento Musical

Este projeto utiliza um Raspberry Pi Pico para controlar LEDs RGB, um display SSD1306 e buzzers. O objetivo é acender LEDs de acordo com dígitos inseridos via USB, tocar notas musicais correspondentes e exibir informações no display.

## Funcionalidades

- **Controle de LEDs RGB**: Acende o LED azul para números pares e o LED vermelho para números ímpares.
- **Display SSD1306**: Exibe o caractere digitado e uma mensagem no display OLED.
- **Buzzers**: Toca uma nota musical correspondente ao dígito inserido.
- **Leitura de Entrada USB**: Recebe caracteres via USB e processa a entrada.

## Componentes Utilizados

- **Raspberry Pi Pico**
- **Display SSD1306**: Conectado via I2C.
- **LEDs RGB**: Conectados aos pinos GPIO.
- **Buzzers**: Conectados aos pinos GPIO.
- **Botões**: Conectados aos pinos GPIO para futuras expansões.

## Configuração de Hardware

### Pinos Utilizados

- **I2C para Display SSD1306**
  - SDA: GPIO 14
  - SCL: GPIO 15
- **LEDs RGB**
  - LED Vermelho: GPIO 13
  - LED Verde: GPIO 11
  - LED Azul: GPIO 12
- **Buzzers**
  - Buzzer A: GPIO 21
  - Buzzer B: GPIO 10
- **Botões**
  - Botão A: GPIO 5
  - Botão B: GPIO 6


## Desenvolvedor: 
### Pedro Alonso Ribeiro Ferreira da Silva



