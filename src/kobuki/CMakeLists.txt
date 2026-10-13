# static library containing abstraction for the kobuki operations
add_library(kobuki STATIC

    # defines anything declared in kobuki.h
    kobuki.c
)

# kobuki.h is located under include/kobuki
target_include_directories(kobuki PUBLIC
    ${PROJECT_SOURCE_DIR}/include/kobuki
)

# kobuki depends on drivers
target_link_libraries(kobuki PRIVATE drivers cmsis_core cmsis_device_g4)