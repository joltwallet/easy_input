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
