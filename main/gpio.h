//
// Created by yekai on 2021/8/5.
//

#ifndef SMART_CONFIG_GPIO_H
#define SMART_CONFIG_GPIO_H

void MY_GPIO_Init();

void SetMuxFM();

void SetMuxESP();

void EnableSpeaker();

void DisableSpeaker();

void ToggleSpeaker();

//static void gpio_isr_handler(void *arg);

#endif //SMART_CONFIG_GPIO_H
