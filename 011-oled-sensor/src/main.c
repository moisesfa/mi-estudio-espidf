#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "dht.h"
#include "ssd1306.h"

//https://github.com/nopnop2002/esp-idf-ssd1306

#define tag "SSD1306"
#define DHT11_PIN 17

SSD1306_t dev;

void init_hw(void)
{
/*
 You have to set this config value with menuconfig
 CONFIG_INTERFACE

 for i2c
 CONFIG_MODEL
 CONFIG_SDA_GPIO
 CONFIG_SCL_GPIO
 CONFIG_RESET_GPIO

 for SPI
 CONFIG_CS_GPIO
 CONFIG_DC_GPIO
 CONFIG_RESET_GPIO
*/

#if CONFIG_I2C_INTERFACE
	ESP_LOGI(tag, "INTERFACE is I2C");
	ESP_LOGI(tag, "CONFIG_SDA_GPIO=%d",CONFIG_SDA_GPIO);
	ESP_LOGI(tag, "CONFIG_SCL_GPIO=%d",CONFIG_SCL_GPIO);
	ESP_LOGI(tag, "CONFIG_RESET_GPIO=%d",CONFIG_RESET_GPIO);
	i2c_master_init(&dev, CONFIG_SDA_GPIO, CONFIG_SCL_GPIO, CONFIG_RESET_GPIO);
#endif // CONFIG_I2C_INTERFACE

#if CONFIG_SSD1306_128x64
	ESP_LOGI(tag, "Panel is 128x64");
	ssd1306_init(&dev, 128, 64);
#endif // CONFIG_SSD1306_128x64

	ssd1306_clear_screen(&dev, false);
	ssd1306_contrast(&dev, 0xff);
	ssd1306_display_text_x3(&dev, 0, "Hello", 5, false);
	ssd1306_display_text_x3(&dev, 4, "DHT11", 5, false);
	vTaskDelay(3000 / portTICK_PERIOD_MS);
	
}

static void draw_screen(void)
{
    ssd1306_clear_screen(&dev, false);    
    ssd1306_display_text(&dev, 1, "Tem:     Hum:", 13, false);
}

static void display_reading(int temp, int hum)
{
    char lineChar[20];
    //ssd1306_clear_screen(&dev, false);    
    sprintf(lineChar, " %d C       %d %%    ", temp, hum);
    ssd1306_display_text(&dev, 3, lineChar,20, false);
}

static void read_dht11(void* arg)
{
    int16_t humidity = 0, temperature = 0;
    while(1)
    {
        vTaskDelay(3000 / portTICK_PERIOD_MS);
        dht_read_data(DHT_TYPE_DHT11, (gpio_num_t)DHT11_PIN, &humidity, &temperature);
        printf("Humidity: %d%% Temp: %dC\n", humidity/10, temperature/10);
        display_reading(temperature / 10, humidity / 10);
    }
}

void app_main(void)
{
	
	init_hw();
    draw_screen();	
	xTaskCreate(read_dht11, "dht11", configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL);

}