#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_log.h"

static const char *TAG = "002_PWM";

int dutyR = 0;
int dutyG = 300;
int dutyB = 600;
int interval = 50;
int timerId = 1; 
TimerHandle_t xTimers;

esp_err_t set_pwm(void);
esp_err_t set_pwm_duty(void);
esp_err_t set_timer(void);

void vTimerCallback( TimerHandle_t pxTimer ){
   
   ESP_LOGI(TAG, "Event was called from timer");
   dutyR +=10;
   if (dutyR > 1023) 
    dutyR = 0;
   dutyG +=10;
   if (dutyG > 1023) 
    dutyG = 0;
   dutyB +=10;
   if (dutyB > 1023) 
    dutyB = 0;

   set_pwm_duty();
   
}

void app_main(void)
{
    set_pwm();
    set_timer();
}

esp_err_t set_pwm(void)
{
    
    ledc_timer_config_t ledc_timer = {0}; 
    ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_timer.duty_resolution = LEDC_TIMER_10_BIT;        
    ledc_timer.timer_num = LEDC_TIMER_0;
    ledc_timer.freq_hz = 20000; // 20KHz

    ledc_timer_config(&ledc_timer);

    
    ledc_channel_config_t ledc_channelR = {0};
        ledc_channelR.gpio_num = 17;
        ledc_channelR.speed_mode = LEDC_HIGH_SPEED_MODE;
        ledc_channelR.channel = LEDC_CHANNEL_0;
        ledc_channelR.intr_type = LEDC_INTR_DISABLE;
        ledc_channelR.timer_sel = LEDC_TIMER_0;
        ledc_channelR.duty = 0;
    

    ledc_channel_config_t ledc_channelG = {0};
        ledc_channelG.gpio_num = 18;
        ledc_channelG.speed_mode = LEDC_HIGH_SPEED_MODE;
        ledc_channelG.channel = LEDC_CHANNEL_1;
        ledc_channelG.intr_type = LEDC_INTR_DISABLE;
        ledc_channelG.timer_sel = LEDC_TIMER_0;
        ledc_channelG.duty = 0;


    ledc_channel_config_t ledc_channelB = {0};
        ledc_channelB.gpio_num = 19;
        ledc_channelB.speed_mode = LEDC_HIGH_SPEED_MODE;
        ledc_channelB.channel = LEDC_CHANNEL_2;
        ledc_channelB.intr_type = LEDC_INTR_DISABLE;
        ledc_channelB.timer_sel = LEDC_TIMER_0;
        ledc_channelB.duty = 0;
    

    ledc_channel_config(&ledc_channelR);
    ledc_channel_config(&ledc_channelG);
    ledc_channel_config(&ledc_channelB);

    return ESP_OK;
}

esp_err_t set_pwm_duty(void)
{
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, dutyR);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, dutyG);
    ledc_set_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, dutyB);

    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1);
    ledc_update_duty(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2);


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