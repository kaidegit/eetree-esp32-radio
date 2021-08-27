//
// Created by yekai on 2021/8/16.
//

#include <driver/i2c.h>
#include <oled.h>
#include "fm.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"

#define FM907_Freq 90700

struct RDA_Handler RDA5807;

RADIO_FREQ FMStationList[50] = {0};
uint16_t FMStationNum = 0;

void RDA_ReadAllInfo() {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0x21, 1);
    // from 0x0A high to 0x0C low
    uint8_t i2c_data_array[6];
    // read 0x0A
    i2c_master_read_byte(cmd, i2c_data_array + 0, 0);
    i2c_master_read_byte(cmd, i2c_data_array + 1, 0);
    // read 0x0B
    i2c_master_read_byte(cmd, i2c_data_array + 2, 0);
    i2c_master_read_byte(cmd, i2c_data_array + 3, 0);
    // read 0x0C
    i2c_master_read_byte(cmd, i2c_data_array + 4, 0);
    i2c_master_read_byte(cmd, i2c_data_array + 5, 1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(FM_I2C_HOST, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    RDA5807.radioInfo.rds = i2c_data_array[0] & 0x80;
    uint16_t readChan = ((i2c_data_array[0] & 0x03) << 8) + i2c_data_array[1];
    switch (RDA5807.band) {
        case RADIO_BAND_US:
            RDA5807.freq = RDA5807.channelSpacing * readChan + 87 * 1000;
            break;
        case RADIO_BAND_JP:
        case RADIO_BAND_WW:
            RDA5807.freq = RDA5807.channelSpacing * readChan + 76 * 1000;
            break;
        case RADIO_BAND_EE:
            RDA5807.freq = RDA5807.channelSpacing * readChan + 65 * 1000;
    }
    RDA5807.radioInfo.rssi = i2c_data_array[2] >> 2;
}

void RDA_WriteAllInfo() {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, 0x20, 1);
    for (uint8_t i = 2; i < 8; i++) {
        i2c_master_write_byte(cmd, RDA5807.regList[i] >> 8, 1);
        i2c_master_write_byte(cmd, RDA5807.regList[i] & 0xff, 1);
    }
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(FM_I2C_HOST, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
}




void RDA_Init() {
    const uint8_t RSSI_MIN = 20;
    RDA5807.channelSpacing = 100;
    // Reg 0x00 and 0x01 are not used
    RDA5807.regList[0] = 0;
    RDA5807.regList[1] = 0;
    RDA5807.regList[2] = 0b11000000 << 8 | 0b00000011;
    RDA5807.regList[3] = 0b00000000 << 8 | 0b00000000;
    RDA5807.regList[4] = 0b00001000 << 8 | 0b00000000;
    RDA5807.regList[5] = 0b10000100 << 8 | 0b11010100;
    RDA5807.regList[6] = 0b00000000 << 8 | 0b00000000;
    RDA5807.regList[7] = 0b00000000 << 8 | 0b00000000;

    RDA_WriteAllInfo();
    RDA_Reset();

//    memset(FMStationList, -1, sizeof(FMStationList));
//    FMStationNum = 0;
//    for (RADIO_FREQ i = 87000; i <= 108000; i += RDA5807.channelSpacing) {
//        RDA_SetFrequency(i);
//        RDA_ReadAllInfo();
//        if (RDA5807.radioInfo.rssi >= RSSI_MIN) {
//            FMStationList[FMStationNum] = i;
//            FMStationNum++;
//        }
//    }
//    printf("%dStation found\r\n", FMStationNum);
//    RDA_SetFrequency(FMStationList[0]);

//    xTaskCreate(show_fm, "show_fm", 4096, NULL, 3, NULL);
}

void RDA_Reset() {
    RDA5807.regList[RADIO_REG_CTRL] |= RADIO_REG_CTRL_RESET;
    RDA_WriteAllInfo();
    vTaskDelay(100 / portTICK_RATE_MS);
    RDA5807.regList[RADIO_REG_CTRL] &= !RADIO_REG_CTRL_RESET;
    RDA_WriteAllInfo();
}

void RDA_SetBandFrequency(enum RADIO_BAND newBand, RADIO_FREQ newFreq) {
    RDA_SetBand(newBand);
    RDA_SetFrequency(newFreq);
}

void RDA_SetFrequency(RADIO_FREQ newFreq) {
    RADIO_FREQ freqHigh, freqLow;
    switch (RDA5807.band) {
        case RADIO_BAND_US:
            freqLow = 87000;
            freqHigh = 108000;
            break;
        case RADIO_BAND_JP:
            freqLow = 76000;
            freqHigh = 91000;
            break;
        case RADIO_BAND_WW:
            freqLow = 76000;
            freqHigh = 108000;
            break;
        case RADIO_BAND_EE:
            freqLow = 65000;
            freqHigh = 76000;
            break;
        default:
            freqLow = 65000;
            freqHigh = 108000;
            break;
    }
    if (newFreq < freqLow) newFreq = freqLow;
    if (newFreq > freqHigh) newFreq = freqHigh;
    uint16_t regChannel = RDA5807.regList[RADIO_REG_CHAN] &
                          (RADIO_REG_CHAN_SPACE | RADIO_REG_CHAN_BAND);
    uint16_t newChannel = (newFreq - freqLow) / RDA5807.channelSpacing;
    regChannel += RADIO_REG_CHAN_TUNE; // enable tuning
    regChannel |= newChannel << 6;
    RDA5807.regList[RADIO_REG_CTRL] |=
            RADIO_REG_CTRL_OUTPUT | RADIO_REG_CTRL_UNMUTE |
            RADIO_REG_CTRL_RDS | RADIO_REG_CTRL_ENABLE; //  | RADIO_REG_CTRL_NEW
    RDA5807.regList[RADIO_REG_CHAN] = regChannel;
    RDA_WriteAllInfo();
}

void RDA_SetBand(enum RADIO_BAND newBand) {
    RDA5807.band = newBand;
    switch (newBand) {
        case RADIO_BAND_US:
            RDA5807.regList[RADIO_REG_CHAN] &= !RADIO_REG_CHAN_BAND;
            RDA5807.regList[RADIO_REG_CHAN] |= RADIO_REG_CHAN_BAND_US;
            break;
        case RADIO_BAND_JP:
            RDA5807.regList[RADIO_REG_CHAN] &= !RADIO_REG_CHAN_BAND;
            RDA5807.regList[RADIO_REG_CHAN] |= RADIO_REG_CHAN_BAND_JP;
            break;
        case RADIO_BAND_WW:
            RDA5807.regList[RADIO_REG_CHAN] &= !RADIO_REG_CHAN_BAND;
            RDA5807.regList[RADIO_REG_CHAN] |= RADIO_REG_CHAN_BAND_WW;
            break;
        case RADIO_BAND_EE:
            RDA5807.regList[RADIO_REG_CHAN] &= !RADIO_REG_CHAN_BAND;
            RDA5807.regList[RADIO_REG_CHAN] |= RADIO_REG_CHAN_BAND_EE;
            break;
        default:
            break;
    }
    RDA5807.regList[RADIO_REG_CHAN] |= RADIO_REG_CHAN_SPACE_100;
    RDA_WriteAllInfo();
}

void show_fm(void *pvParameters) {
    char ch[30];
    while (true) {
        RDA_ReadAllInfo();
        sprintf(ch, "%d %d", RDA5807.freq, RDA5807.radioInfo.rssi);
        OLED_ShowString(0, 6, (uint8_t *) ch, 16);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}


