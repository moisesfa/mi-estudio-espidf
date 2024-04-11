
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include "sub_ui.h"
#include <driver/gpio.h>
#include "sub_rotenc.h"
#include "esp_log.h"
#include "ssd1306.h"

#define APPTAG "mfiles"
#define DISP_MAX_X 127
#define DISP_MAX_Y 63

SSD1306_t dev;

static void rotenc_handler(void *arg);
static int rotenc_pos = 0;
static uisub_config_t config;
static SemaphoreHandle_t disp_guard;
static void reset_display(void);

void uisub_init(uisub_config_t c)
{
    config = c;

    gpio_config_t io_conf;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << config.buzzer_pin);
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    rotenc_init(config.rotenc_clk_pin, config.rotenc_dt_pin);
    rotenc_setPosChangedCallback(rotenc_handler);

    #if CONFIG_I2C_INTERFACE
	ESP_LOGI(APPTAG, "INTERFACE is I2C");
	//ESP_LOGI(APPTAG, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	//ESP_LOGI(APPTAG, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	//ESP_LOGI(APPTAG, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&dev, config.oled_sda, config.oled_scl, config.oled_rst);
    #endif // CONFIG_I2C_INTERFACE

    #if CONFIG_SSD1306_128x64
	ESP_LOGI(APPTAG, "Panel is 128x64");
	ssd1306_init(&dev, 128, 64);
    #endif // CONFIG_SSD1306_128x64

    ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
    char msg[] = "Hello";
	ssd1306_display_text_x3(&dev, 0, msg, 5, false);
	vTaskDelay(3000 / portTICK_PERIOD_MS);

    disp_guard = xSemaphoreCreateMutex();
    reset_display();
    
}


static void rotenc_handler(void *arg)
{
    while (1)
    {
        int pos1 = rotenc_getPos();
        ESP_LOGI(APPTAG, "rotenc_pos: %d", pos1);
        if (abs(pos1 - rotenc_pos) >= 4)
        {
            rotenc_pos = pos1;
            if (config.rotenc_changed != NULL)
            {
                config.rotenc_changed();
            }
            // TODO:
            //uisub_show(last_reading);
        }
        vTaskSuspend(NULL);
    }
}

static void reset_display(void)
{
    ssd1306_clear_screen(&dev, false);
    
    _ssd1306_line(&dev,0, 0, DISP_MAX_X, 0, false);
    _ssd1306_line(&dev,DISP_MAX_X, 1, DISP_MAX_X, DISP_MAX_Y, false);
    _ssd1306_line(&dev,DISP_MAX_X - 1, DISP_MAX_Y, 0, DISP_MAX_Y, false);
    _ssd1306_line(&dev,0, DISP_MAX_Y - 1, 0, 0, false);
    
    ssd1306_show_buffer(&dev);    
}