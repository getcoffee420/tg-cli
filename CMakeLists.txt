cmake_minimum_required(VERSION 3.16)
project(tg-cli CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tg-cli
        src/main.cpp
)

set(TDLIB_ROOT "/home/get-coffee/td/tdlib")

target_include_directories(tg-cli PRIVATE
        ${TDLIB_ROOT}/include
)

target_link_directories(tg-cli PRIVATE
        ${TDLIB_ROOT}/lib
)

target_link_libraries(tg-cli
        tdjson
        pthread
)
