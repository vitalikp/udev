# main udev cmake file
project(udev)
cmake_minimum_required(VERSION 3.0)


set(VERSION 214.4-dev)
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -pipe")
# preprocessor flags
set(CMAKE_C_FLAGS_RELEASE "-Wp,-D_FORTIFY_SOURCE=2")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "-Wp,-D_FORTIFY_SOURCE=2")
# warnings flags
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-result")
# code generation flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-common -fno-strict-aliasing -ffunction-sections -fdata-sections -fstack-protector -fstack-protector-strong -fexceptions")
# debugging and optimizations flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pie --param=ssp-buffer-size=4 -flto -ffat-lto-objects -ffast-math -fvisibility=hidden")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2 -DNDEBUG")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} -g -O2 -DNDEBUG")
set(CMAKE_SHARED_LIBRARY_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_EXE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed -Wl,--no-undefined -Wl,--gc-sections -Wl,-z,relro -Wl,-z,now -Wl,-fuse-ld=gold")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--no-undefined -Wl,--gc-sections -Wl,-z,relro -Wl,-z,now -Wl,-fuse-ld=gold")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_SHARED_LIBRARY_LINK_C_FLAGS)


# check program and library
include(check.cmake)


# directories
set(_GNU_SOURCE 1)
if (NOT SYSCONF_INSTALL_DIR)
	set(SYSCONF_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/etc")
endif()
set(sysconfdir ${SYSCONF_INSTALL_DIR})
add_definitions(-DSYSCONFDIR="${sysconfdir}")
set(udevsysconfdir ${SYSCONF_INSTALL_DIR}/udev)
add_definitions(-DUDEVSYSCONFDIR="${udevsysconfdir}")
set(udevrundir /run/udev)
add_definitions(-DUDEVRUNDIR="${udevrundir}")
set(bindir ${CMAKE_INSTALL_PREFIX}/bin)
set(sbindir ${CMAKE_INSTALL_PREFIX}/sbin)
set(includedir ${CMAKE_INSTALL_PREFIX}/include)
set(libdir ${CMAKE_INSTALL_PREFIX}/lib64)
set(udevlibexecdir ${CMAKE_INSTALL_PREFIX}/lib/udev)
add_definitions(-DUDEVLIBEXECDIR="${udevlibexecdir}")
set(udevhwdbdir ${udevlibexecdir}/hwdb.d)
set(udevrulesdir ${udevlibexecdir}/rules.d)
set(datadir ${CMAKE_INSTALL_PREFIX}/share)

# config
configure_file(config.h.cmake config.h)
add_definitions(-include ${PROJECT_BINARY_DIR}/config.h)

# udev man
include(man/udev-man.cmake)
add_man(docs udev.conf 5)
add_man(docs udev 7)
add_man(docs udevd 8)
add_man(docs udevadm 8)
add_custom_target(man ALL DEPENDS ${docs})

# source code
include_directories(shared)
include_directories(lib)
include_directories(src/core)
add_subdirectory(shared)
add_subdirectory(lib)
add_subdirectory(src)

# data
add_subdirectory(data)

# tests
if (${TESTS_ENABLE})
add_subdirectory(test)
endif()

# show current config
message("   ${CMAKE_PROJECT_NAME} ${VERSION}(${CMAKE_BUILD_TYPE})")
message("      prefix:      ${CMAKE_INSTALL_PREFIX}")
message("     sysconf:      ${SYSCONF_INSTALL_DIR}")
message("       tests:      ${TESTS_ENABLE}")
message("     systemd:      ${SYSTEMD_ENABLE}")
message("        kmod:      ${KMOD_ENABLE}")
message("       blkid:      ${BLKID_ENABLE}")
message("     SELinux:      ${SELINUX_ENABLE}")
