# main program ran on the kobuki robot
add_executable(movement_test
    # entry-point
    main.c
    startup.s
    syscall.c
    sysmem.c
    # add any other files needed here ...
)

# main program depends on drivers, kobuki, and ultrasonic libraries
target_link_libraries(movement_test PRIVATE drivers kobuki ultrasonic cmsis_core cmsis_device_g4)

# set the output name to have .elf appended
set_target_properties(movement_test PROPERTIES OUTPUT_NAME "movement_test.elf")

# link with the Newlib-nano C library
target_compile_options(movement_test PRIVATE --specs=nano.specs)

# use the given linker script
target_link_options(movement_test PRIVATE
    -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)

# custom target for flashing