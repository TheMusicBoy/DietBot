include_directories(${Protobuf_INCLUDE_DIR})

set(PROTO_ROOT "${PROJECT_SOURCE_DIR}/proto")

function(generate_proto TARGET_NAME PROTODIR OUTPUTDIR)
    set(PROTO_OUTPUT)
    foreach(PROTOFILE ${ARGN})
        STRING(REGEX REPLACE "${PROTODIR}/" "" FILENAME ${PROTOFILE})
        STRING(REGEX REPLACE ".proto" "" FILENAME ${FILENAME})
        set(PROTO_OUTPUT ${PROTO_OUTPUT} ${OUTPUTDIR}/${FILENAME}.pb.cc ${OUTPUTDIR}/${FILENAME}.pb.h)
    endforeach()

    if (NOT "${ARGN}" STREQUAL "")
        message(STATUS "Generating protobufs v${Protobuf_VERSION}...")
        add_custom_command(
            COMMAND mkdir -p ${OUTPUTDIR} && ${PROTOC_EXEC} --proto_path=${PROTODIR} --cpp_out=${OUTPUTDIR} ${ARGN}
            OUTPUT ${PROTO_OUTPUT}
        )
        message(STATUS "Generated: ${PROTO_OUTPUT}")

        message(STATUS "Building codegen files")
        add_library(${TARGET_NAME} STATIC ${PROTO_OUTPUT})
        target_include_directories(${TARGET_NAME} PUBLIC ${OUTPUTDIR})
        target_link_libraries(${TARGET_NAME} 
            protobuf::libprotobuf
            protobuf::libprotobuf-lite
            protobuf::libprotoc
            absl::log_internal_message
            absl::log_internal_check_op
            absl::status
        )
        set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
    endif()
endfunction()

generate_proto(
    proto_config
    ${PROTO_ROOT}
    ${PROTO_ROOT}/generated

# -- Proto sources --
    ${PROTO_ROOT}/config.proto
)

generate_proto(
    proto_schema
    ${PROTO_ROOT}
    ${PROTO_ROOT}/generated

# -- Proto sources --
    ${PROTO_ROOT}/consumer.proto
    ${PROTO_ROOT}/product.proto
    ${PROTO_ROOT}/chat.proto
)
