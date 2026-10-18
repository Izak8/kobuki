set(CMSIS_DEVICE_G4_PATH "${CMAKE_SOURCE_DIR}/extern/cmsis-device-g4")
set(CMSIS_DEVICE_G4_INC_PATH "${CMSIS_DEVICE_G4_PATH}/Include")

# cmsis_device_g4 is a header-only library
add_library(cmsis_device_g4 INTERFACE
    ${CMSIS_DEVICE_G4_INC_PATH}
)

target_include_directories(cmsis_device_g4 INTERFACE
    ${CMSIS_DEVICE_G4_INC_PATH}
)

set(CMSIS_CORE_PATH "${CMAKE_SOURCE_DIR}/extern/CMSIS_5")
set(CMSIS_CORE_INC_PATH "${CMSIS_CORE_PATH}/CMSIS/Core/Include")

add_library(cmsis_core INTERFACE
    ${CMSIS_CORE_INC_PATH}
)

target_include_directories(cmsis_core INTERFACE
    ${CMSIS_CORE_INC_PATH}
)