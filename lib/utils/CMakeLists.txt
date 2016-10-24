# udev utils library cmake file


add_library(udev_utils_obj STATIC
	parse-uint.c
	path-create.c
	path-remove.c
	lookup-user.c
	lookup-group.c
)


# tests
if (${TESTS_ENABLE})

# test test-str-empty
add_executable(test-str-empty
	test-str-empty.c
)

# test test-str-copy
add_executable(test-str-copy
	test-str-copy.c
)

# test parse-uint
add_executable(test-parse-uint
	parse-uint.c
)
target_compile_definitions(test-parse-uint PRIVATE TESTS)

# test path-create
add_executable(test-path-create
	path-create.c
)
target_compile_definitions(test-path-create PRIVATE TESTS)

# test path-remove
add_executable(test-path-remove
	path-remove.c
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

add_test(NAME utils-str-empty COMMAND ./test-str-empty)
add_test(NAME utils-str-copy COMMAND ./test-str-copy)
add_test(NAME utils-parse-uint COMMAND ./test-parse-uint)
add_test(NAME utils-path-create COMMAND ./test-path-create)
add_test(NAME utils-path-remove COMMAND ./test-path-remove)
add_test(NAME utils-lookup-user COMMAND ./test-lookup-user)
add_test(NAME utils-lookup-group COMMAND ./test-lookup-group)

endif()
