cmake_minimum_required(VERSION 3.5)
project(path_tracking)

add_compile_options("-std=c++17")

add_subdirectory(clickhouse-cpp)

set(POCO_PREFIX      "/usr/local")
set(POCO_INCLUDE_DIR "${POCO_PREFIX}/include")
set(POCO_LIB_DIR     "${POCO_PREFIX}/lib")
set(POCO_LIBS
        ${POCO_LIB_DIR}/libPocoUtil.so
        ${POCO_LIB_DIR}/libPocoNet.so
        ${POCO_LIB_DIR}/libPocoFoundation.so
        pthread)


include_directories(clickhouse-cpp ${POCO_INCLUDE_DIR}
        /usr/local/include/mongocxx/v_noabi/
        /usr/local/include/bsoncxx/v_noabi/)
include_directories(include)

aux_source_directory(src SRC)
add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} ${POCO_LIBS}
        clickhouse-cpp-lib cityhash-lib lz4-lib
        mongocxx bsoncxx crypto curl)
