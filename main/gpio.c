//
// Created by yekai on 2021/8/5.
//

#include "gpio.h"
#include "driver/gpio.h"
#include "main.h"
#include "oled.h"

static void IRAM_ATTR gpio_isr_handler(void *arg) {
//    printf("Key\r\n");
    OLED_ShowString(0, 3, (uint8_t *) "key", 16);
    uint32_t gpio_num = (uint32_t) arg;
    if (gpio_num == Key1_Pin) {
//        printf("Key1\r\n");
        OLED_ShowString(50, 3, (uint8_t *) "1", 16);
    }
}

void MY_GPIO_Init() {
    uint64_t gpio_mask;
    gpio_config_t io_conf;

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
}

