include_directories(${Protobuf_INCLUDE_DIR})

set(PROTO_ROOT "${PROJECT_SOURCE_DIR}/tests/proto_test")

generate_proto(proto_test
    "${PROTO_ROOT}"
    "${PROTO_ROOT}/output"

# -- Proto sources --
    ${PROTO_ROOT}/message.proto
)
