/* easy_input - Filtered user input to FreeRTOS Queue for the ESP32
 Copyright (C) 2018  Brian Pugh, James Coxon, Michael Smaili
 https://www.joltwallet.com/
 
 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3 of the License, or
 (at your option) any later version.
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

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

TEST_CASE("Push Button 100 Times", "[easy_input]"){
    QueueHandle_t input_queue;
    uint64_t input_buf;

    easy_input_queue_init( &input_queue );
    easy_input_run( &input_queue );

    for(int i=0; i< 100; i++){
        // Block until user inputs a button
        if(xQueueReceive(input_queue, &input_buf, portMAX_DELAY)) {
            printf("User Input #%d: 0b", i);
            print_binary(input_buf);
            printf("\n");
        }
    }
    easy_input_stop();
    vQueueDelete(input_queue);
}
