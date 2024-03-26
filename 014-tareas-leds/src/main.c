#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define ledR 16
#define ledG 17
#define ledB 5
#define STACK_SIZE 1024*2

#define R_DELAY 500
#define G_DELAY 1000
#define B_DELAY 100

const char *TAG = "Tareas";

esp_err_t init_led(void);
esp_err_t create_tasks(void);

void vTaskLedR(void *pvParameters);
void vTaskLedG(void *pvParameters);
void vTaskLedB(void *pvParameters);

void app_main()
{
    init_led();
    create_tasks();
    while (1)
    {
        // gpio_set_level(ledR, 1);
        // gpio_set_level(ledG, 1);
        // gpio_set_level(ledB, 1);
        // vTaskDelay(pdMS_TO_TICKS(500));

        // gpio_set_level(ledR, 0);
        // gpio_set_level(ledG, 0);
        // gpio_set_level(ledB, 0);
        // vTaskDelay(pdMS_TO_TICKS(500));
        vTaskDelay(pdMS_TO_TICKS(1000));
        printf("hello form main\n");
    }
}

esp_err_t init_led()
{
    gpio_reset_pin(ledR);
    gpio_set_direction(ledR, GPIO_MODE_OUTPUT);
    gpio_reset_pin(ledG);
    gpio_set_direction(ledG, GPIO_MODE_OUTPUT);
    gpio_reset_pin(ledB);
    gpio_set_direction(ledB, GPIO_MODE_OUTPUT);
    return ESP_OK;
}

esp_err_t create_tasks(void)
{
    ESP_LOGI(TAG, "Number of Cores: %i", portNUM_PROCESSORS);
    
    static uint8_t ucParameterToPass;
    TaskHandle_t xHandle = NULL;
    //xTaskCreate( vTaskLedR, "vTaskLedR", STACK_SIZE, &ucParameterToPass, 1, &xHandle );
    xTaskCreatePinnedToCore( vTaskLedR, "vTaskLedR", STACK_SIZE, &ucParameterToPass, 1, &xHandle, 0 );
    //xTaskCreate( vTaskLedG, "vTaskLedG", STACK_SIZE, &ucParameterToPass, 1, &xHandle );
    xTaskCreatePinnedToCore( vTaskLedG, "vTaskLedG", STACK_SIZE, &ucParameterToPass, 1, &xHandle, 1 );    
    //xTaskCreate( vTaskLedB, "vTaskLedB", STACK_SIZE, &ucParameterToPass, 1, &xHandle );
    xTaskCreatePinnedToCore( vTaskLedB, "vTaskLedB", STACK_SIZE, &ucParameterToPass, 1, &xHandle, tskNO_AFFINITY );
    
    return ESP_OK;
}

void vTaskLedR(void *pvParameters)
{
    for (;;)
    {
        // Task code goes here.
        // Task code goes here.
        ESP_LOGI(TAG, "Led R");
        gpio_set_level(ledR, 1);
        vTaskDelay(pdMS_TO_TICKS(R_DELAY));
        gpio_set_level(ledR, 0);
        vTaskDelay(pdMS_TO_TICKS(R_DELAY));

    }
}

void vTaskLedG(void *pvParameters)
{
    for (;;)
    {
        // Task code goes here.
        ESP_LOGI(TAG, "Led G");
        gpio_set_level(ledG, 1);
        vTaskDelay(pdMS_TO_TICKS(G_DELAY));
        gpio_set_level(ledG, 0);
        vTaskDelay(pdMS_TO_TICKS(G_DELAY));

    }
}

void vTaskLedB(void *pvParameters)
{
    for (;;)
    {
        // Task code goes here.
        // Task code goes here.
        ESP_LOGI(TAG, "Led B");
        gpio_set_level(ledB, 1);
        vTaskDelay(pdMS_TO_TICKS(B_DELAY));
        gpio_set_level(ledB, 0);
        vTaskDelay(pdMS_TO_TICKS(B_DELAY));
    }
}
