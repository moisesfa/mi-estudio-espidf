
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED 2
uint8_t led_level = 0;
uint8_t count = 0;

static const char *TAG = "002_GPIO";

// Declaración de funciones 
esp_err_t init_led(void);
esp_err_t update_led(void);

void app_main() {
    init_led();

    while (1)
    {
        vTaskDelay(200/portTICK_PERIOD_MS);
        update_led();
        count +=1;

        if (count>30){
            count = 0;            
        }
        if (count<10){
            ESP_LOGI(TAG, "Value: %u.", count);            
        }
        if (count>=10 && count<20){            
            ESP_LOGW(TAG, "Value: %u.", count);
        }
        if (count>=20 ){            
            ESP_LOGE(TAG, "Value: %u.", count);
        }

        // printf("Led level: %u\n", led_level);        
    }    
}

esp_err_t init_led(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);   
    return ESP_OK;
}

esp_err_t update_led(void)
{
    led_level = !led_level;
    gpio_set_level(LED, led_level);
    return ESP_OK;
}
