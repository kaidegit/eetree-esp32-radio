//
// Created by yekai on 2021/8/5.
//

#include "spi.h"
#include "driver/spi_master.h"
#include "main.h"
#include "oled.h"

void MY_SPI_Init() {
    spi_bus_config_t buscfg={
            .miso_io_num=-1,
            .mosi_io_num=OLED_MOSI_Pin,
            .sclk_io_num=OLED_CLK_Pin,
            .quadwp_io_num=-1,
            .quadhd_io_num=-1,
            .max_transfer_sz=0
    };
    spi_device_interface_config_t devcfg={
            .clock_speed_hz=1*1000*1000,            //Clock out at 1 MHz
            .mode=0,                                //SPI mode 0
            .spics_io_num=-1,                       //CS pin
            .queue_size=7,                          //We want to be able to queue 7 transactions at a time
    };
    ESP_ERROR_CHECK( spi_bus_initialize(OLED_SPI_HOST, &buscfg, SPI_DMA_DISABLED));
    ESP_ERROR_CHECK(spi_bus_add_device(OLED_SPI_HOST,&devcfg,&oled_spi_handle));
}
