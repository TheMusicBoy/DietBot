cmake_minimum_required(VERSION 3.24)
project(DietBot)

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Build settings
set(CMAKE_CXX_STANDARD 20)

option(DEBUG "" OFF)

if (${DEBUG})
    message(STATUS "Building with debug...")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

find_package(absl REQUIRED)
find_package(Protobuf 5.27 REQUIRED)
set(PROTOC_EXEC "/Users/danilarutin/Downloads/protoc-27/bin/protoc")

# Building
add_subdirectory(thirdparty)
add_subdirectory(proto)
add_subdirectory(src)

option(TEST "" OFF)

if (${TEST})
    message(STATUS "Building tests...")
    add_subdirectory(tests)
endif()

add_custom_target(build_finished ALL
    COMMENT "Build almost finished...")

add_custom_command(TARGET build_finished POST_BUILD
    COMMAND cp ${PROJECT_BINARY_DIR}/compile_commands.json ${PROJECT_SOURCE_DIR}/
)
add_custom_command(TARGET build_finished POST_BUILD
    COMMAND echo "ctest --test-dir ${PROJECT_BINARY_DIR}/tests" > ${PROJECT_BINARY_DIR}/run_test
)
message(STATUS "Build finished")
