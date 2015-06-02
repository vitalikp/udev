# main udev cmake file
project(udev)
cmake_minimum_required(VERSION 3.0)


set(VERSION 214)
set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -pipe")
# preprocessor flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wp,-D_FORTIFY_SOURCE=2")
# warnings flags
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -pedantic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-result")
# code generation flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-common -fno-strict-aliasing -ffunction-sections -fdata-sections -fstack-protector -fstack-protector-strong -fexceptions")
# debugging and optimizations flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pie --param=ssp-buffer-size=4 -flto -ffat-lto-objects -ffast-math -g -O2 -fvisibility=hidden")
set(CMAKE_SHARED_LIBRARY_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
set(CMAKE_EXE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed -Wl,--no-undefined -Wl,--gc-sections -Wl,-z,relro -Wl,-z,now -Wl,-fuse-ld=gold")
set(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--no-undefined -Wl,--gc-sections -Wl,-z,relro -Wl,-z,now -Wl,-fuse-ld=gold")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)


# check program and library
include(check.cmake)


# directories
set(_GNU_SOURCE 1)
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
add_man(udev 7)
add_man(udevd 8)
add_man(udevadm 8)

# source code
include_directories(include)
include_directories(shared)
include_directories(lib)
include_directories(src/core)
add_subdirectory(shared)
add_subdirectory(lib)
add_subdirectory(src)

# data
add_subdirectory(data)

# tests
add_subdirectory(test)

# show current config
message("   ${CMAKE_PROJECT_NAME} ${VERSION}")
message("      prefix:      ${CMAKE_INSTALL_PREFIX}")
message("      kmod:        ${KMOD_ENABLE}")
message("      blkid:       ${BLKID_ENABLE}")
message("      SELinux:     ${SELINUX_ENABLE}")
message("      SMACK:       ${SMACK_ENABLE}")
message("        efi:       ${EFI_ENABLE}")
