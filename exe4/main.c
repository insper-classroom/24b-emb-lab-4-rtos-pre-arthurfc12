#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

QueueHandle_t xQueueButId;
QueueHandle_t xQueueButId2;

// Callback do botão vermelho (BTN_PIN_R)
void btn_callback_r(uint gpio, uint32_t events) {
    static int delay_r = 100;
    
    // A interrupção é disparada na borda de descida (falling edge)
    if (gpio == BTN_PIN_R && events == GPIO_IRQ_EDGE_FALL) {
        // Aumenta o delay até 1000ms e depois reinicia para 100ms
        if (delay_r < 1000) {
            delay_r += 100;
        } else {
            delay_r = 100;
        }

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(xQueueButId, &delay_r, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

// Callback do botão verde (BTN_PIN_G)
void btn_callback_g(uint gpio, uint32_t events) {
    static int delay_g = 100;

    // A interrupção é disparada na borda de descida (falling edge)
    if (gpio == BTN_PIN_G && events == GPIO_IRQ_EDGE_FALL) {
        // Aumenta o delay até 1000ms e depois reinicia para 100ms
        if (delay_g < 1000) {
            delay_g += 100;
        } else {
            delay_g = 100;
        }

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xQueueSendFromISR(xQueueButId2, &delay_g, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

// Task para controle do LED vermelho (LED_PIN_R)
void led_1_task(void *p) {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    int delay = 0;
    while (true) {
        if (xQueueReceive(xQueueButId, &delay, portMAX_DELAY)) {
            printf("LED R delay: %d\n", delay);
        }

        if (delay > 0) {
            gpio_put(LED_PIN_R, 1);
            vTaskDelay(pdMS_TO_TICKS(delay));
            gpio_put(LED_PIN_R, 0);
            vTaskDelay(pdMS_TO_TICKS(delay));
        }
    }
}

// Task para controle do LED verde (LED_PIN_G)
void led_2_task(void *p) {
    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    int delay = 0;
    while (true) {
        if (xQueueReceive(xQueueButId2, &delay, portMAX_DELAY)) {
            printf("LED G delay: %d\n", delay);
        }

        if (delay > 0) {
            gpio_put(LED_PIN_G, 1);
            vTaskDelay(pdMS_TO_TICKS(delay));
            gpio_put(LED_PIN_G, 0);
            vTaskDelay(pdMS_TO_TICKS(delay));
        }
    }
}

int main() {
    stdio_init_all();
    printf("Start RTOS \n");

    // Criação das filas
    xQueueButId = xQueueCreate(32, sizeof(int));
    xQueueButId2 = xQueueCreate(32, sizeof(int));

    // Configuração das interrupções dos botões
    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);
    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback_r);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);
    gpio_set_irq_enabled_with_callback(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true, &btn_callback_g);

    // Criação das tasks para os LEDs
    xTaskCreate(led_1_task, "LED_Task 1", 256, NULL, 1, NULL);
    xTaskCreate(led_2_task, "LED_Task 2", 256, NULL, 1, NULL);

    // Inicia o agendador
    vTaskStartScheduler();

    while (true)
        ;
}
