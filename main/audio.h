//
// Created by yekai on 2021/8/10.
//

#ifndef SMART_CONFIG_AUDIO_H
#define SMART_CONFIG_AUDIO_H

#include "main.h"

void MY_AUDIO_Init();

void Audio_Play();

void Audio_Stop();

void Audio_SelectStation(uint16_t new_stationID);

extern uint8_t stationID;
extern const uint16_t stationIDList[8];

#endif //SMART_CONFIG_AUDIO_H
