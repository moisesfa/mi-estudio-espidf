
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "esp_log.h"

#define LED 2
uint8_t led_level = 0;

int interval = 1000;
int timerId = 1; 

static const char *TAG = "002_GPIO";
TimerHandle_t xTimers;

// Declaración de funciones 
esp_err_t init_led(void);
esp_err_t update_led(void);
esp_err_t set_timer(void);

void vTimerCallback( TimerHandle_t pxTimer ){
   
   ESP_LOGI(TAG, "Event was called from timer");
   update_led();

}

void app_main() {
    init_led();
    set_timer();
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
