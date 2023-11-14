cmake_minimum_required(VERSION 3.22)

project(ucrp VERSION 1.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_OBJCXX_STANDARD_REQUIRED 17)

file(GLOB SOURCES "../src/*.cpp")

add_executable(ucrp ${SOURCES})
