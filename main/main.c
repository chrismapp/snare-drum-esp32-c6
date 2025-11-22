/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "ST7789.h"
#include "SD_SPI.h"
#include "RGB.h"
#include "Wireless.h"
#include "LVGL_Example.h"
#include <inttypes.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#include <encoder.h>
#include <esp_idf_lib_helpers.h>
#include <esp_log.h>


#define RE_A_GPIO   2
#define RE_B_GPIO   1
#define RE_BTN_GPIO 0

#define EV_QUEUE_LEN 5

static const char *TAG = "encoder_example";

static QueueHandle_t event_queue;
static rotary_encoder_t re;

static void app_task(void *arg)
{
    // Create event queue for rotary encoders
    event_queue = xQueueCreate(EV_QUEUE_LEN, sizeof(rotary_encoder_event_t));

    // Setup rotary encoder library
    ESP_ERROR_CHECK(rotary_encoder_init(event_queue));

    // Add one encoder
    memset(&re, 0, sizeof(rotary_encoder_t));
    re.pin_a = RE_A_GPIO;
    re.pin_b = RE_B_GPIO;
    re.pin_btn = RE_BTN_GPIO;
    ESP_ERROR_CHECK(rotary_encoder_add(&re));

    rotary_encoder_event_t e;
    int32_t val = 0;

    ESP_LOGI(TAG, "Initial value: %" PRIi32, val);
    while (1)
    {
        // raise the task priority of LVGL and/or reduce the handler period can improve the performance
        vTaskDelay(pdMS_TO_TICKS(10));
        // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
        lv_timer_handler();
        
        if( pdTRUE == xQueueReceive(event_queue, &e, 0))
        {
            switch (e.type)
            {
                case RE_ET_BTN_PRESSED:
                    ESP_LOGI(TAG, "Button pressed");
                    break;
                case RE_ET_BTN_RELEASED:
                    ESP_LOGI(TAG, "Button released");
                    break;
                case RE_ET_BTN_CLICKED:
                    ESP_LOGI(TAG, "Button clicked");
                    rotary_encoder_enable_acceleration(&re, 100);
                    ESP_LOGI(TAG, "Acceleration enabled");
                    break;
                case RE_ET_BTN_LONG_PRESSED:
                    ESP_LOGI(TAG, "Looooong pressed button");
                    rotary_encoder_disable_acceleration(&re);
                    ESP_LOGI(TAG, "Acceleration disabled");
                    break;
                case RE_ET_CHANGED:
                    val += e.diff;
                    ESP_LOGI(TAG, "Value = %" PRIi32, val);
                    break;
                default:
                    break;
            }
        }
    }
}

void app_main(void)
{
    Wireless_Init();
    Flash_Searching();
    RGB_Init();
    RGB_Example();
    SD_Init();
    LCD_Init();
    BK_Light(50);
    LVGL_Init();

    Lvgl_Example1();

    xTaskCreate(app_task, TAG, configMINIMAL_STACK_SIZE * 8, NULL, 5, NULL);
}
