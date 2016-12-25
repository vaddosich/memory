cmake_minimum_required(VERSION 3.2)
project(Allocator)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -g -ggdb -pthread")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -O2 -pthread")
#-fsanitize=thread

add_definitions(-D_LINUX)

IF (NOT DEFINED ENV{BUILD_DIR})
    set(BUILD_DIR ".")
ELSE()
    set(BUILD_DIR $ENV{BUILD_DIR})
ENDIF ()

include_directories("${BUILD_DIR}/include"
                    /home/vad/Libs/benchmark/include
                        )

file(GLOB_RECURSE SOURCE_FILES src/*.cpp tests/*.cpp)
file(GLOB_RECURSE HEADER_FILES include/*.h)



link_directories("/usr/lib64"
                  /home/vad/Libs/builds/bench/src/src)


add_executable(Allocator main.cpp ${SOURCE_FILES} ${HEADER_FILES} )
target_link_libraries(Allocator benchmark)



