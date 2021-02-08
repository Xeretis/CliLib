cmake_minimum_required(VERSION 3.17)
project(CliLib)

set(CMAKE_CXX_STANDARD 17)

add_library(${PROJECT_NAME} STATIC src/Cli.cpp src/Args.cpp src/Command.cpp src/Utils.cpp)