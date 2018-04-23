#ifndef __INCLUDE_EASY_INPUT_H__
#define __INCLUDE_EASY_INPUT_H__

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

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
void easy_input_push_button_task( void *input_queue );

#endif
