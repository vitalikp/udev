# udev tests cmake file
enable_testing()

# root sys
add_custom_target(sys.tar.xz
	COMMAND tar -xJf ${CMAKE_CURRENT_SOURCE_DIR}/sys.tar.xz
)

# test-udev
add_executable(test-udev
	test-udev.c
	$<TARGET_OBJECTS:libudev_obj>
)
target_link_libraries(test-udev libudev_int_obj libudev_shared_obj libudev_core_obj libudev_label_obj)
target_link_libraries(test-udev rt pthread)

# udev-test
add_executable(udev-test udev-test.c)
add_dependencies(udev-test sys.tar.xz)
add_dependencies(udev-test test-udev)


add_test(NAME udev COMMAND ./udev-test)
