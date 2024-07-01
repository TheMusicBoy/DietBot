include(FetchContent)

# -- Google test --

FetchContent_Declare(googletest
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip)

FetchContent_MakeAvailable(googletest)
FetchContent_GetProperties(googletest)

if (NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

# -- Libpq++ --

FetchContent_Declare(pqxx
    GIT_REPOSITORY https://github.com/jtv/libpqxx.git
    GIT_TAG 7.9.1)

FetchContent_MakeAvailable(pqxx)
FetchContent_GetProperties(pqxx)

if (NOT pqxx_POPULATED)
    FetchContent_Populate(pqxx)
    add_subdirectory(${pqxx_SOURCE_DIR} ${pqxx_BINARY_DIR})
endif()

# -- Fmt --

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)

FetchContent_MakeAvailable(fmt)
FetchContent_GetProperties(fmt)

if (NOT fmt_POPULATED)
    FetchContent_Populate(fmt)
    add_subdirectory(${fmt_SOURCE_DIR} ${fmt_BINARY_DIR})
endif()
