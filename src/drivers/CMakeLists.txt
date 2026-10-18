# static library containing wrappers for MCU peripherals
add_library(drivers STATIC
    # GPIO init, read/write, pull-up/pull-down, and modes
    gpio.c
    # UART init and rx/tx
    uart.c
    # ADC
    adc.c
)

# headers are located under include/drivers
target_include_directories(drivers PUBLIC
    ${PROJECT_SOURCE_DIR}/include/drivers
)

# drivers depends on cmsis_interface
include(${CMAKE_SOURCE_DIR}/cmake/cmsis.cmake)
target_link_libraries(drivers PRIVATE cmsis_core cmsis_device_g4)