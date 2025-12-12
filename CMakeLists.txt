cmake_minimum_required(VERSION 3.16)
project(tg-cli CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()
include(GoogleTest)

set(ENV_FILE "${CMAKE_SOURCE_DIR}/.env")

if (EXISTS "${ENV_FILE}")
    file(READ "${ENV_FILE}" ENV_CONTENTS)

    string(REPLACE "\n" ";" ENV_LINES "${ENV_CONTENTS}")

    foreach (line ${ENV_LINES})
        if (line MATCHES "^#.*" OR line STREQUAL "")
            continue()
        endif ()

        string(REGEX MATCH "^[A-Za-z_][A-Za-z0-9_]*" KEY "${line}")
        string(REPLACE "${KEY}=" "" VALUE "${line}")

        set(${KEY} "${VALUE}")

        add_compile_definitions(${KEY}="${VALUE}")
    endforeach ()
endif ()

set(DEFAULT_TDLIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/td/tdlib")

set(TDLIB_ROOT "${DEFAULT_TDLIB_ROOT}" CACHE PATH
        "Path to TDLib install dir (with include/ and lib/)")

if (NOT EXISTS "${TDLIB_ROOT}/include/td" OR NOT EXISTS "${TDLIB_ROOT}/lib")
    message(FATAL_ERROR
            "TDLib not found at TDLIB_ROOT='${TDLIB_ROOT}'. "
            "Did you run TDLib build & install in third_party/td?"
    )
endif ()

set(Td_DIR "${TDLIB_ROOT}/lib/cmake/Td")
find_package(Td 1.8.58 REQUIRED)

# TG CLI LIB
add_library(tg-cli-lib
        src/label/Label.cpp
        src/tgClient/TgClientTdlib.cpp
        src/controllers/AuthController.cpp
        src/controllers/GetChatsController.cpp
        src/controllers/SendMessageController.cpp
        src/controllers/ChatHistoryController.cpp
        src/facade/TgClientFacade.cpp
        src/label/LabelsParser.cpp
        src/label/LabelsParser.h
)

target_include_directories(tg-cli-lib
        PUBLIC ${TDLIB_ROOT}/include
)

target_link_directories(tg-cli-lib
        PUBLIC ${TDLIB_ROOT}/lib
)

target_link_libraries(tg-cli-lib
        PUBLIC Td::TdStatic
)

# TG CLI
add_executable(tg-cli main/main.cpp)

target_include_directories(tg-cli
        PRIVATE src
)

target_link_directories(tg-cli
        PRIVATE src
)

target_link_libraries(tg-cli
        PRIVATE tg-cli-lib
)

# CORE TEST
add_executable(core_test
        core_test/main.cpp
)

target_include_directories(core_test
        PRIVATE src
)

target_link_directories(core_test
        PRIVATE src
)

target_link_libraries(core_test
        PRIVATE tg-cli-lib
)

# TEST
add_executable(tests tests/test.cpp
        tests/label/Label.cpp
        tests/label/LabelParserTest.cpp
)

target_include_directories(tests
        PRIVATE src
)

target_link_directories(tests
        PRIVATE src
)

target_link_libraries(tests
        PRIVATE tg-cli-lib gtest_main
)
gtest_discover_tests(tests)
