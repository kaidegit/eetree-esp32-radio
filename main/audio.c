//
// Created by yekai on 2021/8/10.
//


#include <audio_common.h>
#include <audio_element.h>
#include <audio_pipeline.h>
#include <esp_log.h>
#include <esp_peripherals.h>
#include <mp3_decoder.h>
#include <string.h>
#include "audio.h"
#include "main.h"
#include <pwm_stream.h>

audio_pipeline_handle_t pipeline;
audio_element_handle_t mp3_decoder, output_stream_writer;

extern const uint8_t adf_music_mp3_start[] asm("_binary_adf_music_mp3_start");
extern const uint8_t adf_music_mp3_end[] asm("_binary_adf_music_mp3_end");


int mp3_music_read_cb(audio_element_handle_t el, char *buf, int len, TickType_t wait_time, void *ctx) {
    static int mp3_pos;
    int read_size = adf_music_mp3_end - adf_music_mp3_start - mp3_pos;
    if (read_size == 0) {
        return AEL_IO_DONE;
    } else if (len < read_size) {
        read_size = len;
    }
    memcpy(buf, adf_music_mp3_start + mp3_pos, read_size);
    mp3_pos += read_size;
    return read_size;
}

void MY_AUDIO_Init() {
//    ESP_LOGI(TAG, "[ 1 ] Periph init");
    esp_periph_config_t periph_cfg = {
            .task_stack         = DEFAULT_ESP_PERIPH_STACK_SIZE,
            .task_prio          = DEFAULT_ESP_PERIPH_TASK_PRIO,
            .task_core          = DEFAULT_ESP_PERIPH_TASK_CORE,
            .extern_stack       = false,
    };
    esp_periph_set_handle_t set = esp_periph_set_init(&periph_cfg);

//    ESP_LOGI(TAG, "[ 2 ] Create audio pipeline for playback");
    audio_pipeline_cfg_t pipeline_cfg = {
            .rb_size            = DEFAULT_PIPELINE_RINGBUF_SIZE,
    };
    pipeline = audio_pipeline_init(&pipeline_cfg);
    mem_assert(pipeline);

//    ESP_LOGI(TAG, "[2.1] Create output stream to write data to codec chip");
    pwm_stream_cfg_t pwm_cfg = {
            .type = AUDIO_STREAM_WRITER,
            .pwm_config = {
                    .tg_num = TIMER_GROUP_0,
                    .timer_num = TIMER_0,
                    .gpio_num_left = AUDIO_Pin,
                    .gpio_num_right = -1,
                    .ledc_channel_left = LEDC_CHANNEL_0,
                    .ledc_channel_right = -1,
                    .ledc_timer_sel = LEDC_TIMER_0,
                    .duty_resolution = LEDC_TIMER_8_BIT,
                    .data_len = PWM_CONFIG_RINGBUFFER_SIZE,
            },
            .out_rb_size = PWM_STREAM_RINGBUFFER_SIZE,
            .task_stack = PWM_STREAM_TASK_STACK,
            .task_core = PWM_STREAM_TASK_CORE,
            .task_prio = PWM_STREAM_TASK_PRIO,
            .buffer_len =  PWM_STREAM_BUF_SIZE,
            .ext_stack = false,
    };
    output_stream_writer = pwm_stream_init(&pwm_cfg);

//    ESP_LOGI(TAG, "[2.2] Create wav decoder to decode wav file");
    mp3_decoder_cfg_t mp3_cfg = {
            .out_rb_size        = MP3_DECODER_RINGBUFFER_SIZE,
            .task_stack         = MP3_DECODER_TASK_STACK_SIZE,
            .task_core          = MP3_DECODER_TASK_CORE,
            .task_prio          = MP3_DECODER_TASK_PRIO,
            .stack_in_ext       = true,
    };
    mp3_decoder = mp3_decoder_init(&mp3_cfg);
    audio_element_set_read_cb(mp3_decoder, mp3_music_read_cb, NULL);

//    ESP_LOGI(TAG, "[2.3] Register all elements to audio pipeline");
    audio_pipeline_register(pipeline, mp3_decoder, "mp3");
    audio_pipeline_register(pipeline, output_stream_writer, "output");

//    ESP_LOGI(TAG, "[2.4] Link it together [mp3_music_read_cb]-->mp3_decoder-->output_stream-->[pa_chip]");
    const char *link_tag[2] = {"mp3", "output"};
    audio_pipeline_link(pipeline, &link_tag[0], 2);

//    ESP_LOGI(TAG, "[ 3 ] Set up  event listener");
    audio_event_iface_cfg_t evt_cfg = {
            .internal_queue_size = DEFAULT_AUDIO_EVENT_IFACE_SIZE,
            .external_queue_size = DEFAULT_AUDIO_EVENT_IFACE_SIZE,
            .queue_set_size = DEFAULT_AUDIO_EVENT_IFACE_SIZE,
            .on_cmd = NULL,
            .context = NULL,
            .wait_time = portMAX_DELAY,
            .type = 0,
    };
    audio_event_iface_handle_t evt = audio_event_iface_init(&evt_cfg);

//    ESP_LOGI(TAG, "[3.1] Listening event from all elements of pipeline");
    audio_pipeline_set_listener(pipeline, evt);

//    ESP_LOGI(TAG, "[3.2] Listening event from peripherals");
    audio_event_iface_set_listener(esp_periph_set_get_event_iface(set), evt);

//    ESP_LOGI(TAG, "[ 4 ] Start audio_pipeline");
    audio_pipeline_run(pipeline);
}
