cmake_minimum_required(VERSION 3.22)

project(ucr VERSION 0.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OBJCXX_STANDARD_REQUIRED 17)

file(GLOB SOURCES "../src/*.cpp")

add_executable(ucr ${SOURCES})
