#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "push_button.h"
#include "touch.h"

static const char TAG[] = "easy_input_common";

#if CONFIG_EASY_INPUT_PUSH_BUTTON_ENABLE
static TaskHandle_t h_push_button = NULL;
#endif
#if CONFIG_EASY_INPUT_TOUCH_ENABLE
static TaskHandle_t h_touch = NULL;
#endif

void easy_input_queue_init(QueueHandle_t *input_queue){
    *input_queue = xQueueCreate(5, sizeof(uint64_t));
}

void easy_input_run( QueueHandle_t *input_queue ) {
    /* Sets up Input buttons, polls them for digital filter, and when
     * a debounced input has been triggered, puts the resulting bitmask onto
     * the specified input_queue */

    /* Setup Inputs */
#if CONFIG_EASY_INPUT_PUSH_BUTTON_ENABLE
    ESP_LOGD(TAG, "Creating EasyInput PushButton Task");
    xTaskCreate(pb_task, \
            "PushButton", 1520,
            (void *)input_queue, 20, \
            &h_push_button);
#endif
#if CONFIG_EASY_INPUT_TOUCH_ENABLE
    ESP_LOGD(TAG, "Creating EasyInput Touch Task");
    xTaskCreate(touch_task, \
            "Touch", 2048,
            (void *)input_queue, 20, \
            &h_touch);
#endif
}

void easy_input_stop() {
#if CONFIG_EASY_INPUT_PUSH_BUTTON_ENABLE
    if( NULL != h_push_button ) {
        vTaskDelete(h_push_button);
        h_push_button = NULL;
    }
#endif
#if CONFIG_EASY_INPUT_TOUCH_ENABLE
    if( NULL != h_touch ) {
        vTaskDelete(h_touch);
        h_touch = NULL;
    }
#endif
}
