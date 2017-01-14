# udev node cmake file


include_directories(.)

# udev core
set(libudev_core_src
	udev-event.c
	udev-watch.c
	udev-node.c
	udev-rules.c
	udev-ctrl.c
	builtin/udev-builtin.c
	builtin/udev-builtin-btrfs.c
	builtin/udev-builtin-hwdb.c
	builtin/udev-builtin-input_id.c
	builtin/udev-builtin-keyboard.c
	builtin/udev-builtin-net_id.c
	builtin/udev-builtin-path_id.c
	builtin/udev-builtin-usb_id.c
)
if (${KMOD_ENABLE})
	list(APPEND libudev_core_src builtin/udev-builtin-kmod.c)
endif()
if (${BLKID_ENABLE})
	list(APPEND libudev_core_src builtin/udev-builtin-blkid.c)
endif()
add_library(udev_core_obj STATIC
	${libudev_core_src}
)
target_link_libraries(udev_core_obj udev_int_obj udev_label_obj udev_shared_obj)
target_link_libraries(udev_core_obj ${KMOD_LIBRARIES} ${BLKID_LIBRARIES} ${SYSTEMD_LIBRARIES})
target_link_libraries(udev_core_obj -pthread rt)

# tests
if (${TESTS_ENABLE})

# test rules
add_executable(test-rules
	udev-rules.c
)
target_compile_definitions(test-rules PRIVATE DEBUG TESTS)
target_link_libraries(test-rules udev_core_obj)

add_test(NAME udev-rules COMMAND ./test-rules)

endif()