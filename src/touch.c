#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "driver/touch_pad.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"

#include "easy_input.h"
#include "touch.h"
#if CONFIG_EASY_INPUT_TOUCH_ENABLE

#define TOUCH_THRESH_NO_USE   (0)
static const char TAG[] = "easy_input_touch";
static uint8_t s_pad_activated[TOUCH_PAD_MAX];
static uint32_t s_pad_init_val[TOUCH_PAD_MAX];
static uint8_t s_pad_counter[TOUCH_PAD_MAX] = { 0 };
static bool s_pad_state[TOUCH_PAD_MAX] = { false };

/*
  Read values sensed at all available touch pads.
  Use 2 / 3 of read value as the threshold
  to trigger interrupt when the pad is touched.
  Note: this routine demonstrates a simple way
  to configure activation threshold for the touch pads.
  Do not touch any pads when this routine
  is running (on application start).
 */
static void touch_set_thresholds(void) {
    uint16_t touch_value;
    for (int i = 0; i<TOUCH_PAD_MAX; i++) {
        //read filtered value
        touch_pad_read_filtered(i, &touch_value);
        s_pad_init_val[i] = touch_value;
        ESP_LOGI(TAG, "test init: touch pad [%d] val is %d", i, touch_value);
        float thresh = touch_value;
        thresh *= CONFIG_EASY_INPUT_TOUCH_THRESH_PERCENT;
        thresh /= 1000;
        //set interrupt threshold.
        ESP_ERROR_CHECK(touch_pad_set_thresh(i, (uint16_t)thresh));
    }
}

/*
  Handle an interrupt triggered when a pad is touched.
  Recognize what pad has been touched and save it in a table.
 */
static void touch_rtc_intr(void *arg) {
    uint32_t pad_intr = touch_pad_get_status();
    //clear interrupt
    touch_pad_clear_status();
    for (int i = 0; i < TOUCH_PAD_MAX; i++) {
        if ((pad_intr >> i) & 0x01 && s_pad_activated[i]<255) {
            s_pad_activated[i] += 1;
        }
    }
}

static void touch_setup() {
    ESP_LOGI(TAG, "Initializing touch pad; it will start a timer to run a filter");
    touch_pad_init();
    touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    // Set reference voltage for charging/discharging
    touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);

#if CONFIG_EASY_INPUT_TOUCH_UP_PIN != -1
    touch_pad_config(CONFIG_EASY_INPUT_TOUCH_UP_PIN, TOUCH_THRESH_NO_USE);
#endif
#if CONFIG_EASY_INPUT_TOUCH_DOWN_PIN != -1
    touch_pad_config(CONFIG_EASY_INPUT_TOUCH_DOWN_PIN, TOUCH_THRESH_NO_USE);
#endif
#if CONFIG_EASY_INPUT_TOUCH_LEFT_PIN != -1
    touch_pad_config(CONFIG_EASY_INPUT_TOUCH_LEFT_PIN, TOUCH_THRESH_NO_USE);
#endif
#if CONFIG_EASY_INPUT_TOUCH_RIGHT_PIN != -1
    touch_pad_config(CONFIG_EASY_INPUT_TOUCH_RIGHT_PIN, TOUCH_THRESH_NO_USE);
#endif
#if CONFIG_EASY_INPUT_TOUCH_BACK_PIN != -1
    touch_pad_config(CONFIG_EASY_INPUT_TOUCH_BACK_PIN, TOUCH_THRESH_NO_USE);
#endif
#if CONFIG_EASY_INPUT_TOUCH_ENTER_PIN != -1
    touch_pad_config(CONFIG_EASY_INPUT_TOUCH_ENTER_PIN, TOUCH_THRESH_NO_USE);
#endif

    // Initialize and start a software filter to detect slight change of capacitance.
    touch_pad_filter_start(CONFIG_EASY_INPUT_TOUCH_FILTER_PERIOD_MS);

    // Set threshold
    touch_set_thresholds();

    // Register touch interrupt ISR
    touch_pad_isr_register(touch_rtc_intr, NULL);
    touch_pad_intr_enable();
}

static bool check_pad(uint8_t pad) {
    bool res = false;
    if (s_pad_activated[pad]) {
        uint8_t tmp = s_pad_activated[pad];
        s_pad_activated[pad] = 0;
        ESP_LOGD(TAG, "T%d activated!", pad);
        if( s_pad_counter[pad] == 0 ) {
            s_pad_state[pad] = false;
        }
        if( s_pad_counter[pad] < 3) {
            s_pad_state[pad] = false;
            s_pad_counter[pad] += tmp;
        }
        if( s_pad_counter[pad] > 2 && s_pad_state[pad] == false) {
            res = true;
            s_pad_state[pad] = true;
        }
    }
    else {
        if(s_pad_counter[pad] > 0) {
            s_pad_counter[pad]--;
        }
    }
    return res;
}

static uint64_t touch_trigger() {
    uint64_t triggered_buttons = 0;
#if CONFIG_EASY_INPUT_TOUCH_UP_PIN != -1
    triggered_buttons |= (check_pad(CONFIG_EASY_INPUT_TOUCH_UP_PIN) << \
            EASY_INPUT_UP);
#endif
#if CONFIG_EASY_INPUT_TOUCH_DOWN_PIN != -1
    triggered_buttons |= (check_pad(CONFIG_EASY_INPUT_TOUCH_DOWN_PIN) << \
            EASY_INPUT_DOWN);
#endif
#if CONFIG_EASY_INPUT_TOUCH_LEFT_PIN != -1
    triggered_buttons |= (check_pad(CONFIG_EASY_INPUT_TOUCH_LEFT_PIN) << \
            EASY_INPUT_LEFT);
#endif
#if CONFIG_EASY_INPUT_TOUCH_RIGHT_PIN != -1
    triggered_buttons |= (check_pad(CONFIG_EASY_INPUT_TOUCH_RIGHT_PIN) << \
            EASY_INPUT_RIGHT);
#endif
#if CONFIG_EASY_INPUT_TOUCH_BACK_PIN != -1
    triggered_buttons |= (check_pad(CONFIG_EASY_INPUT_TOUCH_BACK_PIN) << \
            EASY_INPUT_BACK);
#endif
#if CONFIG_EASY_INPUT_TOUCH_ENTER_PIN != -1
    triggered_buttons |= (check_pad(CONFIG_EASY_INPUT_TOUCH_ENTER_PIN) << \
            EASY_INPUT_ENTER);
#endif
    return triggered_buttons;
}

void touch_task( void *input_queue ) {
    touch_setup();
    for(uint64_t triggered_buttons=0;;vTaskDelay(pdMS_TO_TICKS(31))) {
        triggered_buttons = 0;
        triggered_buttons |= touch_trigger();
        // If a button is triggered, send it off to the queue
        if(triggered_buttons){
            ESP_LOGD(TAG, "touch triggered");
            xQueueSend(*(QueueHandle_t *)input_queue, &triggered_buttons, 0);
        }
    }
    vTaskDelete(NULL); // Should never reach here!
}

#endif
