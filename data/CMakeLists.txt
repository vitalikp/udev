# udev data cmake file

# etc
install(FILES udev.conf DESTINATION ${udevsysconfdir})

# completions
# bash
if (BASH_COMPL_FOUND)
	install(FILES completions/bash/udevadm DESTINATION ${datadir}/bash-completion/completions)
endif(BASH_COMPL_FOUND)
# zsh
if (ZSH)
	install(FILES completions/zsh/_udevadm DESTINATION ${datadir}/zsh/site-functions)
endif(ZSH)

# hwdb
add_subdirectory(hwdb)

# rpm
configure_file(rpm/macros-udev.cmake macros.udev)

install(FILES ${PROJECT_BINARY_DIR}/data/macros.udev DESTINATION lib/rpm/macros.d)

# udev rules
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/data/rules.d)
install(DIRECTORY ${PROJECT_BINARY_DIR}/data/rules.d DESTINATION ${udevsysconfdir})
install(FILES rules/42-usb-hid-pm.rules DESTINATION ${udevrulesdir})
install(FILES rules/50-udev-default.rules DESTINATION ${udevrulesdir})
install(FILES rules/60-drm.rules DESTINATION ${udevrulesdir})
install(FILES rules/60-keyboard.rules DESTINATION ${udevrulesdir})
install(FILES rules/60-persistent-storage-tape.rules DESTINATION ${udevrulesdir})
install(FILES rules/60-persistent-serial.rules DESTINATION ${udevrulesdir})
install(FILES rules/60-persistent-input.rules DESTINATION ${udevrulesdir})
install(FILES rules/60-persistent-alsa.rules DESTINATION ${udevrulesdir})
install(FILES rules/60-persistent-storage.rules DESTINATION ${udevrulesdir})
install(FILES rules/64-btrfs.rules DESTINATION ${udevrulesdir})
install(FILES rules/75-net-description.rules DESTINATION ${udevrulesdir})
install(FILES rules/75-tty-description.rules DESTINATION ${udevrulesdir})
install(FILES rules/78-sound-card.rules DESTINATION ${udevrulesdir})
install(FILES rules/95-udev-late.rules DESTINATION ${udevrulesdir})
# kmod
if (${KMOD_ENABLE})
	install(FILES rules/80-drivers.rules DESTINATION ${udevrulesdir})
endif()
# cdrom_id rules
install(FILES rules/60-cdrom_id.rules DESTINATION ${udevrulesdir})
# v4l_id
install(FILES rules/60-persistent-v4l.rules DESTINATION ${udevrulesdir})
# accelerometer
install(FILES rules/61-accelerometer.rules DESTINATION ${udevrulesdir})
# mtd_probe
install(FILES rules/75-probe_mtd.rules DESTINATION ${udevrulesdir})

# systemd
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/data/sockets.target.wants/)
execute_process(COMMAND ${LN} -fs ../udevd-kernel.socket ${PROJECT_BINARY_DIR}/data/sockets.target.wants/)
execute_process(COMMAND ${LN} -fs ../udevd-control.socket ${PROJECT_BINARY_DIR}/data/sockets.target.wants/)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/data/sysinit.target.wants/)
execute_process(COMMAND ${LN} -fs ../udev-trigger.service ${PROJECT_BINARY_DIR}/data/sysinit.target.wants/)
execute_process(COMMAND ${LN} -fs ../udevd.service ${PROJECT_BINARY_DIR}/data/sysinit.target.wants/)
configure_file(systemd/udevd-control.socket.cmake udevd-control.socket)
configure_file(systemd/udev-trigger.service.cmake udev-trigger.service)
configure_file(systemd/udev-settle.service.cmake udev-settle.service)
configure_file(systemd/udevd.service.cmake udevd.service)

install(FILES systemd/udevd-kernel.socket DESTINATION lib/systemd/system)
install(FILES ${PROJECT_BINARY_DIR}/data/udevd-control.socket DESTINATION lib/systemd/system)
install(DIRECTORY ${PROJECT_BINARY_DIR}/data/sockets.target.wants DESTINATION lib/systemd/system)
install(FILES ${PROJECT_BINARY_DIR}/data/udevd.service DESTINATION lib/systemd/system)
install(DIRECTORY ${PROJECT_BINARY_DIR}/data/sysinit.target.wants DESTINATION lib/systemd/system)
install(FILES ${PROJECT_BINARY_DIR}/data/udev-trigger.service DESTINATION lib/systemd/system)
install(FILES ${PROJECT_BINARY_DIR}/data/udev-settle.service DESTINATION lib/systemd/system)
