# main program ran on the kobuki robot
add_executable(prac6_test
    # entry-point
    main.c
    startup.s
    syscall.c
    sysmem.c
    # add any other files needed here ...
)

# main program depends on drivers, kobuki, and ultrasonic libraries
target_link_libraries(prac6_test PRIVATE drivers kobuki ultrasonic cmsis_core cmsis_device_g4)

# set the output name to have .elf appended
set_target_properties(prac6_test PROPERTIES OUTPUT_NAME "prac6_test.elf")

# link with the Newlib-nano C library
target_compile_options(prac6_test PRIVATE --specs=nano.specs)

# use the given linker script
target_link_options(prac6_test PRIVATE
    -T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld
)

# custom target for flashing