# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ffunction-sections -fdata-sections -Wl,--gc-sections")
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.1)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.1.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(rp2040-programmer-calculator C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(rp2040-programmer-calculator rp2040-programmer-calculator.c )

pico_set_program_name(rp2040-programmer-calculator "rp2040-programmer-calculator")
pico_set_program_version(rp2040-programmer-calculator "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(rp2040-programmer-calculator 0)
pico_enable_stdio_usb(rp2040-programmer-calculator 1)

# U8G2 library
target_include_directories(rp2040-programmer-calculator PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/u8g2/csrc
)
file(GLOB U8G2_SRC ${CMAKE_CURRENT_LIST_DIR}/u8g2/csrc/*.c)
add_library(u8g2 ${U8G2_SRC})
target_link_libraries(rp2040-programmer-calculator u8g2)

# Add the standard library to the build
target_link_libraries(rp2040-programmer-calculator
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(rp2040-programmer-calculator PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

add_subdirectory(tca8418)
add_subdirectory(bit_leds)

# Add any user requested libraries
target_link_libraries(rp2040-programmer-calculator 
        hardware_spi
        hardware_i2c
        tca8418
        bit_leds
        )

pico_add_extra_outputs(rp2040-programmer-calculator)

