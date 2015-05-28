# libudev cmake file 

#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/libudev.sym -Wl,-z,relro")
set(LIBUDEV_C_FLAGS "-Wl,--version-script=${CMAKE_CURRENT_SOURCE_DIR}/libudev.sym -Wl,-z,relro")

#set(LIBUDEV_CURRENT 5)
#set(LIBUDEV_REVISION 1)
#set(LIBUDEV_AGE 4)

# (c-a).(a).(r)
set(LIBUDEV_SOVERSION 1)
set(LIBUDEV_VERSION ${LIBUDEV_SOVERSION}.4.1)

configure_file(libudev.pc.cmake libudev.pc @ONLY)

add_library(libudev_obj OBJECT
	#libudev.sym
	libudev-private.h
	libudev.c
	libudev-list.c
	libudev-util.c
	libudev-device.c
	libudev-enumerate.c
	libudev-monitor.c
	libudev-queue.c
	libudev-hwdb-def.h
	libudev-hwdb.c
)

# internal
add_library(libudev_int_obj STATIC
	libudev-device-private.c
)

add_library(udev SHARED
	$<TARGET_OBJECTS:libudev_obj>
)

set_target_properties(udev PROPERTIES
	VERSION ${LIBUDEV_VERSION}
	SOVERSION ${LIBUDEV_SOVERSION}
)
target_link_libraries(udev libudev_int_obj libudev_shared_obj ${LIBUDEV_C_FLAGS})
target_link_libraries(udev rt pthread)

# libudev header files
install(FILES libudev.h DESTINATION ${includedir})

# install
install(FILES ${PROJECT_BINARY_DIR}/lib/libudev.pc DESTINATION ${libdir}/pkgconfig)
install(TARGETS udev DESTINATION ${libdir})