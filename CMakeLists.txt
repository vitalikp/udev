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
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-common -fno-strict-aliasing -ffunction-sections -fdata-sections -fstack-protector -fstack-protector-strong")
# debugging and optimizations flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --param=ssp-buffer-size=4 -flto -ffat-lto-objects -g -O2")
set(CMAKE_SHARED_LIBRARY_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden -fPIC")
set(CMAKE_EXE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--as-needed -Wl,--no-undefined -Wl,--gc-sections -Wl,-z,relro -Wl,-z,now -pie -Wl,-fuse-ld=gold")


# check program and library
include(check.cmake)


set(_GNU_SOURCE 1)
set(udevlibexecdir ${CMAKE_INSTALL_PREFIX})
set(udevlibexecdir ${CMAKE_INSTALL_PREFIX})
add_definitions(-DUDEVLIBEXECDIR="${udevlibexecdir}")

# config
configure_file(config.h.cmake config.h)
add_definitions(-include ${PROJECT_BINARY_DIR}/config.h)

# source code
include_directories(include)
include_directories(shared)
include_directories(lib)
include_directories(src/core)
add_subdirectory(shared)
add_subdirectory(lib)
add_subdirectory(src)
