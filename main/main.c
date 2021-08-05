//
// Created by yekai on 2021/8/5.
//

#include <esp_err.h>
#include <nvs_flash.h>
#include "smartconfig.h"
#include "oled.h"
#include "gpio.h"
#include "spi.h"

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    MY_GPIO_Init();
    MY_SPI_Init();
    OLED_Init();
    OLED_Clear();
    char ch[] = "HelloWorld";
    OLED_ShowString(0, 0, (uint8_t *) ch, 16);
    initialise_wifi();
}

