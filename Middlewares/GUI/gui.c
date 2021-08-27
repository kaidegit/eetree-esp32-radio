//
// Created by yekai on 2021/8/23.
//

#include "gui.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include "smartconfig.h"
#include "time_calibrate.h"
#include "oled.h"
#include "gpio.h"
#include "fm.h"
#include <time.h>
#include <audio.h>

bool isFM = true;

uint8_t lastState = -1;

void GUI_Init() {
    xTaskCreate(GUI_Task, "GUI_Task", 4096, NULL, 3, NULL);
}

void GUI_Task() {
    while (1) {
        if (!isConnect) {
            OLED_ShowNotConnect();
            lastState = 0;
        } else {
            if (!isTimeCorrect) {
                OLED_ShowTimeCalibrating();
                lastState = 1;
            } else {
                OLED_ShowTime();
                if (isFM) {
                    OLED_ShowFM();
                    lastState = 2;
                } else {
                    OLED_ShowInternetFM();
                    lastState = 3;
                }
            }
        }
        vTaskDelay(100 / portTICK_RATE_MS);
    }

}

void OLED_ShowNotConnect() {
    if (lastState != 0) {
        OLED_Clear();
        OLED_ShowString(0, 0, (uint8_t *) "Wifi is not connected. Please Connect Wifi with EspTouch.", 16);
    }
}

void OLED_ShowTimeCalibrating() {
    if (lastState != 1) {
        OLED_Clear();
        OLED_ShowString(0, 0, (uint8_t *) "Wifi is connected. Trying to calibrate time...", 16);
    }
}

uint32_t freq_temp = -1;
void OLED_ShowFM() {
    static uint8_t rssi_temp = -1;
    char ch[30];
    RDA_ReadAllInfo();
    if (lastState != 2) {
        OLED_Clear();
        SetMuxFM();
        OLED_ShowString(0, 0, (uint8_t *) "FM", 16);

        sprintf(ch, "freq:%6d", RDA5807.freq);
        OLED_ShowString(0, 2, (uint8_t *) ch, 16);
        freq_temp = RDA5807.freq;

        sprintf(ch, "rssi: %2d/64", RDA5807.radioInfo.rssi + 1);
        OLED_ShowString(0, 4, (uint8_t *) ch, 16);
        rssi_temp = RDA5807.radioInfo.rssi;

        OLED_ShowString(0, 6, (uint8_t *) "SW  Pre Nex Mute", 16);
    } else {
        if (freq_temp != RDA5807.freq) {
            sprintf(ch, "freq:%6d", freq_temp);
            OLED_ShowString(0, 2, (uint8_t *) ch, 16);
            RDA_SetFrequency(freq_temp);
        }
        if (rssi_temp != RDA5807.radioInfo.rssi) {
            sprintf(ch, "rssi: %2d/64", RDA5807.radioInfo.rssi + 1);
            OLED_ShowString(0, 4, (uint8_t *) ch, 16);
            rssi_temp = RDA5807.radioInfo.rssi;
        }
    }
}

void OLED_ShowInternetFM() {
    static uint8_t stationID_temp = -1;
    char ch[30];
    if (lastState != 3) {
        OLED_Clear();
        SetMuxESP();
        OLED_ShowString(0, 0, (uint8_t *) "OL", 16);

        OLED_ShowString(0, 2, (uint8_t *) "IP:115.231.142.5", 16);

        sprintf(ch, "FM%4d", ConvStaIDToName(stationIDList[stationID]));
        OLED_ShowString(0, 4, (uint8_t *) ch, 16);
        stationID_temp = stationID;

        OLED_ShowQingtingLogo(6 * 8, 4);

        OLED_ShowString(0, 6, (uint8_t *) "SW  Pre Nex Mute", 16);
    } else {
        if (stationID_temp != stationID) {
            Audio_SelectStation(stationID);
            sprintf(ch, "FM%4d", ConvStaIDToName(stationIDList[stationID]));
            OLED_ShowString(0, 4, (uint8_t *) ch, 16);
            stationID_temp = stationID;
        }
    }
}

