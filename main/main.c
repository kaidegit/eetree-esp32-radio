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
#include "time_calibrate.h"


void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    MY_GPIO_Init();
    MY_SPI_Init();
    MY_I2C_Init();

    RDA_Init();
    SetMuxFM();
    EnableSpeaker();
    RDA_SetBandFrequency(RADIO_BAND_US, (int) (102.1 * 1000));
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(0, 0, (uint8_t *) "tm isnt synced", 16);

    initialise_wifi();
    while (!isConnect) {
        vTaskDelay(100 / portTICK_RATE_MS);
    }

    esp_wait_sntp_sync();
    //TODO:Audio is not available.
    MY_AUDIO_Init();
    Audio_Play();
}

