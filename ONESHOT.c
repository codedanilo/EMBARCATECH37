#include "pico/stdlib.h"
#include "pico/time.h"
#include "stdio.h"

// Definindo os pinos dos LEDs
#define LED_BLUE_PIN   11  // LED azul no GPIO 11
#define LED_RED_PIN    12  // LED vermelho no GPIO 12
#define LED_GREEN_PIN  13  // LED verde no GPIO 13
#define BUTTON_PIN     5   // Botão no GPIO 5

// Definindo estados dos LEDs
volatile bool leds_on = false;  // Indica se os LEDs estão ligados
volatile int led_state = 0;     // Controle do estado dos LEDs: 0 - todos acesos, 1 - dois acesos, 2 - um aceso

// Variáveis de debounce
volatile bool button_pressed = false;
uint32_t last_button_press_time = 0;
#define DEBOUNCE_TIME 200 // Tempo de debounce (200 ms)

// Função de callback para desligar os LEDs
void turn_off_leds() {
    if (led_state == 0) {
        gpio_put(LED_BLUE_PIN, 0);   // Desliga LED azul
        gpio_put(LED_RED_PIN, 0);    // Desliga LED vermelho
        gpio_put(LED_GREEN_PIN, 1);  // Deixa apenas LED verde aceso
        led_state = 1;  // Próximo estado será dois LEDs acesos
    } else if (led_state == 1) {
        gpio_put(LED_GREEN_PIN, 0);  // Desliga LED verde
        gpio_put(LED_RED_PIN, 1);    // Liga LED vermelho
        led_state = 2;  // Próximo estado será um LED aceso
    } else if (led_state == 2) {
        gpio_put(LED_RED_PIN, 0);    // Desliga LED vermelho
        led_state = 0;  // Reinicia a sequência
    }
}

// Função de debounce para o botão
void debounce_button() {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    
    // Verifica se o tempo de debounce já passou e se o botão foi pressionado
    if (current_time - last_button_press_time > DEBOUNCE_TIME) {
        if (gpio_get(BUTTON_PIN) == 0) { // Botão pressionado (assumindo que o botão puxa para GND)
            last_button_press_time = current_time;
            if (!button_pressed) {
                button_pressed = true;
                if (!leds_on) {
                    leds_on = true; // Marca que os LEDs estão ligados
                    gpio_put(LED_BLUE_PIN, 1);   // Liga LED azul
                    gpio_put(LED_RED_PIN, 1);    // Liga LED vermelho
                    gpio_put(LED_GREEN_PIN, 1);  // Liga LED verde

                    // Adiciona o alarme para a sequência de desligamento dos LEDs
                    add_alarm_in_ms(3000, turn_off_leds, NULL, false); // Atraso de 3 segundos
                }
            }
        }
    }
}

// Função principal
int main() {
    // Inicializando UART para comunicação serial
    stdio_init_all();

    // Inicializando pinos dos LEDs
    gpio_init(LED_BLUE_PIN);
    gpio_set_dir(LED_BLUE_PIN, GPIO_OUT);
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

    // Inicializando pino do botão
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);  // Habilita pull-up para o botão

    // Loop principal
    while (true) {
        debounce_button();  // Verifica o estado do botão

        // Se todos os LEDs estiverem apagados, o botão pode ser pressionado novamente
        if (led_state == 0) {
            button_pressed = false;
            leds_on = false; // Reseta a condição de LEDs ligados
        }

        sleep_ms(10); // Atraso para não sobrecarregar o processador
    }

    return 0;
}
