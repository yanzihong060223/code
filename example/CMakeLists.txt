cmake_minimum_required(VERSION 3.10)
project(muduo-example)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/../include)

add_executable(testserver testserver.cc)

target_link_libraries(testserver pthread)