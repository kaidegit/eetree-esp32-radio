//
// Created by yekai on 2021/8/5.
//

#include "audio.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include "gpio.h"
#include "driver/gpio.h"
#include "main.h"
#include "oled.h"
#include "gui.h"
#include "fm.h"

bool isSpeakerOn = false;

static void gpio_isr_handler(void *arg) {
    uint32_t gpio_num = (uint32_t) arg;
    switch (gpio_num) {
        case Key1_Pin:
            isFM = !isFM;
            break;
        case Key2_Pin:
            if (isFM) {
                freq_temp -= RDA5807.channelSpacing;
            } else {
                if (stationID == 0) {
                    stationID = 7;
                } else {
                    stationID--;
                }
            }
            break;
        case Key3_Pin:
            if (isFM) {
                freq_temp += RDA5807.channelSpacing;
            } else {
                if (stationID == 7) {
                    stationID = 0;
                } else {
                    stationID++;
                }
            }
            break;
        case Key4_Pin:
            ToggleSpeaker();
            break;
        default:
            break;
    }
}

void SetMuxFM() {
    gpio_set_level(MUX_Pin, 0);
}

void SetMuxESP() {
    gpio_set_level(MUX_Pin, 1);
}

void EnableSpeaker() {
    isSpeakerOn = true;
    gpio_set_level(SPEAKER_EN_Pin, 1);
}

void DisableSpeaker() {
    isSpeakerOn = false;
    gpio_set_level(SPEAKER_EN_Pin, 0);
}

void ToggleSpeaker() {
    if (isSpeakerOn) {
        DisableSpeaker();
    } else {
        EnableSpeaker();
    }
}

void MY_GPIO_Init() {
    uint64_t gpio_mask;
    gpio_config_t io_conf;

    gpio_mask = (1ULL << SPEAKER_EN_Pin) |
                (1ULL << MUX_Pin);
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;
    io_conf.pin_bit_mask = gpio_mask;
    gpio_config(&io_conf);

    gpio_mask = (1ULL << OLED_DC_Pin) |
                (1ULL << OLED_RST_Pin);
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    io_conf.pin_bit_mask = gpio_mask;
    gpio_config(&io_conf);

    gpio_mask = (1ULL << Key1_Pin) |
                (1ULL << Key2_Pin) |
                (1ULL << Key3_Pin) |
                (1ULL << Key4_Pin);
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    io_conf.pin_bit_mask = gpio_mask;
    gpio_config(&io_conf);
    gpio_set_intr_type(Key1_Pin, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(Key2_Pin, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(Key3_Pin, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(Key4_Pin, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);
    gpio_isr_handler_add(Key1_Pin, gpio_isr_handler, (void *) Key1_Pin);
    gpio_isr_handler_add(Key2_Pin, gpio_isr_handler, (void *) Key2_Pin);
    gpio_isr_handler_add(Key3_Pin, gpio_isr_handler, (void *) Key3_Pin);
    gpio_isr_handler_add(Key4_Pin, gpio_isr_handler, (void *) Key4_Pin);
}

