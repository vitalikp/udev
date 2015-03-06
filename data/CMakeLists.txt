# udev data cmake file

# rpm
configure_file(rpm/macros-udev.cmake macros.udev)

install(FILES ${PROJECT_BINARY_DIR}/data/macros.udev DESTINATION lib/rpm/macros.d)

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
