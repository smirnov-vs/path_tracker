cmake_minimum_required(VERSION 3.5)
project(path_tracking)

add_compile_options("-std=c++17")

add_subdirectory(clickhouse-cpp)
include_directories(clickhouse-cpp)

include_directories(include)

aux_source_directory(src SRC)
add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} pthread
        PocoNet PocoFoundation PocoUtil
        clickhouse-cpp-lib cityhash-lib lz4-lib)
