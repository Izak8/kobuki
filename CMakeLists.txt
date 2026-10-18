cmake_minimum_required(VERSION 4.2.3)

project(KobukiRobot

    VERSION 0.0.1
    DESCRIPTION "Firmware source code for the Kobuki Robot assignment"
    HOMEPAGE_URL "https://github.com/izak8/kobuki"
    LANGUAGES C ASM
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED Yes)

# common driver wrappers (e.g. gpio, exti, timer etc.)
add_subdirectory(src/drivers)

# kobuki wrappers
add_subdirectory(src/kobuki)

# ultrasonic sensor wrappers
add_subdirectory(src/ultrasonic)

# main program
add_subdirectory(app/kobuki_main)

# prac6 test
add_subdirectory(app/prac6_test)