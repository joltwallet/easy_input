#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "push_button.h"
#include "touch.h"

static const char TAG[] = "easy_input_common";

void easy_input_queue_init(QueueHandle_t *input_queue){
    *input_queue = xQueueCreate(5, sizeof(uint64_t));
}

void easy_input_task( void *input_queue ){
    /* Sets up Input buttons, polls them for digital filter, and when
     * a debounced input has been triggered, puts the resulting bitmask onto
     * the specified input_queue */

    /* Setup Inputs */
#if CONFIG_EASY_INPUT_PUSH_BUTTON_ENABLE
    pb_setup();
#endif
#if CONFIG_EASY_INPUT_TOUCH_ENABLE
    touch_setup();
#endif

    for(uint64_t triggered_buttons=0;;
            vTaskDelay(pdMS_TO_TICKS(
            CONFIG_EASY_INPUT_PUSH_BUTTON_POLLING_PERIOD_MS))) {
        triggered_buttons = 0;

#if CONFIG_EASY_INPUT_PUSH_BUTTON_ENABLE
        triggered_buttons |= pb_trigger();
#endif
#if CONFIG_EASY_INPUT_TOUCH_ENABLE
        triggered_buttons |= touch_trigger();
#endif

        // If a button is triggered, send it off to the queue
        if(triggered_buttons){
            ESP_LOGD(TAG, "button triggered");
            xQueueSend(*(QueueHandle_t *)input_queue, &triggered_buttons, 0);
        }
    }
    vTaskDelete(NULL); // Should never reach here!
}

