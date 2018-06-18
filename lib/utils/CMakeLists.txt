# udev utils library cmake file


include_directories(.)
add_subdirectory(str)

add_library(udev_utils_obj STATIC
	parse-uint.c
	parse-value.c
	parse-argv.c
	path/relative.c
	path/encode.c
	path/mkdir.c
	path/remove.c
	lookup-user.c
	lookup-group.c
)
target_link_libraries(udev_utils_obj udev_str_obj)


# tests
if (${TESTS_ENABLE})

# test char-encode
add_executable(test-char-encode
	test-char-encode.c
)

# test parse-uint
add_executable(test-parse-uint
	parse-uint.c
)
target_compile_definitions(test-parse-uint PRIVATE TESTS)

# test parse-value
add_executable(test-parse-value
	parse-value.c
)
target_compile_definitions(test-parse-value PRIVATE TESTS)

# test parse-argv
add_executable(test-parse-argv
	parse-argv.c
)
target_compile_definitions(test-parse-argv PRIVATE TESTS)
target_link_libraries(test-parse-argv udev_utils_obj)

# test path-relative
add_executable(test-path-relative
	path/relative.c
)
target_compile_definitions(test-path-relative PRIVATE TESTS)

# test path-encode
add_executable(test-path-encode
	path/encode.c
)
target_compile_definitions(test-path-encode PRIVATE TESTS)

# test path-mkdir
add_executable(test-path-mkdir
	path/mkdir.c
)
target_compile_definitions(test-path-mkdir PRIVATE TESTS)

# test path-create
add_executable(test-path-create
	path/create.c
)
target_compile_definitions(test-path-create PRIVATE TESTS)
target_link_libraries(test-path-create udev_utils_obj)

# test path-remove
add_executable(test-path-remove
	path/remove.c
)
target_compile_definitions(test-path-remove PRIVATE TESTS)
target_link_libraries(test-path-remove udev_utils_obj)

# test lookup user
add_executable(test-lookup-user
	lookup-user.c
)
target_compile_definitions(test-lookup-user PRIVATE TESTS)
target_link_libraries(test-lookup-user udev_utils_obj)

# test lookup group
add_executable(test-lookup-group
	lookup-group.c
)
target_compile_definitions(test-lookup-group PRIVATE TESTS)
target_link_libraries(test-lookup-group udev_utils_obj)

add_test(NAME utils-char-encode COMMAND ./test-char-encode)
add_test(NAME utils-parse-uint COMMAND ./test-parse-uint)
add_test(NAME utils-parse-value COMMAND ./test-parse-value)
add_test(NAME utils-parse-argv COMMAND ./test-parse-argv)
add_test(NAME utils-path-relative COMMAND ./test-path-relative)
add_test(NAME utils-path-encode COMMAND ./test-path-encode)
add_test(NAME utils-path-mkdir COMMAND ./test-path-mkdir)
add_test(NAME utils-path-create COMMAND ./test-path-create)
add_test(NAME utils-path-remove COMMAND ./test-path-remove)
add_test(NAME utils-lookup-user COMMAND ./test-lookup-user)
add_test(NAME utils-lookup-group COMMAND ./test-lookup-group)

endif()
