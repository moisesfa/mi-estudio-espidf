#include "sub_ui.h"

#define ROTENC_CLK_PIN 19
#define ROTENC_DT_PIN 18
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST 15

#define BUZZER_PIN 17

// https://github.com/nopnop2002/esp-idf-ssd1306

static void init_subsystems(void);
static void update_power_man(void);

extern "C" void app_main(void) 
{
    init_subsystems();
}

static void update_power_man(void)
{
    //pmsub_update(false);
}

static void init_subsystems(void)
{
    uisub_config_t ui_cfg = {
        .buzzer_pin = BUZZER_PIN,
        .rotenc_clk_pin = ROTENC_CLK_PIN,
        .rotenc_dt_pin = ROTENC_DT_PIN,
        .rotenc_changed = update_power_man,
        .oled_sda = OLED_SDA,
        .oled_scl = OLED_SCL,
        .oled_rst = OLED_RST
    };
    uisub_init(ui_cfg);
}

