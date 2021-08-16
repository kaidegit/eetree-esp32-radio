//
// Created by yekai on 2021/8/5.
//

#include <esp_err.h>
#include <nvs_flash.h>
#include "smartconfig.h"
#include "oled.h"
#include "gpio.h"
#include "spi.h"
#include "i2c.h"
#include "driver/i2c.h"
#include "main.h"
#include "audio.h"
#include "fm.h"


void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    MY_GPIO_Init();
    MY_SPI_Init();
    MY_I2C_Init();
    //TODO:Audio is not available.
//    MY_AUDIO_Init();
    RDA_Init();
    SetMuxFM();
    EnableSpeaker();
    RDA_SetBandFrequency(RADIO_BAND_US,(int)(102.1 * 1000));
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(0, 0, (uint8_t *) "tm isnt synced", 16);

//    gpio_set_level(SPEAKER_EN_Pin, 1);

//    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
//    i2c_master_start(cmd);
//    i2c_master_write_byte(cmd, 0x21, 1);
//    uint8_t data_h = 0, data_l = 0;
//    i2c_master_read_byte(cmd, &data_h, 0);
//    i2c_master_read_byte(cmd, &data_l, 0);
//    i2c_master_read_byte(cmd, &data_h, 0);
//    i2c_master_read_byte(cmd, &data_l, 0);
//    i2c_master_read_byte(cmd, &data_h, 0);
//    i2c_master_read_byte(cmd, &data_l, 1);
//    i2c_master_stop(cmd);
//    i2c_master_cmd_begin(FM_I2C_HOST, cmd, 1000 / portTICK_RATE_MS);
//    i2c_cmd_link_delete(cmd);

//    char ch[30];
//    sprintf(ch, "%#X", (data_h << 8) + data_l);
//    OLED_ShowString(0, 0, (uint8_t *) ch, 16);

    initialise_wifi();
}

