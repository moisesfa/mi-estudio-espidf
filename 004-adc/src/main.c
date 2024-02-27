#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "driver/adc.h"

#define LED 2
#define LEDR 33
#define LEDG 25
#define LEDB 26
uint8_t led_level = 0;

int interval = 1000;
int timerId = 1; 
int adc_val = 0; 

static const char *TAG = "002_GPIO";
TimerHandle_t xTimers;

// Declaración de funciones 
esp_err_t init_led(void);
esp_err_t update_led(void);
esp_err_t set_timer(void);
esp_err_t set_adc(void);

void vTimerCallback( TimerHandle_t pxTimer ){
   
   ESP_LOGI(TAG, "Event was called from timer");
   update_led();
   adc_val = adc1_get_raw(ADC1_CHANNEL_4);

}

void app_main() {
    init_led();
    set_timer();
}

esp_err_t init_led(void)
{
    gpio_reset_pin(LED);
    gpio_set_direction(LED, GPIO_MODE_OUTPUT);   
    
    gpio_reset_pin(LEDR);
    gpio_set_direction(LEDR, GPIO_MODE_OUTPUT);   
    gpio_reset_pin(LEDG);
    gpio_set_direction(LEDG, GPIO_MODE_OUTPUT);   
    gpio_reset_pin(LEDB);
    gpio_set_direction(LEDB, GPIO_MODE_OUTPUT);   

    return ESP_OK;
}

esp_err_t update_led(void)
{
    led_level = !led_level;
    gpio_set_level(LED, led_level);
    
    ESP_LOGI(TAG, "ADC VAL: %i ", adc_val);

    int adc_case = adc_val /1000; //(valores de 0 a 4)

    switch (adc_case)
    {
    case 0:    
        gpio_set_level(LEDR, 0);
        gpio_set_level(LEDG, 0);
        gpio_set_level(LEDB, 0);
        break;
    case 1:    
        gpio_set_level(LEDR, 1);
        gpio_set_level(LEDG, 0);
        gpio_set_level(LEDB, 0);
        break;
    case 2:    
        gpio_set_level(LEDR, 1);
        gpio_set_level(LEDG, 1);
        gpio_set_level(LEDB, 0);
        break;
    case 3:    
    case 4:    
        gpio_set_level(LEDR, 1);
        gpio_set_level(LEDG, 1);
        gpio_set_level(LEDB, 1);
        break;
    
    default:
        break;
    }

    


    return ESP_OK;
}

esp_err_t set_timer(void)
{
    ESP_LOGI(TAG, "Timer init configuration");
 
    xTimers = xTimerCreate("Timer",       // Just a text name, not used by the kernel.
                              (pdMS_TO_TICKS(interval)),     // The timer period in ticks.
                              pdTRUE,        // The timers will auto-reload themselves when they expire.
                              (void *)timerId,     // Assign each timer a unique id equal to its array index.
                              vTimerCallback // Each timer calls the same callback when it expires.
    );

    if (xTimers == NULL)
    {
        // The timer was not created.
        ESP_LOGE(TAG, "The timer was not created.");
    }
    else
    {
        // Start the timer.  No block time is specified, and even if one was
        // it would be ignored because the scheduler has not yet been
        // started.
        if (xTimerStart(xTimers, 0) != pdPASS)
        {
            // The timer could not be set into the Active state.
            ESP_LOGE(TAG, "The timer could not be set into the Active state.");
        }
    }
    
    return ESP_OK;
}

esp_err_t set_adc(void)
{
    // Configuración del ADC
    adc1_config_channel_atten(ADC_CHANNEL_4, ADC_ATTEN_DB_11);
    adc1_config_width(ADC_WIDTH_BIT_12);
    
    return ESP_OK;
}
