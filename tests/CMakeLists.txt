set(SRCROOT "${PROJECT_SOURCE_DIR}/tests")

include(GoogleTest)
enable_testing()

function(test_libs)
    set(TEST_LIBS "${ARGN}" CACHE INTERNAL "TEST_LIBS")
endfunction()

function(test_dirs)
    set(TEST_DIRS "${ARGN}" CACHE INTERNAL "TEST_DIRS")
endfunction()

function(add_test NAME)
    add_executable(${NAME} "${SRCROOT}/${NAME}.cpp")
    target_link_libraries(${NAME} PUBLIC ${TEST_LIBS} GTest::gtest_main)
    target_include_directories(${NAME} PUBLIC ${PROJECT_SOURCE_DIR}/src ${TEST_DIRS})
    gtest_discover_tests(${NAME})
endfunction()

# Tests

add_subdirectory(proto_test)

test_libs(proto_test)
test_dirs(${SRCROOT}/proto_test)
add_test(test_proto)
