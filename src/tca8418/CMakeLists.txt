add_library(tca8418 tca8418.c tca8418.h)
target_include_directories(tca8418 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})
target_link_libraries(tca8418 PUBLIC pico_stdlib hardware_i2c)
target_include_directories(tca8418 PUBLIC ${CMAKE_SOURCE_DIR})