void OLED_ShowTime() {
    static uint8_t i = 0;
    char ch[30];
    time_t now = 0;
    struct tm timeinfo = {0};
    i++;
    time(&now);
    localtime_r(&now, &timeinfo);

    if (i > 50) {
        sprintf(ch, "%2dy%2dm%2dd",
                timeinfo.tm_year % 100, timeinfo.tm_mon + 1, timeinfo.tm_mday);
        if (i == 70) {
            i = 0;
        }
    } else {
        sprintf(ch, "%2d:%2d:%2d ",
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    }

    OLED_ShowString(56, 0, (uint8_t *) ch, 16);
}

uint16_t ConvStaIDToName(uint16_t _stationID) {
    switch (_stationID) {
        case 4522:
            return 930;      //FM93交通之声
        case 4518:
            return 880;      //浙江之声
        case 1133:
            return 918;     //杭州交通91.8电台
        case 1163:
            return 1054;    //西湖之声
        case 4521:
            return 996;     //浙江FM99.6
        case 1135:
            return 922;     //嘉兴交通广播
        case 4866:
            return 968;     //浙江音乐调频
        case 4523:
            return 1070;    //浙江城市之声
        default:
            return -1;
    }
}

void OLED_ShowQingtingLogo(uint8_t x, uint8_t y) {
    const uint8_t logo[][16] = {
            {0x00, 0x08, 0x08, 0x28, 0xC8, 0x08, 0x08, 0xFF, 0x08, 0x08, 0x88, 0x68, 0x08, 0x08, 0x00, 0x00},
            {0x21, 0x21, 0x11, 0x11, 0x09, 0x05, 0x03, 0xFF, 0x03, 0x05, 0x09, 0x11, 0x11, 0x21, 0x21, 0x00},/*"来",0*/

            {0x00, 0x00, 0x00, 0xF8, 0x88, 0x8C, 0x8A, 0x89, 0x88, 0x88, 0x88, 0xF8, 0x00, 0x00, 0x00, 0x00},
            {0x00, 0x00, 0x00, 0xFF, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0x44, 0xFF, 0x00, 0x00, 0x00, 0x00},/*"自",1*/

            {0x00, 0xF8, 0x08, 0xFF, 0x08, 0xF8, 0x44, 0x54, 0x54, 0x54, 0x7F, 0x54, 0x54, 0x54, 0x44, 0x00},
            {0x20, 0x63, 0x21, 0x1F, 0x11, 0x39, 0x00, 0xFF, 0x15, 0x15, 0x15, 0x55, 0x95, 0x7F, 0x00, 0x00},/*"蜻",2*/

            {0xF8, 0x08, 0xFF, 0x08, 0xF8, 0x00, 0x84, 0xE4, 0x9C, 0x00, 0x44, 0x44, 0xFC, 0x42, 0x42, 0x00},
            {0x23, 0x61, 0x3F, 0x11, 0xB9, 0x40, 0x26, 0x18, 0x27, 0x40, 0x48, 0x48, 0x4F, 0x48, 0x48, 0x00},/*"蜓",3*/
    };

    for (uint8_t j = 0; j < 2; j++) {
        OLED_Set_Pos(x + j * 16, y);
        for (uint8_t i = 0; i < 16; i++) {
            OLED_WR_DATA(logo[2 * j][i]);
        }
        OLED_Set_Pos(x + j * 16, y + 1);
        for (uint8_t i = 0; i < 16; i++) {
            OLED_WR_DATA(logo[2 * j + 1][i]);
        }
    }
//    OLED_ShowChar(x + 2 * 16, y, ' ', 16);
    for (uint8_t j = 2; j < 4; j++) {
        OLED_Set_Pos(x + j * 16, y);
        for (uint8_t i = 0; i < 16; i++) {
            OLED_WR_DATA(logo[2 * j][i]);
        }
        OLED_Set_Pos(x + j * 16, y + 1);
        for (uint8_t i = 0; i < 16; i++) {
            OLED_WR_DATA(logo[2 * j + 1][i]);
        }
    }
    OLED_ShowChar(x + 4 * 16, y, 'F', 16);
    OLED_ShowChar(x + 4 * 16 + 8, y, 'M', 16);
}