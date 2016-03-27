# udev tests cmake file
enable_testing()

# test-libudev
add_executable(test-libudev
	test-libudev.c
)
target_link_libraries(test-libudev udev_int_obj udev_shared_obj udev_label_obj)

# root sys
#add_custom_target(sys.tar.xz
#	COMMAND tar -xJf ${CMAKE_CURRENT_SOURCE_DIR}/sys.tar.xz
#)

# test-udev
add_executable(test-udev
	test-udev.c
)
target_link_libraries(test-udev udev_core_obj)

# udev-test
add_executable(udev-test udev-test.c)
#add_dependencies(udev-test sys.tar.xz)
add_dependencies(udev-test test-udev)


add_test(NAME udev COMMAND ./udev-test)
