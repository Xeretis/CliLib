cmake_minimum_required(VERSION 3.17)
project(CliApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXE_LINKER_FLAGS -static)

add_executable(${PROJECT_NAME} main.cpp)

add_subdirectory(CliLib)
target_include_directories(${PROJECT_NAME} PRIVATE CliLib/include)
target_link_libraries(${PROJECT_NAME} CliLib)