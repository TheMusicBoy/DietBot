include_directories(${Protobuf_INCLUDE_DIR})

set(SRCROOT "${PROJECT_SOURCE_DIR}/src/db")

set(SRC
    ${SRCROOT}/client.h
    ${SRCROOT}/client.cpp
)

if (NOT "${SRC}" STREQUAL "")
    message(STATUS "Building db client...")
    add_library(db ${SRC})
    target_link_libraries(db PUBLIC proto_config proto_schema pqxx common ${Protobuf_LIBRARIES})
    target_include_directories(db PUBLIC ${PROJECT_SOURCE_DIR}/src)
    target_include_directories(db PUBLIC ${PROJECT_SOURCE_DIR}/proto)
    target_include_directories(db PUBLIC ${Protobuf_INCLUDE_DIRS})
    set_target_properties(db PROPERTIES LINKER_LANGUAGE CXX)
else()
    message(WARNING "Db client is not built")
endif()
