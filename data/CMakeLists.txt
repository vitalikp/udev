# udev data cmake file

configure_file(rpm/macros-udev.cmake macros.udev)

install(FILES ${PROJECT_BINARY_DIR}/data/macros.udev DESTINATION lib/rpm/macros.d)