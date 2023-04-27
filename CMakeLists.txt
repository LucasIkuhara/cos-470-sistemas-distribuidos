cmake_minimum_required(VERSION 3.25)
project(distributed_systems)

set(CMAKE_CXX_STANDARD 17)

add_executable(distributed_systems_signals signals.cpp)
add_executable(distributed_systems_pipes pipes.cpp)
add_executable(distributed_systems_sockets sockets.cpp)
