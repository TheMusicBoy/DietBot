include_directories(${Protobuf_INCLUDE_DIR})

set(SRCROOT "${PROJECT_SOURCE_DIR}/src/service")

set(SRC
    ${SRCROOT}/service.h
    ${SRCROOT}/service.cpp
)

if (NOT "${SRC}" STREQUAL "")
    message(STATUS "Building service...")
    add_library(service ${SRC})
    target_link_libraries(service PUBLIC db TgBot ${Protobuf_LIBRARIES})
    target_include_directories(service PUBLIC ${PROJECT_SOURCE_DIR}/src)
    target_include_directories(service PUBLIC ${PROJECT_SOURCE_DIR}/proto)
    target_include_directories(service PUBLIC ${Protobuf_INCLUDE_DIRS})
    set_target_properties(service PROPERTIES LINKER_LANGUAGE CXX)
else()
    message(WARNING "Service is not built")
endif()
