# Easy Input

A library that handles the setup, digital filtering, and queueing of user inputs for the ESP32.

# Design

The goal of this library is to abstract away an input method into a  FreeRTOS Queue. A message on this queue is a `uint64_t` bitmask for whatever inputs were activated since the last polling cycle. This abstraction allows easy swapping out of input methods; for example, switching out pushbuttons for touch sensors.

# Input types

* Bouncy pushbuttons - No hardware filtering necessary.

# Todo:

* Simultaneous button press logic

* Automate code generation of easy_input_push_button_task

* Hold press for buttons. 

# Unit Tests

Unit tests can be used by selecting this library with a target using the [ESP32 Unit Tester](https://github.com/BrianPugh/esp32_unit_tester).

```
make flash TEST_COMPONENTS='easy_input' monitor
```

The unit tests (in the `test` folder) is a good source of examples on how to use this library.

# Related Projects

[`menu8g2`](https://github.com/joltwallet/menu8g2) - menuing library using `easy_input` for user input and [`u8g2`](https://github.com/olikraus/u8g2) for graphics.
