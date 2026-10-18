# Toolchain file
# https://kleinembedded.com/stm32-without-cubeide-part-4-cmake-fpu-and-stm32-libraries/

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

set(MCU_FLAGS "-mcpu=cortex-m4 -mthumb")

set(CMAKE_C_FLAGS_INIT ${MCU_FLAGS})
set(CMAKE_CXX_FLAGS_INIT ${MCU_FLAGS})
set(CMAKE_ASM_FLAGS_INIT ${MCU_FLAGS})

add_compile_definitions(STM32G474xx)