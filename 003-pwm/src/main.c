#include <stdio.h>
#include "driver/ledc.h"
#include "esp_err.h"

#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO (5) // Defino la salida GPIO
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT // Establecer la resolución de duty en 13 bits
#define LEDC_DUTY (4096)                // Establezca el duty al 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY (4000)           // Frecuencia en hercios. Establecer la frecuencia a 4 kHz

static void ledc_init(void);

void app_main() {
    // Establecer la configuración del periférico LEDC
    ledc_init();
    // Establecer el duty al 50%
    ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY));
    // Actualizar deber para aplicar el nuevo valor.
    ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

void ledc_init(void)
{
    // Preparar y luego aplicar la configuración del temporizador LEDC PWM
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_MODE,
        .duty_resolution = LEDC_DUTY_RES,
        .timer_num = LEDC_TIMER,
        .freq_hz = LEDC_FREQUENCY, 
        .clk_cfg = LEDC_AUTO_CLK};
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Preparar y luego aplicar la configuración del canal LEDC PWM
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

}
