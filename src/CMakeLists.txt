set(SRCROOT "${PROJECT_SOURCE_DIR}/src")

set(SRC
    ${SRCROOT}/main.cpp
)

if (NOT "${SRC}" STREQUAL "")
    message(STATUS "Building main executable...")
    add_executable(main ${SRC})
    target_include_directories(main PUBLIC ${PROJECT_SOURCE_DIR}/src)
    set_target_properties(main PROPERTIES LINKER_LANGUAGE CXX)
else()
    message(WARNING "Main executable is not built")
endif()
