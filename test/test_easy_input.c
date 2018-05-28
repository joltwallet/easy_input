#include "unity.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_system.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "easy_input.h"

static void print_binary(uint64_t x){
    for(uint64_t i=1ULL<<63; i>0ULL; i>>=1){
        (x & i)? printf("1") : printf("0");
    }
}

TEST_CASE("Push Button 10 Times", "[easy_input]"){
    QueueHandle_t input_queue;
    TaskHandle_t h_push_button = NULL;
    uint64_t input_buf;

    easy_input_queue_init(&input_queue);
    xTaskCreate(easy_input_push_button_task, \
            "ButtonDebounce", 2048,
            (void *)&input_queue, 20, \
            &h_push_button);

    for(int i=0; i< 10; i++){
        // Block until user inputs a button
        if(xQueueReceive(input_queue, &input_buf, portMAX_DELAY)) {
            printf("User Input #%d: 0b", i);
            print_binary(input_buf);
            printf("\n");
        }
    }
    vTaskDelete(h_push_button);
}
