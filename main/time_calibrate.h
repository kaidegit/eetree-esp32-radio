//
// Created by yekai on 2021/8/9.
//

#ifndef SMART_CONFIG_TIME_CALIBRATE_H
#define SMART_CONFIG_TIME_CALIBRATE_H

#include "main.h"

void esp_wait_sntp_sync(void);

extern struct tm timeinfo;

#endif //SMART_CONFIG_TIME_CALIBRATE_H
