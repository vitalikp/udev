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
add_library(udev_core_obj STATIC
	${libudev_core_src}
	${PROJECT_BINARY_DIR}/src/link-config-gperf.c
)
target_link_libraries(udev_core_obj ${KMOD_LIBRARIES} ${BLKID_LIBRARIES})
target_link_libraries(udev_core_obj udev_int_obj udev_shared_obj)

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
#add_dependencies(udev_core_obj link-config-gperf.c)

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
target_link_libraries(udevadm udev_core_obj udev_label_obj)
target_link_libraries(udevadm rt dl gcc_s pthread)

# udevd
add_executable(udevd
	udevd/udevd.c
	$<TARGET_OBJECTS:libudev_obj>
)
target_link_libraries(udevd udev_core_obj udev_label_obj)
target_link_libraries(udevd rt pthread)

# udevrule
add_executable(udevrule
	udevrule/udevrule.h
	udevrule/udevrule-parse.c
	udevrule/udevrule-check.c
	udevrule/udevrule.c
)

# ata_id
add_executable(ata_id
	ata_id/ata_id.c
	$<TARGET_OBJECTS:libudev_obj>
)
target_link_libraries(ata_id udev_int_obj udev_shared_obj)
target_link_libraries(ata_id dl)
install(TARGETS ata_id DESTINATION ${udevlibexecdir})

# cdrom_id
add_executable(cdrom_id
	cdrom_id/cdrom_id.c
	$<TARGET_OBJECTS:libudev_obj>
)
target_link_libraries(cdrom_id udev_int_obj udev_shared_obj)
target_link_libraries(cdrom_id dl)
install(TARGETS cdrom_id DESTINATION ${udevlibexecdir})

# collect
add_executable(collect
	collect/collect.c
	$<TARGET_OBJECTS:libudev_obj>
)
target_link_libraries(collect udev_int_obj udev_shared_obj)
target_link_libraries(collect dl)
install(TARGETS collect DESTINATION ${udevlibexecdir})

# scsi_id
add_executable(scsi_id
	scsi_id/scsi_id.c
	scsi_id/scsi_serial.c
	scsi_id/scsi.h
	scsi_id/scsi_id.h
	$<TARGET_OBJECTS:libudev_obj>
)
target_link_libraries(scsi_id udev_int_obj udev_shared_obj)
target_link_libraries(scsi_id dl)
install(TARGETS scsi_id DESTINATION ${udevlibexecdir})

# v4l_id
add_executable(v4l_id
	v4l_id/v4l_id.c
)
target_link_libraries(v4l_id udev_int_obj)
target_link_libraries(v4l_id dl)
install(TARGETS v4l_id DESTINATION ${udevlibexecdir})

# accelerometer
add_executable(accelerometer
	accelerometer/accelerometer.c
	$<TARGET_OBJECTS:libudev_obj>
)
target_link_libraries(accelerometer udev_int_obj udev_shared_obj)
target_link_libraries(accelerometer m dl)
install(TARGETS accelerometer DESTINATION ${udevlibexecdir})

# mtd_probe
add_executable(mtd_probe
	mtd_probe/mtd_probe.c
	mtd_probe/mtd_probe.h
	mtd_probe/probe_smartmedia.c
)
target_link_libraries(mtd_probe dl)
install(TARGETS mtd_probe DESTINATION ${udevlibexecdir})

# install
install(FILES ${PROJECT_BINARY_DIR}/src/udev.pc DESTINATION share/pkgconfig)
install(TARGETS udevadm DESTINATION ${bindir})
install(TARGETS udevrule DESTINATION ${bindir})
install(TARGETS udevd DESTINATION ${sbindir})