#ifndef __INCLUDE_EASY_INPUT_H__
#define __INCLUDE_EASY_INPUT_H__

    typedef enum buttons{
        EASY_INPUT_UP,
        EASY_INPUT_DOWN,
        EASY_INPUT_LEFT,
        EASY_INPUT_RIGHT,
        EASY_INPUT_BACK,
        EASY_INPUT_ENTER,
        EASY_INPUT_NUMBER_OF_BUTTON_TYPES
    } buttons_t;

    #if 0
    // I/O GPIO Definitions
	// NOTE: GPIO 6-11 are usually used for SPI Flash
	//       GPIO34-39 Can only be input and DO NOT have software pull up/down
    #define BACK_PIN 13
    #define UP_PIN 12
    #define DOWN_PIN 14
    #define ENTER_PIN 27

	// Other Stuff
	#define ESP_INTR_FLAG_DEFAULT 0

    // Debounce Parameters
    #define BUTTON_POLLING_PERIOD_MS pdMS_TO_TICKS( 10 )
    #define BUTTON_DEBOUNCE_PERIOD 4

    
    void setup_buttons();

    TaskFunction_t vButtonDebounceTask( void *input_queue );
    #endif

    
    void easy_input_queue_init(QueueHandle_t *input_queue);
    void easy_input_push_button_task( void *input_queue );

#endif
