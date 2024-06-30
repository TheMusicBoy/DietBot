include_directories(${Protobuf_INCLUDE_DIR})

set(SRCROOT "${PROJECT_SOURCE_DIR}/src")

set(SRC
    ${SRCROOT}/main.cpp
)

if (NOT "${SRC}" STREQUAL "")
    message(STATUS "Building main executable...")
    add_executable(main ${SRC})
    target_link_libraries(main PUBLIC proto ${Protobuf_LIBRARIES})
    target_include_directories(main PUBLIC ${PROJECT_SOURCE_DIR}/src)
    target_include_directories(main PUBLIC ${PROJECT_SOURCE_DIR}/proto)
    target_include_directories(main PUBLIC ${Protobuf_INCLUDE_DIRS})
    set_target_properties(main PROPERTIES LINKER_LANGUAGE CXX)
else()
    message(WARNING "Main executable is not built")
endif()
