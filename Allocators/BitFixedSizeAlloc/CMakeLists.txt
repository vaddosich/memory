cmake_minimum_required(VERSION 3.2)
project(Allocator)

set(CMAKE_CXX_FLAGS_RELEASE "-std=c++14 -O2 -pthread")
#set(CMAKE_CXX_FLAGS_RELEASE "-std=c++14 -g -ggdb -pthread")
set(CMAKE_CXX_FLAGS_DEBUG  "-std=c++14 -g -ggdb -pthread -fsanitize=address ")

IF (NOT DEFINED ENV{BUILD_DIR})
    set(BUILD_DIR ".")
ELSE()
    set(BUILD_DIR $ENV{BUILD_DIR})
ENDIF ()

IF ("$ENV{BENCH}" STREQUAL "y" AND "$ENV{GTEST}" STREQUAL "y")
    message( FATAL_ERROR "You can set only one - gtest or benchmark." )
ENDIF ()

IF (DEFINED ENV{BENCH_DIR})
    set(BENCH_DIR $ENV{BENCH_DIR})
    IF ("$ENV{BENCH}" STREQUAL "y")
        add_definitions(-D_BENCHMARKS)
        include_directories(${BENCH_DIR}/include )
        link_directories(${BENCH_DIR}/lib)
    ENDIF ()
ENDIF ()


IF (DEFINED ENV{GTEST_DIR})
    set(GTEST_DIR $ENV{GTEST_DIR})
    IF ("$ENV{GTEST}" STREQUAL "y")
        add_definitions(-D_GTEST)
        include_directories(${GTEST_DIR}/include )
        link_directories(${GTEST_DIR}/lib)
    ENDIF ()
ENDIF ()

include_directories("${BUILD_DIR}/include"
                    "/home/vad/Libs/concurrentqueue")

file(GLOB_RECURSE SOURCE_FILES src/*.cpp tests/*.cpp)
file(GLOB_RECURSE HEADER_FILES include/*.h)

add_executable(Allocator main.cpp ${SOURCE_FILES} ${HEADER_FILES} )
IF ("$ENV{BENCH}" STREQUAL "y")
    target_link_libraries(Allocator benchmark)
ENDIF ()

IF ("$ENV{GTEST}" STREQUAL "y")
    target_link_libraries(Allocator gtest gtest_main)
ENDIF ()




