# udev cmake file

# generate keyboard-keys-from-name.h and keyboard-keys-to-name.h
include(keyboard-keys.cmake)

include_directories(core)


# udev
configure_file(udev.pc.cmake udev.pc)

# udev core
add_subdirectory(core)

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
)
target_link_libraries(udevadm udev_core_obj)

# udevd
add_executable(udevd
	udevd/udevd.c
)
target_link_libraries(udevd udev_core_obj)

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
)
target_link_libraries(ata_id udev_int_obj udev_shared_obj)
install(TARGETS ata_id DESTINATION ${udevlibexecdir})

# cdrom_id
add_executable(cdrom_id
	cdrom_id/cdrom_id.c
)
target_link_libraries(cdrom_id udev_int_obj udev_shared_obj)
install(TARGETS cdrom_id DESTINATION ${udevlibexecdir})

# collect
add_executable(collect
	collect/collect.c
)
target_link_libraries(collect udev_int_obj udev_shared_obj)
install(TARGETS collect DESTINATION ${udevlibexecdir})

# scsi_id
add_executable(scsi_id
	scsi_id/scsi_id.c
	scsi_id/scsi_serial.c
	scsi_id/scsi.h
	scsi_id/scsi_id.h
)
target_link_libraries(scsi_id udev_int_obj udev_shared_obj)
install(TARGETS scsi_id DESTINATION ${udevlibexecdir})

# v4l_id
add_executable(v4l_id
	v4l_id/v4l_id.c
)
target_link_libraries(v4l_id udev_int_obj)
install(TARGETS v4l_id DESTINATION ${udevlibexecdir})

# accelerometer
add_executable(accelerometer
	accelerometer/accelerometer.c
)
target_link_libraries(accelerometer udev_int_obj udev_shared_obj)
target_link_libraries(accelerometer m)
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
