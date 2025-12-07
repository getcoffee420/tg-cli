cmake_minimum_required(VERSION 3.16)
project(tg-cli CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tg-cli-lib
        src/label/Label.cpp
)

add_executable(tg-cli main/main.cpp)

add_executable(tests tests/test.cpp
        tests/label/Label.cpp)

# По умолчанию ищем TDLib в сабмодуле
set(DEFAULT_TDLIB_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/third_party/td/tdlib")

set(TDLIB_ROOT "${DEFAULT_TDLIB_ROOT}" CACHE PATH
        "Path to TDLib install dir (with include/ and lib/)")

if (NOT EXISTS "${TDLIB_ROOT}/include/td" OR NOT EXISTS "${TDLIB_ROOT}/lib")
    message(FATAL_ERROR
            "TDLib not found at TDLIB_ROOT='${TDLIB_ROOT}'. "
            "Did you run TDLib build & install in third_party/td?"
    )
endif()

target_include_directories(tg-cli PRIVATE
        ${TDLIB_ROOT}/include
)

target_link_directories(tg-cli PRIVATE
        ${TDLIB_ROOT}/lib
)

target_link_libraries(tg-cli
        tg-cli-lib
        tdjson
        pthread
)

enable_testing()

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

target_link_libraries(tests PRIVATE tg-cli-lib gtest_main)
target_include_directories(tg-cli-lib PUBLIC src)

include(GoogleTest)
gtest_discover_tests(tests)
