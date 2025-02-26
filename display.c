#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/adc.h"
#include "pico/bootrom.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

// Definições de constantes
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define DISPLAY_ADDRESS 0x3C
#define NUM_PIXELS 25
#define OUT_PIN 7
#define DEBOUNCE_TIME_US 200000
#define CLOCK_SPEED_KHZ 100000

// Definição dos LEDs RGB
typedef enum {
    RLED_PIN = 13,
    GLED_PIN = 11,
    BLED_PIN = 12
} LedPins;

// Definição dos botões
typedef enum {
    BTNA_PIN = 5,
    BTNB_PIN = 6
} ButtonPins;

// Definição dos buzzers
#define BUZZER_A_PIN 21
#define BUZZER_B_PIN 10

// Frequências das notas (em Hz) para cada dígito (0 a 9)
const uint32_t digit_frequencies[10] = {
    262,  // 0: Dó (C4)
    294,  // 1: Ré (D4)
    330,  // 2: Mi (E4)
    349,  // 3: Fá (F4)
    392,  // 4: Sol (G4)
    440,  // 5: Lá (A4)
    494,  // 6: Si (B4)
    523,  // 7: Dó (C5)
    587,  // 8: Ré (D5)
    659   // 9: Mi (E5)
};

// Enumeração para dígitos de 0 a 9
typedef enum {
    DIGIT_0 = 0,
    DIGIT_1 = 1,
    DIGIT_2 = 2,
    DIGIT_3 = 3,
    DIGIT_4 = 4,
    DIGIT_5 = 5,
    DIGIT_6 = 6,
    DIGIT_7 = 7,
    DIGIT_8 = 8,
    DIGIT_9 = 9
} Digit;

// Variável de mudança de string para inteiro
static volatile int ic = 0;

// Variável ligada ao debounce dos botões
static volatile uint32_t last_time = 0;

// Inicializa a estrutura do display
ssd1306_t ssd;

// Matriz com todos os dígitos
const double digits[10][25] = {
    { // Digito 0
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0
    },
    { // Digito 1
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 1.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0
    },
    { // Digito 2
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0
    },
    { // Digito 3
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0
    },
    { // Digito 4
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0
    },
    { // Digito 5
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0
    },
    { // Digito 6
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0
    },
    { // Digito 7
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0, 0.0
    },
    { // Digito 8
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0
    },
    { // Digito 9
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 1.0, 0.0,
        0.0, 1.0, 1.0, 1.0, 0.0,
        0.0, 1.0, 0.0, 0.0, 0.0,
        0.0, 0.0, 0.0, 1.0, 0.0
    }
};

// Inicialização dos LEDs, botões e buzzers
void init_all() {
    gpio_init(RLED_PIN);
    gpio_set_dir(RLED_PIN, GPIO_OUT);
    gpio_put(RLED_PIN, 0);

    gpio_init(GLED_PIN);
    gpio_set_dir(GLED_PIN, GPIO_OUT);
    gpio_put(GLED_PIN, 0);

    gpio_init(BLED_PIN);
    gpio_set_dir(BLED_PIN, GPIO_OUT);
    gpio_put(BLED_PIN, 0);

    gpio_init(BTNA_PIN);
    gpio_set_dir(BTNA_PIN, GPIO_IN);
    gpio_pull_up(BTNA_PIN);

    gpio_init(BTNB_PIN);
    gpio_set_dir(BTNB_PIN, GPIO_IN);
    gpio_pull_up(BTNB_PIN);

    gpio_init(BUZZER_A_PIN);
    gpio_set_dir(BUZZER_A_PIN, GPIO_OUT);
    gpio_put(BUZZER_A_PIN, 0);

    gpio_init(BUZZER_B_PIN);
    gpio_set_dir(BUZZER_B_PIN, GPIO_OUT);
    gpio_put(BUZZER_B_PIN, 0);
}

// Função para tocar uma nota no buzzer
void play_note(uint8_t buzzer_pin, uint32_t frequency, uint32_t duration_ms) {
    if (frequency == 0) {
        gpio_put(buzzer_pin, 0); // Desliga o buzzer
        sleep_ms(duration_ms);
        return;
    }

    uint32_t period_us = 1000000 / frequency;
    uint32_t half_period_us = period_us / 2;

    uint32_t start_time = to_ms_since_boot(get_absolute_time());
    while (to_ms_since_boot(get_absolute_time()) - start_time < duration_ms) {
        gpio_put(buzzer_pin, 1);
        sleep_us(half_period_us);
        gpio_put(buzzer_pin, 0);
        sleep_us(half_period_us);
    }
}

// Função principal
int main() {
    stdio_init_all();
    init_all();

    // Configuração do display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&ssd, 128, 64, false, DISPLAY_ADDRESS, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    while (true) {
        if (stdio_usb_connected()) {
            char c;
            if (scanf("%c", &c) == 1) {
                printf("Recebido: '%c'\n", c);

                ssd1306_fill(&ssd, false);
                ssd1306_draw_string(&ssd, "DIGITADO:", 2, 38);
                ssd1306_draw_char(&ssd, c, 80, 38);
                ssd1306_send_data(&ssd);

                if (c >= '0' && c <= '9') {
                    ic = c - '0'; // Conversão de string para inteiro
                    play_note(BUZZER_A_PIN, digit_frequencies[ic], 200); // Toca a nota correspondente

                    // Acende o LED correspondente
                    if (ic % 2 == 0) {
                        gpio_put(BLED_PIN, 1); // Acende o LED azul
                        gpio_put(RLED_PIN, 0); // Apaga o LED vermelho
                    } else {
                        gpio_put(BLED_PIN, 0); // Apaga o LED azul
                        gpio_put(RLED_PIN, 1); // Acende o LED vermelho
                    }
                }
            }
        }
        sleep_ms(100);
    }
    return 0;
}