cmake_minimum_required(VERSION 3.10)

project(game_server)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread")

add_executable(game_server server.cpp server.h main.cpp io_context.h io_context.cpp ../lib/send_command.h ../lib/send_command_impl.h room.h room.cpp)