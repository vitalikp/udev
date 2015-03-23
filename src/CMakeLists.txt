# udev cmake file

# generate keyboard-keys-from-name.h and keyboard-keys-to-name.h
include(keyboard-keys.cmake)

include_directories(core)
include_directories(core/net)


# udev
configure_file(udev.pc.cmake udev.pc)

# udev core
set(libudev_core_src
	core/udev.h
	core/udev-event.c
	core/udev-watch.c
	core/udev-node.c
	core/udev-rules.c
	core/udev-ctrl.c
	core/builtin/udev-builtin.c
	core/builtin/udev-builtin-btrfs.c
	core/builtin/udev-builtin-hwdb.c
	core/builtin/udev-builtin-input_id.c
	core/builtin/udev-builtin-keyboard.c
	core/builtin/udev-builtin-net_id.c
	core/builtin/udev-builtin-net_setup_link.c
	core/builtin/udev-builtin-path_id.c
	core/builtin/udev-builtin-usb_id.c
	core/net/link-config.h
	core/net/link-config.c
	core/net/ethtool-util.h
	core/net/ethtool-util.c
)
if (${KMOD_ENABLE})
	list(APPEND libudev_core_src core/builtin/udev-builtin-kmod.c)
endif()
if (${BLKID_ENABLE})
	list(APPEND libudev_core_src core/builtin/udev-builtin-blkid.c)
endif()
add_library(libudev_core_obj STATIC
	${libudev_core_src}
	${PROJECT_BINARY_DIR}/src/link-config-gperf.c
)
target_link_libraries(libudev_core_obj ${KMOD_LIBRARIES} ${BLKID_LIBRARIES})

add_custom_command(
	OUTPUT link-config-gperf.c
	COMMAND ${GPERF} < ${CMAKE_CURRENT_SOURCE_DIR}/core/net/link-config-gperf.gperf > link-config-gperf.c
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/core/net/link-config-gperf.gperf
)

#add_custom_target(
#	link-config-gperf.c ALL
#	COMMAND ${GPERF} < ${CMAKE_CURRENT_SOURCE_DIR}/core/net/link-config-gperf.gperf > link-config-gperf.c
#	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/core/net/link-config-gperf.gperf
#	#WORKING_DIRECTORY ...
#)
#add_dependencies(libudev_core_obj link-config-gperf.c)

# udebadm
add_library(udevadm_obj OBJECT
	udevadm/udevadm.c
	udevadm/udevadm-info.c
	udevadm/udevadm-control.c
	udevadm/udevadm-monitor.c
	udevadm/udevadm-hwdb.c
	udevadm/udevadm-settle.c
	udevadm/udevadm-trigger.c
	udevadm/udevadm-test.c
	udevadm/udevadm-test-builtin.c
)

add_executable(udevadm
	$<TARGET_OBJECTS:udevadm_obj>
	$<TARGET_OBJECTS:libudev_obj>
)
target_link_libraries(udevadm libudev_int_obj libudev_shared_obj libudev_core_obj libudev_label_obj)
target_link_libraries(udevadm rt dl gcc_s ${CAP_LIBRARIES} pthread)

# udevd
add_executable(udevd
	udevd/udevd.c
	$<TARGET_OBJECTS:libudev_obj>
)
target_link_libraries(udevd libudev_int_obj libudev_shared_obj libudev_core_obj libudev_label_obj)
target_link_libraries(udevd rt pthread)

# install
install(FILES ${PROJECT_BINARY_DIR}/src/udev.pc DESTINATION share/pkgconfig)
install(TARGETS udevadm DESTINATION ${bindir})
install(TARGETS udevd DESTINATION ${sbindir})