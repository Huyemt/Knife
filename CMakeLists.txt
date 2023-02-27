cmake_minimum_required(VERSION 3.24)
project(Knife)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
include_directories(src/ src/front src/front/generator)
file(GLOB_RECURSE SRCS
        "src/main.cpp"
        "src/front/*.cpp"
        "src/front/generator/*.cpp")

add_executable(${PROJECT_NAME} ${SRCS})
