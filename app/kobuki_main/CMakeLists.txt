# main program ran on the kobuki robot
add_executable(kobuki_main
    # entry-point
    main.c
    startup.s
    syscall.c
    sysmem.c
    # add any other files needed here ...
)

# main program depends on drivers, kobuki, and ultrasonic libraries
target_link_libraries(kobuki_main PRIVATE drivers kobuki ultrasonic cmsis_core cmsis_device_g4)

# set the output name to have .elf appended
set_target_properties(kobuki_main PROPERTIES OUTPUT_NAME "kobuki_main.elf")

# link with the Newlib-nano C library
target_compile_options(kobuki_main PRIVATE --specs=nano.specs)

# use the given linker script
target_link_options(kobuki_main PRIVATE
    -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)

# custom target for flashing