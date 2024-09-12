#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

void led_1_task(void *p) {
  gpio_init(LED_PIN_R);
  gpio_set_dir(LED_PIN_R, GPIO_OUT);

  int delay = 250; // tempo em milissegundos
  while (true) {
    gpio_put(LED_PIN_R, 1); // Liga o LED vermelho
    vTaskDelay(pdMS_TO_TICKS(delay)); // Espera o tempo definido
    gpio_put(LED_PIN_R, 0); // Desliga o LED vermelho
    vTaskDelay(pdMS_TO_TICKS(delay)); // Espera novamente
  }
}

void led_2_task(void *p) {
  gpio_init(LED_PIN_G);
  gpio_set_dir(LED_PIN_G, GPIO_OUT);

  int delay = 500; // Define um delay diferente para o LED verde
  while (true) {
    gpio_put(LED_PIN_G, 1); // Liga o LED verde
    vTaskDelay(pdMS_TO_TICKS(delay)); // Espera o tempo definido
    gpio_put(LED_PIN_G, 0); // Desliga o LED verde
    vTaskDelay(pdMS_TO_TICKS(delay)); // Espera novamente
  }
}

int main() {
  stdio_init_all();
  printf("Start RTOS \n");

  // Cria as tasks para os LEDs
  xTaskCreate(led_1_task, "LED_Task 1", 256, NULL, 1, NULL);
  xTaskCreate(led_2_task, "LED_Task 2", 256, NULL, 1, NULL);

  vTaskStartScheduler(); // Inicia o agendador

  while (true)
    ;
}
