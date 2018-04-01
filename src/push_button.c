#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "soc/timer_group_struct.h"
#include "driver/timer.h"
#include "driver/gpio.h"

#include "easy_input.h"

static void setup_push_button(uint8_t pin);

// Struct to hold status for a button debounce
typedef struct{
    uint8_t pin;
    uint8_t counter;
    bool prev_state;
} push_button_t;

static void init_push_button(push_button_t *x, uint8_t pin){
    setup_push_button(pin);
    x->pin = pin;
    x->counter = 0;
    x->prev_state = true;
}

void easy_input_queue_init(QueueHandle_t *input_queue){
    *input_queue = xQueueCreate(5, sizeof(uint64_t));
}

static bool button_periodic_update(push_button_t *button){
    /* Update push_button's debounce counter.
     * Returns true if a button press has been successfully detected
     * Returns false if the button has not been debounce pressed yet
     */
    if(!gpio_get_level(button->pin)){
        if(button->counter < CONFIG_EASY_INPUT_PUSH_BUTTON_DEBOUNCE_PERIOD){
            (button->counter)++;
        }
        else if(button->prev_state && \
                button->counter >= CONFIG_EASY_INPUT_PUSH_BUTTON_DEBOUNCE_PERIOD){
            button->prev_state = false;
            return true;
        }
    }
    else{
        if(button->counter > 0){
            button->counter--;
        }
        else{
            button->prev_state = true;
        }
    }
    return false;
}

void easy_input_push_button_task( void *input_queue ){
    /* Sets up Input buttons, polls them for digital filter, and when
     * a debounced input has been triggered, puts the resulting bitmask onto
     * the specified input_queue */
    TickType_t xNextWakeTime = xTaskGetTickCount();

    // Declare buttons
    #if CONFIG_EASY_INPUT_PUSH_BUTTON_UP_PIN != -1
        push_button_t up_pb;
        init_push_button(&up_pb, CONFIG_EASY_INPUT_PUSH_BUTTON_UP_PIN);
    #endif
    #if CONFIG_EASY_INPUT_PUSH_BUTTON_DOWN_PIN != -1
        push_button_t down_pb;
        init_push_button(&down_pb, CONFIG_EASY_INPUT_PUSH_BUTTON_DOWN_PIN);
    #endif
    #if CONFIG_EASY_INPUT_PUSH_BUTTON_LEFT_PIN != -1
        push_button_t left_pb;
        init_push_button(&left_pb, CONFIG_EASY_INPUT_PUSH_BUTTON_LEFT_PIN);
    #endif
    #if CONFIG_EASY_INPUT_PUSH_BUTTON_RIGHT_PIN != -1
        push_button_t right_pb;
        init_push_button(&right_pb, CONFIG_EASY_INPUT_PUSH_BUTTON_RIGHT_PIN);
    #endif
    #if CONFIG_EASY_INPUT_PUSH_BUTTON_BACK_PIN != -1
        push_button_t back_pb;
        init_push_button(&back_pb, CONFIG_EASY_INPUT_PUSH_BUTTON_BACK_PIN);
    #endif
    #if CONFIG_EASY_INPUT_PUSH_BUTTON_ENTER_PIN != -1
        push_button_t enter_pb;
        init_push_button(&enter_pb, CONFIG_EASY_INPUT_PUSH_BUTTON_ENTER_PIN);
    #endif

    for(uint64_t triggered_buttons=0;;
            vTaskDelayUntil(&xNextWakeTime, \
            pdMS_TO_TICKS(CONFIG_EASY_INPUT_PUSH_BUTTON_POLLING_PERIOD_MS))) {
        triggered_buttons = 0;

        #if CONFIG_EASY_INPUT_PUSH_BUTTON_UP_PIN != -1
            triggered_buttons |= (button_periodic_update(&up_pb) << \
                    EASY_INPUT_UP);
        #endif
        #if CONFIG_EASY_INPUT_PUSH_BUTTON_DOWN_PIN != -1
            triggered_buttons |= (button_periodic_update(&down_pb) << \
                    EASY_INPUT_DOWN);
        #endif
        #if CONFIG_EASY_INPUT_PUSH_BUTTON_LEFT_PIN != -1
            triggered_buttons |= (button_periodic_update(&left_pb) << \
                    EASY_INPUT_LEFT);
        #endif
        #if CONFIG_EASY_INPUT_PUSH_BUTTON_RIGHT_PIN != -1
            triggered_buttons |= (button_periodic_update(&right_pb) << \
                    EASY_INPUT_RIGHT);
        #endif
        #if CONFIG_EASY_INPUT_PUSH_BUTTON_BACK_PIN != -1
            triggered_buttons |= (button_periodic_update(&back_pb) << \
                    EASY_INPUT_BACK);
        #endif
        #if CONFIG_EASY_INPUT_PUSH_BUTTON_ENTER_PIN != -1
            triggered_buttons |= (button_periodic_update(&enter_pb) << \
                    EASY_INPUT_ENTER);
        #endif

        // If a debounced button is triggered, send it off to the queue
        if(triggered_buttons){
            xQueueSend(*(QueueHandle_t *)input_queue, &triggered_buttons, 0);
        }
    }
    vTaskDelete(NULL); // Should never reach here!
}

static void setup_push_button(uint8_t pin){
    /* Setup a single pin as an input */
    gpio_config_t config; // config object
    config.mode = GPIO_MODE_INPUT;
    config.pin_bit_mask = 1ULL << pin;
    config.pull_up_en = 1;
    config.pull_down_en = 0;
    config.intr_type = GPIO_PIN_INTR_NEGEDGE;
    gpio_config(&config); // Apply Configurations
}
