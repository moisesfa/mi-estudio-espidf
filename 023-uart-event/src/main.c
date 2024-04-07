#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "freertos/queue.h"

static const char *TAG = "UART EVENT";

#define ledR 14
#define ledG 12
#define ledB 13
#define UART_NUM UART_NUM_2
#define BUF_SIZE 1024
#define TASK_MEMORY 1024 * 2

// Declaración de la cola
static QueueHandle_t uart_queue;

static void uart_task(void *pvParameters)
{
    // Uart Event
    uart_event_t uart_event;
    // Puntero para almacenar los datos
    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);

    while (1)
    {
        // Aqui se entrará cuando se reciba algún mensaje mediante eventos
        if (xQueueReceive(uart_queue, (void *)&uart_event, portMAX_DELAY))
        {
            // Borrar el espacio de memoria de data
            bzero(data, BUF_SIZE);

            switch (uart_event.type)
            {
            // Trabajamos con uart_data (hay datos en el uart) pero hay más eventos
            case UART_DATA:
                // Leemos lo que nos puedamn enviar por el puerto
                uart_read_bytes(UART_NUM, data, uart_event.size, pdMS_TO_TICKS(100));
                // Volvemos a enviar lo recibido (eco)
                uart_write_bytes(UART_NUM, (const char *)data, uart_event.size);
                // Limpiamos el buffer de entrada
                uart_flush(UART_NUM);

                // Imprimimos en consola lo recibido
                ESP_LOGI(TAG, "Data recibida: %s", data);

                // Manejo de los leds dependiendo de lo recibido (No hay \n ni \r)
                for (size_t i = 0; i < uart_event.size; i++)
                {
                    char value = data[i];
                    switch (value)
                    {
                    case 'R':
                        gpio_set_level(ledR, 1);
                        gpio_set_level(ledG, 0);
                        gpio_set_level(ledB, 0);
                        break;

                    case 'G':
                        gpio_set_level(ledR, 0);
                        gpio_set_level(ledG, 1);
                        gpio_set_level(ledB, 0);
                        break;

                    case 'B':
                        gpio_set_level(ledR, 0);
                        gpio_set_level(ledG, 0);
                        gpio_set_level(ledB, 1);
                        break;

                    default:
                        gpio_set_level(ledR, 0);
                        gpio_set_level(ledG, 0);
                        gpio_set_level(ledB, 0);
                        break;
                    }
                }
                break;

            default:
                break;
            }
        }
    }
}

static void init_uart(void)
{
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB};
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, 5, 4, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // Trabajando sin queues
    // uart_driver_install(UART_NUM, BUF_SIZE, BUF_SIZE, 0, NULL, 0);
    // Trabajando con queues
    uart_driver_install(UART_NUM, BUF_SIZE, BUF_SIZE, 5, &uart_queue, 0);

    xTaskCreate(uart_task, "uart_task", TASK_MEMORY, NULL, 5, NULL);

    ESP_LOGI(TAG, "Inicialización de Uart Completada");
}

static void init_led(void)
{
    gpio_reset_pin(ledR);
    gpio_set_direction(ledR, GPIO_MODE_OUTPUT);

    gpio_reset_pin(ledG);
    gpio_set_direction(ledG, GPIO_MODE_OUTPUT);

    gpio_reset_pin(ledB);
    gpio_set_direction(ledB, GPIO_MODE_OUTPUT);

    ESP_LOGI(TAG, "Inicialización de Led Compleada");
}

void app_main()
{
    init_led();
    init_uart();
}