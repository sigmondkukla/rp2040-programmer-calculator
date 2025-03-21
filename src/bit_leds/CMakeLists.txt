add_library(bit_leds bit_leds.c bit_leds.h)
target_include_directories(bit_leds PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})
target_link_libraries(bit_leds PUBLIC pico_stdlib hardware_gpio)
target_include_directories(bit_leds PUBLIC ${CMAKE_SOURCE_DIR})
