cmake_minimum_required(VERSION 3.2)
project(Allocator)

set(CMAKE_CXX_FLAGS_RELEASE "-std=c++14 -O2 -pthread")
set(CMAKE_CXX_FLAGS_DEBUG  "-std=c++14 -g -ggdb -pthread")

IF (NOT DEFINED ENV{BUILD_DIR})
    set(BUILD_DIR ".")
ELSE()
    set(BUILD_DIR $ENV{BUILD_DIR})
ENDIF ()

IF (DEFINED ENV{BENCH_DIR})
    set(BENCH_DIR $ENV{BENCH_DIR})
    add_definitions(-D_BENCHMARKS)
    include_directories(${BENCH_DIR}/include )
    link_directories(${BENCH_DIR}/lib)
    message("${BENCH_DIR}/include")
ENDIF ()

include_directories("${BUILD_DIR}/include")

file(GLOB_RECURSE SOURCE_FILES src/*.cpp tests/*.cpp)
file(GLOB_RECURSE HEADER_FILES include/*.h)

add_executable(Allocator main.cpp ${SOURCE_FILES} ${HEADER_FILES} )
IF (DEFINED ENV{BENCH_DIR})
    target_link_libraries(Allocator benchmark)
ENDIF ()




