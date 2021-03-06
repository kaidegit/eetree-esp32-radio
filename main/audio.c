#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "audio_element.h"
#include "audio_pipeline.h"
#include "audio_event_iface.h"
#include "audio_common.h"
#include "http_stream.h"
#include "i2s_stream.h"
#include "aac_decoder.h"
#include <pwm_stream.h>

#include "esp_peripherals.h"
#include "periph_wifi.h"
#include "board.h"
#include "esp_netif.h"
#include "smartconfig.h"
#include "audio.h"
#include "main.h"


static const char *TAG = "HTTP_LIVINGSTREAM_EXAMPLE";

audio_event_iface_handle_t evt;
esp_periph_set_handle_t set;
audio_pipeline_handle_t pipeline;
audio_element_handle_t http_stream_reader, output_stream_writer, aac_decoder;

const uint16_t stationIDList[8] = {
        4522, 4518, 1133, 1163, 4521, 1135, 4866, 4523
};
uint8_t stationID = 0;


int _http_stream_event_handle(http_stream_event_msg_t *msg) {
    if (msg->event_id == HTTP_STREAM_RESOLVE_ALL_TRACKS) {
        return ESP_OK;
    }

    if (msg->event_id == HTTP_STREAM_FINISH_TRACK) {
        return http_stream_next_track(msg->el);
    }
    if (msg->event_id == HTTP_STREAM_FINISH_PLAYLIST) {
        return http_stream_fetch_again(msg->el);
    }
    return ESP_OK;
}

void MY_AUDIO_Init() {
    esp_log_level_set("*", ESP_LOG_INFO);
    esp_log_level_set(TAG, ESP_LOG_DEBUG);

//    ESP_LOGI(TAG, "[ 1 ] Start audio codec chip");
//    audio_board_handle_t board_handle = audio_board_init();
//    audio_hal_ctrl_codec(board_handle->audio_hal, AUDIO_HAL_CODEC_MODE_DECODE, AUDIO_HAL_CTRL_START);
    ESP_LOGI(TAG, "[ 1 ] Periph init");


    ESP_LOGI(TAG, "[2.0] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = DEFAULT_AUDIO_PIPELINE_CONFIG();
    pipeline = audio_pipeline_init(&pipeline_cfg);

    ESP_LOGI(TAG, "[2.1] Create http stream to read data");
    http_stream_cfg_t http_cfg = HTTP_STREAM_CFG_DEFAULT();
    http_cfg.event_handle = _http_stream_event_handle;
    http_cfg.type = AUDIO_STREAM_READER;
    http_cfg.enable_playlist_parser = true;
    http_stream_reader = http_stream_init(&http_cfg);

    ESP_LOGI(TAG, "[2.2] Create pwm stream to write data to codec chip");
    pwm_stream_cfg_t pwm_cfg = PWM_STREAM_CFG_DEFAULT();
    pwm_cfg.pwm_config.gpio_num_left = AUDIO_Pin;
    pwm_cfg.pwm_config.gpio_num_right = AUDIO_Pin + 1;
    output_stream_writer = pwm_stream_init(&pwm_cfg);

    ESP_LOGI(TAG, "[2.3] Create aac decoder to decode aac file");
    aac_decoder_cfg_t aac_cfg = {
            .out_rb_size=(2 * 1024),
            .task_stack=(4 * 1024),
            .task_core=(0),
            .task_prio=(5),
            .stack_in_ext=1,
    };
    aac_decoder = aac_decoder_init(&aac_cfg);

    ESP_LOGI(TAG, "[2.4] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, http_stream_reader, "http");
    audio_pipeline_register(pipeline, aac_decoder, "aac");
    audio_pipeline_register(pipeline, output_stream_writer, "pwm");

    ESP_LOGI(TAG, "[2.5] Link it together http_stream-->aac_decoder-->pwm_stream");
    const char *link_tag[3] = {"http", "aac", "pwm"};
    audio_pipeline_link(pipeline, &link_tag[0], 3);

    ESP_LOGI(TAG, "[2.6] Set up  uri (http as http_stream, aac as aac decoder, and default output is pwm)");
    char url[100];
    sprintf(url, "http://open.ls.qingting.fm/live/%d/64k.m3u8?format=aac", stationIDList[stationID]);
    audio_element_set_uri(http_stream_reader, url);

//    ESP_LOGI(TAG, "[ 3 ] Start and wait for Wi-Fi network");
//    esp_periph_config_t periph_cfg = DEFAULT_ESP_PERIPH_SET_CONFIG();
//    set = esp_periph_set_init(&periph_cfg);
//    periph_wifi_cfg_t wifi_cfg = {
//            .ssid = (const char *) wifi_config.sta.ssid,
//            .password = (const char *) wifi_config.sta.password,
//    };
//    esp_periph_handle_t wifi_handle = periph_wifi_init(&wifi_cfg);
//    esp_periph_start(set, wifi_handle);
//    periph_wifi_wait_for_connected(wifi_handle, portMAX_DELAY);

    ESP_LOGI(TAG, "[ 4 ] Set up  event listener");
    audio_event_iface_cfg_t evt_cfg = AUDIO_EVENT_IFACE_DEFAULT_CFG();
    evt = audio_event_iface_init(&evt_cfg);

    ESP_LOGI(TAG, "[4.1] Listening event from all elements of pipeline");
    audio_pipeline_set_listener(pipeline, evt);

    ESP_LOGI(TAG, "[4.2] Listening event from peripherals");
//    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);

    ESP_LOGI(TAG, "[ 5 ] Start audio_pipeline");
    audio_pipeline_run(pipeline);
}

void Audio_Play() {
    while (1) {
        audio_event_iface_msg_t msg;
        esp_err_t ret = audio_event_iface_listen(evt, &msg, portMAX_DELAY);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "[ * ] Event interface error : %d", ret);
            continue;
        }

        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT &&
            msg.source == (void *) aac_decoder &&
            msg.cmd == AEL_MSG_CMD_REPORT_MUSIC_INFO) {
            audio_element_info_t music_info = {0};
            audio_element_getinfo(aac_decoder, &music_info);

            ESP_LOGI(TAG, "[ * ] Receive music info from aac decoder, sample_rates=%d, bits=%d, ch=%d",
                     music_info.sample_rates, music_info.bits, music_info.channels);

            audio_element_setinfo(output_stream_writer, &music_info);
            pwm_stream_set_clk(output_stream_writer, music_info.sample_rates, music_info.bits, music_info.channels);
            continue;
        }

        /* restart stream when the first pipeline element (http_stream_reader in this case) receives stop event (caused by reading errors) */
        if (msg.source_type == AUDIO_ELEMENT_TYPE_ELEMENT && msg.source == (void *) http_stream_reader
            && msg.cmd == AEL_MSG_CMD_REPORT_STATUS && (int) msg.data == AEL_STATUS_ERROR_OPEN) {
            ESP_LOGW(TAG, "[ * ] Restart stream");
            audio_pipeline_stop(pipeline);
            audio_pipeline_wait_for_stop(pipeline);
            audio_element_reset_state(aac_decoder);
            audio_element_reset_state(output_stream_writer);
            audio_pipeline_reset_ringbuffer(pipeline);
            audio_pipeline_reset_items_state(pipeline);
            audio_pipeline_run(pipeline);
            continue;
        }
    }
}

