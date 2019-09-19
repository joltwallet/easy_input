/* easy_input - Filtered user input to FreeRTOS Queue for the ESP32
 Copyright (C) 2018  Brian Pugh, James Coxon, Michael Smaili
 https://www.joltwallet.com/
 */

#ifndef __INCLUDE_EASY_INPUT_H__
#define __INCLUDE_EASY_INPUT_H__

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "sdkconfig.h"

#if CONFIG_EASY_INPUT_VOLATILE
extern volatile uint64_t easy_input_state;
#endif


typedef enum buttons{
    EASY_INPUT_UP,
    EASY_INPUT_DOWN,
    EASY_INPUT_LEFT,
    EASY_INPUT_RIGHT,
    EASY_INPUT_BACK,
    EASY_INPUT_ENTER,
    EASY_INPUT_NUMBER_OF_BUTTON_TYPES
} buttons_t;

void easy_input_queue_init(QueueHandle_t *input_queue);
void easy_input_run( QueueHandle_t *input_queue );
void easy_input_stop();

#endif
