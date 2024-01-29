/* GPIO Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"

/**
 * Brief:
 * Este código de prueba muestra cómo configurar gpio y cómo usar la interrupción gpio.
 *
 * GPIO status:
 * GPIO18: salida 
 * GPIO19: salida 
 * GPIO4:  entrada, pulled up, interrupción desde el flanco ascendente y el flanco descendente
 * GPIO5:  entrada, pulled up, interrupción desde el flanco ascendente.
 *
 * Test:
 * Conectar GPIO18 con  GPIO4
 * Conectar GPIO19 con GPIO5
 * Genera pulsos en GPIO18/19, que desencadena una interrupción en GPIO4/5
 *
 */

#define GPIO_OUTPUT_IO_0    18
#define GPIO_OUTPUT_IO_1    19
#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_IO_0) | (1ULL<<GPIO_OUTPUT_IO_1))
/*
 * Digamos, GPIO_OUTPUT_IO_0=18, GPIO_OUTPUT_IO_1=19
 * En representación binaria,
 * 1ULL<<GPIO_OUTPUT_IO_0 es igual a  0000000000000000000001000000000000000000 y
 * 1ULL<<GPIO_OUTPUT_IO_1 es igual a  0000000000000000000010000000000000000000
 * GPIO_OUTPUT_PIN_SEL                0000000000000000000011000000000000000000
 * */
#define GPIO_INPUT_IO_0     4
#define GPIO_INPUT_IO_1     5
#define GPIO_INPUT_PIN_SEL  ((1ULL<<GPIO_INPUT_IO_0) | (1ULL<<GPIO_INPUT_IO_1))
/*
 * Digamos, GPIO_INPUT_IO_0=4, GPIO_INPUT_IO_1=5
 * En representación binaria,
 * 1ULL<<GPIO_INPUT_IO_0 es igual a  0000000000000000000000000000000000010000 y 
 * 1ULL<<GPIO_INPUT_IO_1 es igual a  0000000000000000000000000000000000100000
 * GPIO_INPUT_PIN_SEL                0000000000000000000000000000000000110000
 * */
#define ESP_INTR_FLAG_DEFAULT 0

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void gpio_task_example(void* arg)
{
    uint32_t io_num;
    for (;;) {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            printf("GPIO[%"PRIu32"] intr, val: %d\n", io_num, gpio_get_level(io_num));
        }
    }
}

void init_hw (void)
{
   //inicialice a cero la estructura de configuración.
    gpio_config_t io_conf = {};
    //desactivar la interrupción
    io_conf.intr_type = GPIO_INTR_DISABLE;
    //establecer como modo salida 
    io_conf.mode = GPIO_MODE_OUTPUT;
    //máscara de bits de los pines que desea configurar, por ejemplo GPIO18/19
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //desactivar modo pull-down
    io_conf.pull_down_en = 0;
    //desactivar modo pull-up
    io_conf.pull_up_en = 0;
    //configurar GPIO con la configuración dada Salidas
    gpio_config(&io_conf);

    //interrupción del flanco ascendente
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    //máscara de bits de los pines, use GPIO4/5 aquí
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //establecer como modo entrada 
    io_conf.mode = GPIO_MODE_INPUT;
    //activar modo pull-up 
    io_conf.pull_up_en = 1;
    //configurar GPIO con la configuración dada Entradas    
    gpio_config(&io_conf);

    //cambiar el tipo de interrupción gpio para un pin
    gpio_set_intr_type(GPIO_INPUT_IO_0, GPIO_INTR_ANYEDGE);

    //crear una cola para manejar el evento gpio desde isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    
    //inicio la tarea gpio 
    xTaskCreate(gpio_task_example, "gpio_task_example", 2048, NULL, 10, NULL);

    //instalar el servicio  gpio isr 
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //enganchar el manejador isr para pin gpio específico
    gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);
    //enganchar el manejador isr para pin gpio específico
    gpio_isr_handler_add(GPIO_INPUT_IO_1, gpio_isr_handler, (void*) GPIO_INPUT_IO_1);

    //remove isr handler for gpio number.
    //gpio_isr_handler_remove(GPIO_INPUT_IO_0);
    //hook isr handler for specific gpio pin again
    //gpio_isr_handler_add(GPIO_INPUT_IO_0, gpio_isr_handler, (void*) GPIO_INPUT_IO_0);

    printf("Minimum free heap size: %"PRIu32" bytes\n", esp_get_minimum_free_heap_size());


}

void app_main(void)
{    
    init_hw();
    
    int cnt = 0;
    while (1) {
        printf("cnt: %d\n", cnt++);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // Una forma elegante de cambiar el estado del pin
        gpio_set_level(GPIO_OUTPUT_IO_0, cnt % 2);
        gpio_set_level(GPIO_OUTPUT_IO_1, cnt % 2);
    }
}