#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_log.h"

#define ledR 16
#define ledG 17

#define STACK_SIZE 1024 * 2

#define R_DELAY 10000
#define G_DELAY 2000

SemaphoreHandle_t xSemaphoreBin = NULL;

const char *TAG = "Tareas";

esp_err_t init_led(void);
esp_err_t create_tasks(void);
esp_err_t shared_resource(int led);

void vTaskLedR(void *pvParameters);
void vTaskLedG(void *pvParameters);

void app_main()
{
    xSemaphoreBin = xSemaphoreCreateBinary();
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

// Recurso compartido
esp_err_t shared_resource(int led)
{
    for (size_t i = 0; i < 8; i++)
    {
        vTaskDelay(pdMS_TO_TICKS(400));
        gpio_set_level(led, 1);
        vTaskDelay(pdMS_TO_TICKS(400));
        gpio_set_level(led, 0);
    }

    return ESP_OK;
}

void vTaskLedR(void *pvParameters)
{
    for (;;)
    {        
        shared_resource(ledR);
        ESP_LOGI(TAG, "La Tarea R entrega la llave");
        xSemaphoreGive(xSemaphoreBin);
        vTaskDelay(pdMS_TO_TICKS(R_DELAY));
    }
}

void vTaskLedG(void *pvParameters)
{

    for (;;)
    {
        // Puede estar esperando aprox. 12 horas hasta que le entregen la llave
        if (xSemaphoreTake(xSemaphoreBin, portMAX_DELAY))
        {
            ESP_LOGW(TAG, "La Tarea G esta trabajando");
            shared_resource(ledG);
            ESP_LOGW(TAG, "La Tarea G esta durmiendo");            
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}