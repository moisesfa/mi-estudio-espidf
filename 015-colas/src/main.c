// Tarea R - consigue los datos de una manera mas rápida 400ms
// Tarea G - procesa los datos cada 2000 ms

#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#define ledR 16
#define ledG 17

#define STACK_SIZE 1024 * 2

#define R_DELAY 400
#define G_DELAY 2000

const char *TAG = "Tareas";

QueueHandle_t globalQueue = 0;

esp_err_t init_led(void);
esp_err_t create_tasks(void);

void vTaskLedR(void *pvParameters);
void vTaskLedG(void *pvParameters);

void app_main()
{ // Creamos la cola de enteros
    globalQueue = xQueueCreate(20, sizeof(uint32_t));
    init_led();
    create_tasks();
}

esp_err_t init_led()
{
    gpio_reset_pin(ledR);
    gpio_set_direction(ledR, GPIO_MODE_OUTPUT);
    gpio_reset_pin(ledG);
    gpio_set_direction(ledG, GPIO_MODE_OUTPUT);
    return ESP_OK;
}

esp_err_t create_tasks(void)
{
    ESP_LOGI(TAG, "Number of Cores: %i", portNUM_PROCESSORS);

    static uint8_t ucParameterToPass;
    TaskHandle_t xHandle = NULL;
    xTaskCreatePinnedToCore(vTaskLedR, "vTaskLedR", STACK_SIZE, &ucParameterToPass, 1, &xHandle, 0);
    xTaskCreatePinnedToCore(vTaskLedG, "vTaskLedG", STACK_SIZE, &ucParameterToPass, 1, &xHandle, 1);

    return ESP_OK;
}

void vTaskLedR(void *pvParameters)
{
    for (;;)
    {
        // Task code goes here.
        // Enviamos datos generados en el for a la cola
        for (size_t i = 0; i < 8; i++)
        {
            vTaskDelay(pdMS_TO_TICKS(R_DELAY / 2));
            gpio_set_level(ledR, 1);
            ESP_LOGW(TAG, "Enviando %i a la cola", i);
            // Intentamos enviar el dato
            if (!xQueueSend(globalQueue, &i, pdMS_TO_TICKS(100)))
                ESP_LOGE(TAG, "Error al enviar %i a la cola", i);

            vTaskDelay(pdMS_TO_TICKS(R_DELAY / 2));
            gpio_set_level(ledR, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(7000));
    }
}

void vTaskLedG(void *pvParameters)
{
    // Task code goes here.
    // Recibiendo datos de la cola
    int receivedValue = 0;    
    for (;;)
    {

        if (!xQueueReceive(globalQueue, &receivedValue, pdMS_TO_TICKS(100)))
        {
            ESP_LOGE(TAG, "Error al recibir valores de la cola");
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(G_DELAY / 2));
            gpio_set_level(ledG, 1);
            ESP_LOGI(TAG, "Valor recibido %i de la cola", receivedValue);
            vTaskDelay(pdMS_TO_TICKS(G_DELAY / 2));
            gpio_set_level(ledG, 0);
        }        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}