void Audio_Stop() {
    ESP_LOGI(TAG, "[ 6 ] Stop audio_pipeline");
    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_pipeline_terminate(pipeline);

    audio_pipeline_unregister(pipeline, http_stream_reader);
    audio_pipeline_unregister(pipeline, output_stream_writer);
    audio_pipeline_unregister(pipeline, aac_decoder);

    /* Terminate the pipeline before removing the listener */
    audio_pipeline_remove_listener(pipeline);

    /* Stop all peripherals before removing the listener */
//    esp_periph_set_stop_all(set);
//    audio_event_iface_remove_listener(esp_periph_set_get_event_iface(set), evt);

    /* Make sure audio_pipeline_remove_listener & audio_event_iface_remove_listener are called before destroying event_iface */
    audio_event_iface_destroy(evt);

    /* Release all resources */
    audio_pipeline_deinit(pipeline);
    audio_element_deinit(http_stream_reader);
    audio_element_deinit(output_stream_writer);
    audio_element_deinit(aac_decoder);
//    esp_periph_set_destroy(set);
}

void Audio_SelectStation(uint16_t new_stationID) {
//    Audio_Stop();
    char url[100];
    sprintf(url, "http://open.ls.qingting.fm/live/%d/64k.m3u8?format=aac", stationIDList[new_stationID]);
    audio_pipeline_stop(pipeline);
    audio_pipeline_wait_for_stop(pipeline);
    audio_element_reset_state(aac_decoder);
    audio_element_reset_state(output_stream_writer);
    audio_pipeline_reset_ringbuffer(pipeline);
    audio_pipeline_reset_items_state(pipeline);
    audio_element_set_uri(http_stream_reader, url);
    audio_pipeline_run(pipeline);
//    Audio_Play();
}