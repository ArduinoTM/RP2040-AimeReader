cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

project(rp2040-CardReader C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

add_executable(rp2040-CardReader src/main.c src/usb_descriptors.c src/USB_Serial.c src/aime_reader.c)
target_include_directories(rp2040-CardReader PRIVATE include/)

target_link_libraries(rp2040-CardReader PRIVATE pico_stdlib tinyusb_device tinyusb_board hardware_pwm)
pico_add_extra_outputs(rp2040-CardReader)
