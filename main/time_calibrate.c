//
// Created by yekai on 2021/8/9.
//
#include <stdio.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/apps/sntp.h"
#include "esp_log.h"
#include "oled.h"
#include "time_calibrate.h"

bool isTimeCorrect = false;

static const char *TAG = "sntp";

static void esp_initialize_sntp(void) {
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "ntp.tuna.tsinghua.edu.cn");
    sntp_setservername(1, "ntp1.aliyun.com");
    sntp_setservername(2, "210.72.145.44");
    sntp_init();
}

void esp_wait_sntp_sync(void) {
    char strftime_buf[64];
    esp_initialize_sntp();

    // wait for time to be set
    time_t now = 0;
    int retry = 0;
    struct tm timeinfo = {0};

    while (timeinfo.tm_year < (2019 - 1900)) {
        ESP_LOGD(TAG, "Waiting for system time to be set... (%d)", ++retry);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        time(&now);
        localtime_r(&now, &timeinfo);
    }

    // set timezone to China Standard Time
//    setenv("TZ", "UTC+8", 1);
//    timeinfo.tm_hour += 8;
    setenv("TZ", "CST-8", 1);
    tzset();

    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time in Shanghai is: %s", strftime_buf);
//    xTaskCreate(show_time, "show_time", 4096, NULL, 3, NULL);
    isTimeCorrect = true;

    sntp_stop();
}

//————————————————
//版权声明：本文为CSDN博主「乐鑫科技 Espressif」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
//原文链接：https://blog.csdn.net/espressif/article/details/103001337

//void show_time(void *pvParameters) {
//    char ch[30];
//    while (true){
//        time_t now = 0;
//        struct tm timeinfo = {0};
//        time(&now);
//        localtime_r(&now, &timeinfo);
//        timeinfo.tm_hour += 8;
//        sprintf(ch, "time:%2d:%2d:%2d ", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
//        OLED_ShowString(0, 0, (uint8_t *) ch, 16);
//        vTaskDelay(1000 / portTICK_RATE_MS);
//    }
//}