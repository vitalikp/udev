# udev utils library cmake file


add_library(udev_utils_obj STATIC
	parse-uint.c
	lookup-user.c
	lookup-group.c
)


# tests
if (${TESTS_ENABLE})

# test parse-uint
add_executable(test-parse-uint
	parse-uint.c
)
target_compile_definitions(test-parse-uint PRIVATE TESTS)

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

add_test(NAME utils-parse-uint COMMAND ./test-parse-uint)
add_test(NAME utils-lookup-user COMMAND ./test-lookup-user)
add_test(NAME utils-lookup-group COMMAND ./test-lookup-group)

endif()
