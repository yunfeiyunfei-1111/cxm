cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(cpp_engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Torch REQUIRED)

include_directories(${TORCH_INCLUDE_DIRS})

add_executable(cpp_engine d.cpp m.cpp l.cpp main.cpp)
target_link_libraries(cpp_engine "${TORCH_LIBRARIES}")
set_property(TARGET cpp_engine PROPERTY CXX_STANDARD 17)