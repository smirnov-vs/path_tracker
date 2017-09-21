cmake_minimum_required(VERSION 3.8)
project(path_tracking)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(clickhouse-cpp)

find_library(CLICKHOUSE_CPP NAMES clickhouse-cpp-lib PATHS clickhouse-cpp/clickhouse)
find_library(CITYHASH cityhash-lib PATHS clickhouse-cpp/contrib/cityhash)
find_library(LZ4 NAMES lz4-lib PATHS clickhouse-cpp/contrib/lz4)

include_directories(include)
include_directories(clickhouse-cpp)

aux_source_directory(src SRC)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} pthread PocoNet PocoFoundation PocoUtil
        ${CLICKHOUSE_CPP} ${CITYHASH} ${LZ4})
