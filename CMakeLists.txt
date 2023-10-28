cmake_minimum_required(VERSION 3.0.0)
project(mymalloc VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wundef -Wshadow -Wpointer-arith -Wcast-align -pedantic -O3")

include(CTest)
enable_testing()

add_executable(mymalloc main.c mymalloc.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
