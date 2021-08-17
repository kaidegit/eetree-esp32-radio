//
// Created by yekai on 2021/8/5.
//

#ifndef SMART_CONFIG_SMARTCONFIG_H
#define SMART_CONFIG_SMARTCONFIG_H

#include <esp_wifi_types.h>

void initialise_wifi();

wifi_config_t wifi_config;

bool isConnect;

#endif //SMART_CONFIG_SMARTCONFIG_H
