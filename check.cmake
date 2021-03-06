# check program and library cmake file

include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckLibraryExists)

# check ln
find_program(LN ln)
if (NOT LN)
	message(FATAL_ERROR "*** ln not found! ***")
else()
	message("-- Found ln")
endif()

# check zsh
find_program(ZSH zsh)
if (ZSH)
	message("-- Found zsh")
endif()

# check xsltproc
find_program(XSLTPROC xsltproc)
if (NOT XSLTPROC)
	message(FATAL_ERROR "*** xsltproc not found! ***")
else()
	message("-- Found xsltproc")
endif()

# check awk
find_program(AWK awk)
if (NOT AWK)
	message(FATAL_ERROR "*** awk not found! ***")
else()
	EXECUTE_PROCESS(COMMAND ${AWK} -V OUTPUT_VARIABLE AWK_VERSION)
	string(REGEX REPLACE "^GNU Awk ([^\n]+)\n.*" "\\1" AWK_VERSION "${AWK_VERSION}")
	message("-- Found awk ${AWK_VERSION}")
endif()

# check gperf
find_program(GPERF gperf)
if (NOT GPERF)
	message(FATAL_ERROR "*** gperf not found! ***")
else()
	EXECUTE_PROCESS(COMMAND ${GPERF} -v OUTPUT_VARIABLE GPERF_VERSION)
	string(REGEX REPLACE "^GNU gperf ([^\n]+)\n.*" "\\1" GPERF_VERSION "${GPERF_VERSION}")
	message("-- Found gperf ${GPERF_VERSION}")
endif()

# check type size
check_type_size(pid_t SIZEOF_PID_T)
check_type_size(uid_t SIZEOF_UID_T)
check_type_size(gid_t SIZEOF_GID_T)
check_type_size(time_t SIZEOF_TIME_T)
set(CMAKE_EXTRA_INCLUDE_FILES sys/time.h sys/resource.h)
check_type_size(rlim_t SIZEOF_RLIM_T)

# check headers
check_include_file(sys/auxv.h HAVE_SYS_AUXV_H)

# check functions
CHECK_FUNCTION_EXISTS(__secure_getenv HAVE___SECURE_GETENV)
CHECK_FUNCTION_EXISTS(secure_getenv HAVE_SECURE_GETENV)
set(CMAKE_EXTRA_INCLUDE_FILES fcntl.h)
CHECK_FUNCTION_EXISTS(name_to_handle_at HAVE_DECL_NAME_TO_HANDLE_AT)
set(CMAKE_EXTRA_INCLUDE_FILES sched.h)
check_function_exists(setns HAVE_DECL_SETNS)

# check dl
CHECK_LIBRARY_EXISTS(dl dlsym "" DL_FOUND)
if (NOT DL_FOUND)
	message(FATAL_ERROR "*** Dynamic linking loader library not found ***")
endif()

# check rt
CHECK_LIBRARY_EXISTS(rt mq_open "" RT_FOUND)
if (NOT RT_FOUND)
	message(FATAL_ERROR "*** POSIX RT library not found ***")
endif()

# dependencies
find_package(PkgConfig REQUIRED)

# check bash-completion
pkg_check_modules(BASH_COMPL bash-completion)

# check tests option
option(TESTS_ENABLE "Enable build tests" OFF)
if (${TESTS_ENABLE})
	enable_testing()
endif()

# check systemd library
option(SYSTEMD_ENABLE "Disable optional systemd support" ON)
if (${SYSTEMD_ENABLE})
	pkg_check_modules(SYSTEMD REQUIRED libsystemd>=214)
	set(HAVE_SYSTEMD 1)
endif()

# check kmod library
option(KMOD_ENABLE "Disable loadable modules support" ON)
if (${KMOD_ENABLE})
	pkg_check_modules(KMOD REQUIRED libkmod>=15)
	set(HAVE_KMOD 1)
endif()

# check blkid library
option(BLKID_ENABLE "Disable blkid support" ON)
if (${BLKID_ENABLE})
	pkg_check_modules(BLKID REQUIRED blkid>=2.20)
	set(HAVE_BLKID 1)
endif()

# check SELinux library
option(SELINUX_ENABLE "Disable optional SELINUX support" ON)
if (${SELINUX_ENABLE})
	pkg_check_modules(SELINUX REQUIRED libselinux)
	set(HAVE_SELINUX 1)
endif()
