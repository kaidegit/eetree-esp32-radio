//
// Created by yekai on 2021/8/23.
//

#ifndef SMART_CONFIG_GUI_H
#define SMART_CONFIG_GUI_H

#include "main.h"

void GUI_Init();

void GUI_Task();

void OLED_ShowNotConnect();

void OLED_ShowTimeCalibrating();

void OLED_ShowFM();

void OLED_ShowInternetFM();

void OLED_ShowTime();

void OLED_ShowQingtingLogo(uint8_t x, uint8_t y);

extern bool isFM;

#endif //SMART_CONFIG_GUI_H
