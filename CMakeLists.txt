cmake_minimum_required(VERSION 3.0.0)
project(server VERSION 0.1.0)
set(CMAKE_CXX_FLAGS  "-std=c++20  -Wall -Wextra -pedantic ${CMAKE_CXX_FLAGS}")

include_directories(./src/base/include)
add_subdirectory(./src/base)
find_library(base_lib REQUIRED)

include_directories(./src/net/include)
add_subdirectory(./src/net)
find_library(net_lib REQUIRED)


add_subdirectory(./example)

file(GLOB TEST_FILES "test/*.cc")

# foreach(TEST_FILE ${TEST_FILES})
#     message("FILE: ${TEST_FILE}")
#     get_filename_component(TEST_NAME ${TEST_FILE} NAME_WLE)
#     add_executable(${TEST_NAME} ${TEST_FILE})
#     target_link_libraries(${TEST_NAME} base_lib)
#     target_link_libraries(${TEST_NAME} net_lib)
# endforeach(TEST_FILE ${TEST_FILES})