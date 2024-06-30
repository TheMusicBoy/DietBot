include_directories(${Protobuf_INCLUDE_DIR})

set(PROTODIR "${PROJECT_SOURCE_DIR}/proto")
set(OUTPUTDIR "${PROTODIR}/output")

set(SRC
    # ${PROTODIR}/config.proto
)

set(OUTPUT)
foreach(PROTOFILE ${SRC})
    STRING(REGEX REPLACE "${PROTODIR}/" "" FILENAME ${PROTOFILE})
    STRING(REGEX REPLACE ".proto" "" FILENAME ${FILENAME})
    set(OUTPUT ${OUTPUT} ${OUTPUTDIR}/${FILENAME}.pb.cc ${OUTPUTDIR}/${FILENAME}.pb.h)
endforeach()

if (NOT "${SRC}" STREQUAL "")
    message(STATUS "Generating protobufs v${Protobuf_VERSION}...")
    add_custom_command(
        COMMAND mkdir -p ${PROTODIR}/output && ${PROTOC_EXEC} --proto_path=${PROTODIR} --cpp_out=${PROTODIR}/output ${SRC}
        OUTPUT ${OUTPUT}
    )
    message(STATUS "Generated: ${OUTPUT}")

    message(STATUS "Building codegen files")
    add_library(proto STATIC ${OUTPUT})
    target_include_directories(proto PUBLIC ${PROTODIR}/output)
    target_link_libraries(proto 
        protobuf::libprotobuf
        protobuf::libprotobuf-lite
        protobuf::libprotoc
        absl::log_internal_message
        absl::log_internal_check_op
        absl::status
    )
    set_target_properties(proto PROPERTIES LINKER_LANGUAGE CXX)
endif()
