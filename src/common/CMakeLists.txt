include_directories(${Protobuf_INCLUDE_DIR})

set(SRCROOT "${PROJECT_SOURCE_DIR}/src/common")

set(SRC
    ${SRCROOT}/exception.h
    ${SRCROOT}/exception.cpp
    ${SRCROOT}/logger.h
    ${SRCROOT}/logger.cpp
    ${SRCROOT}/thread_pool.h
    ${SRCROOT}/thread_pool.cpp
)

if (NOT "${SRC}" STREQUAL "")
    add_library(common ${SRC})
    target_link_libraries(common PUBLIC fmt ${Protobuf_LIBRARIES})
    target_include_directories(common PUBLIC ${PROJECT_SOURCE_DIR}/src)
    target_include_directories(common PUBLIC ${PROJECT_SOURCE_DIR}/proto)
    target_include_directories(common PUBLIC ${Protobuf_INCLUDE_DIRS})
    set_target_properties(common PROPERTIES LINKER_LANGUAGE CXX)
else()
    message(WARNING "Common is not built")
endif()
