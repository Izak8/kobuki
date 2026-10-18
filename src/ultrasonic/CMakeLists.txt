# static library containing abstraction for the ultrasonic sensor
add_library(ultrasonic STATIC

    # defines anything declared in ultrasonic.h
    ultrasonic.c
)

# ultrasonic.h is located under include/ultrasonic
target_include_directories(ultrasonic PUBLIC
    ${PROJECT_SOURCE_DIR}/include/ultrasonic
)

# ultrasonic depends on drivers
target_link_libraries(ultrasonic PRIVATE drivers cmsis_core cmsis_device_g4)