#include "pico/stdlib.h"
#include "pico/time.h"
#include "stdio.h"
#include "pico/stdlib.h"
//#include "hardware/pio.h"

// Definindo os pinos dos LEDs
#define LED_RED_PIN   11  // LED vermelho no GPIO 11
#define LED_YELLOW_PIN 12 // LED amarelo no GPIO 12
#define LED_GREEN_PIN  13 // LED verde no GPIO 13

// Estado do semáforo (0: vermelho, 1: amarelo, 2: verde)
volatile int semaphore_state = 0;

// Função de callback do temporizador
bool repeating_timer_callback(struct repeating_timer *t) {
    // Alterar o estado do semáforo a cada 3 segundos
    if (semaphore_state == 0) { // Vermelho
        gpio_put(LED_RED_PIN, 0);    // Desliga o LED vermelho
        gpio_put(LED_YELLOW_PIN, 1); // Liga o LED amarelo
        gpio_put(LED_GREEN_PIN, 0);  // Desliga o LED verde
        semaphore_state = 1;         // Próximo estado será o amarelo
    } else if (semaphore_state == 1) { // Amarelo
        gpio_put(LED_RED_PIN, 0);    // Desliga o LED vermelho
        gpio_put(LED_YELLOW_PIN, 0); // Desliga o LED amarelo
        gpio_put(LED_GREEN_PIN, 1);  // Liga o LED verde
        semaphore_state = 2;         // Próximo estado será o verde
    } else { // Verde
        gpio_put(LED_RED_PIN, 1);    // Liga o LED vermelho
        gpio_put(LED_YELLOW_PIN, 0); // Desliga o LED amarelo
        gpio_put(LED_GREEN_PIN, 0);  // Desliga o LED verde
        semaphore_state = 0;         // Próximo estado será o vermelho
    }

    return true;  // Retorna true para que o temporizador continue a ser chamado
}

int main() {
    // Inicializando o UART para impressão de mensagens no terminal
    stdio_init_all();
    
    // Inicializando os pinos GPIO dos LEDs
    gpio_init(LED_RED_PIN);
    gpio_set_dir(LED_RED_PIN, GPIO_OUT);
    gpio_init(LED_YELLOW_PIN);
    gpio_set_dir(LED_YELLOW_PIN, GPIO_OUT);
    gpio_init(LED_GREEN_PIN);
    gpio_set_dir(LED_GREEN_PIN, GPIO_OUT);

    // Iniciando o semáforo com o LED vermelho ligado
    gpio_put(LED_RED_PIN, 1);
    gpio_put(LED_YELLOW_PIN, 0);
    gpio_put(LED_GREEN_PIN, 0);
    
    // Definindo o temporizador repetitivo para 3 segundos (3000 ms)
    struct repeating_timer timer;
    add_repeating_timer_ms(3000, repeating_timer_callback, NULL, &timer);

    // Loop principal
    while (true) {
        // Espera 1 segundo e imprime uma mensagem na porta serial
        sleep_ms(1000);
        printf("Semáforo: %s\n", (semaphore_state == 0) ? "Vermelho" : 
                                  (semaphore_state == 1) ? "Amarelo" : "Verde");
    }

    return 0;
}
