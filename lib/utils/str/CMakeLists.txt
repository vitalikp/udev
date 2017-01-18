# udev str utils library cmake file


add_library(udev_str_obj STATIC
	new.c
	resize.c
	set.c
	append.c
)


# tests
if (${TESTS_ENABLE})

# test test-str-empty
add_executable(test-str-empty
	test-empty.c
)

# test test-str-eq
add_executable(test-str-eq
	test-eq.c
)

# test test-str-copy
add_executable(test-str-copy
	test-copy.c
)

# test str-new
add_executable(test-str-new
	new.c
)
target_compile_definitions(test-str-new PRIVATE TESTS)

# test str-set
add_executable(test-str-set
	set.c
)
target_compile_definitions(test-str-set PRIVATE TESTS)
target_link_libraries(test-str-set udev_str_obj)

# test str-append
add_executable(test-str-append
	append.c
)
target_compile_definitions(test-str-append PRIVATE TESTS)
target_link_libraries(test-str-append udev_str_obj)

add_test(NAME utils-str-empty COMMAND ./test-str-empty)
add_test(NAME utils-str-eq COMMAND ./test-str-empty)
add_test(NAME utils-str-copy COMMAND ./test-str-copy)
add_test(NAME utils-str-new COMMAND ./test-str-new)
add_test(NAME utils-str-set COMMAND ./test-str-set)
add_test(NAME utils-str-append COMMAND ./test-str-append)

endif()