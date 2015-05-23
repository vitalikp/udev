# udev data cmake file

# etc
install(FILES udev.conf DESTINATION /etc/udev)

# hwdb
install(FILES hwdb/20-pci-vendor-model.hwdb DESTINATION ${udevhwdbdir})
install(FILES hwdb/20-pci-classes.hwdb DESTINATION ${udevhwdbdir})
install(FILES hwdb/20-usb-vendor-model.hwdb DESTINATION ${udevhwdbdir})
install(FILES hwdb/20-usb-classes.hwdb DESTINATION ${udevhwdbdir})
install(FILES hwdb/20-sdio-vendor-model.hwdb DESTINATION ${udevhwdbdir})
install(FILES hwdb/20-sdio-classes.hwdb DESTINATION ${udevhwdbdir})
install(FILES hwdb/20-bluetooth-vendor-product.hwdb DESTINATION ${udevhwdbdir})
install(FILES hwdb/20-acpi-vendor.hwdb DESTINATION ${udevhwdbdir})
install(FILES hwdb/20-OUI.hwdb DESTINATION ${udevhwdbdir})
install(FILES hwdb/20-net-ifname.hwdb DESTINATION ${udevhwdbdir})
install(FILES hwdb/60-keyboard.hwdb DESTINATION ${udevhwdbdir})

# rpm
configure_file(rpm/macros-udev.cmake macros.udev)

install(FILES ${PROJECT_BINARY_DIR}/data/macros.udev DESTINATION lib/rpm/macros.d)

# udev rules
# cdrom_id rules
install(FILES rules/60-cdrom_id.rules DESTINATION ${udevrulesdir})
# v4l_id
install(FILES rules/60-persistent-v4l.rules DESTINATION ${udevrulesdir})
# accelerometer
install(FILES rules/61-accelerometer.rules DESTINATION ${udevrulesdir})

# systemd
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/data/sysinit.target.wants/)
execute_process(COMMAND ${LN} -fs ../udevd-kernel.socket ${PROJECT_BINARY_DIR}/data/sysinit.target.wants/)
execute_process(COMMAND ${LN} -fs ../udevd-control.socket ${PROJECT_BINARY_DIR}/data/sysinit.target.wants/)
execute_process(COMMAND ${LN} -fs ../udevd.service ${PROJECT_BINARY_DIR}/data/sysinit.target.wants/)
configure_file(systemd/udev-trigger.service.cmake udev-trigger.service)
configure_file(systemd/udev-settle.service.cmake udev-settle.service)
configure_file(systemd/udevd.service.cmake udevd.service)

install(FILES systemd/udevd-kernel.socket DESTINATION lib/systemd/system)
install(FILES systemd/udevd-control.socket DESTINATION lib/systemd/system)
install(FILES ${PROJECT_BINARY_DIR}/data/udevd.service DESTINATION lib/systemd/system)
install(DIRECTORY ${PROJECT_BINARY_DIR}/data/sysinit.target.wants DESTINATION lib/systemd/system)
install(FILES ${PROJECT_BINARY_DIR}/data/udev-trigger.service DESTINATION lib/systemd/system)
install(FILES ${PROJECT_BINARY_DIR}/data/udev-settle.service DESTINATION lib/systemd/system)
