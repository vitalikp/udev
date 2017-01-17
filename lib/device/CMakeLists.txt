# udev device library cmake file


include_directories(.)

add_library(udev_device_obj STATIC
	init.c
	getaction.c
	setaction.c
)

# tests
if (${TESTS_ENABLE})

# test device set action
add_executable(test-device-setaction
	setaction.c
)
target_compile_definitions(test-device-setaction PRIVATE TESTS)

add_test(NAME device-setaction COMMAND ./test-device-setaction)

endif()
