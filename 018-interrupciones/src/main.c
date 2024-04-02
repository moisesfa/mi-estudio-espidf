#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#define ledR 16
#define ledG 17
#define ledB 5
#define button_isr 27

const char *TAG = "Interrupciones";
uint8_t count = 0;

esp_err_t init_led(void);
esp_err_t init_isr(void);
void isr_handler (void *args);

void app_main()
{
    init_led(); 
    init_isr();

    while (1)
    {
        switch (count)
        {
        case 0:
            ESP_LOGW(TAG, "ledR,ledG,ledB apagados");
            break;
        case 1:
            ESP_LOGW(TAG, "ledR encendido");
            break;
        case 2:
            ESP_LOGW(TAG, "ledG encendido");
            break;
        case 3:
            ESP_LOGW(TAG, "ledB encendido");
            break;
        default:
            break;
        }
    vTaskDelay(pdMS_TO_TICKS(2000));
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
    ESP_LOGI(TAG, "Init led completado");
    return ESP_OK;
}

esp_err_t init_isr(void)
{
    gpio_config_t pGPIOConfig;

    pGPIOConfig.pin_bit_mask = (1ULL << button_isr);       /*!< GPIO pin: set with bit mask, each bit maps to a GPIO */
    pGPIOConfig.mode = GPIO_MODE_DEF_INPUT;                /*!< GPIO mode: set input/output mode                     */
    pGPIOConfig.pull_up_en = GPIO_PULLUP_DISABLE;          /*!< GPIO pull-up                                         */
    pGPIOConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;      /*!< GPIO pull-down                                       */
    pGPIOConfig.intr_type = GPIO_INTR_NEGEDGE;             /*!< GPIO interrupt type                                  */

    gpio_config(&pGPIOConfig);
    
    gpio_install_isr_service(0);
    gpio_isr_handler_add(button_isr, isr_handler, NULL);
    
    ESP_LOGI(TAG, "Init isr completado");
    return ESP_OK;
}

void isr_handler(void *args)
{
    count ++;
    if (count > 3)
        count = 0;
    
    switch (count)
    {
    case 0:
        gpio_set_level(ledR, 0);
        gpio_set_level(ledG, 0);
        gpio_set_level(ledB, 0);
        break;
    case 1:
        gpio_set_level(ledR, 1);
        gpio_set_level(ledG, 0);
        gpio_set_level(ledB, 0);
        break;
    case 2:
        gpio_set_level(ledR, 0);
        gpio_set_level(ledG, 1);
        gpio_set_level(ledB, 0);
        break;
    case 3:
        gpio_set_level(ledR, 0);
        gpio_set_level(ledG, 0);
        gpio_set_level(ledB, 1);
        break;
    
    default:
        break;
    }

}
