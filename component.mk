COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := src

$(call compile_only_if_not,$(CONFIG_EASY_INPUT_PUSH_BUTTON_ENABLE), push_button.o)
