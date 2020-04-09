cmake_minimum_required(VERSION 3.10)

project(library)

add_library(library room_info.cpp room_info.h tcp_utilities.h tcp_connection.h tcp_utilities.cpp send_command.h send_command_impl.h player_info.h player_info.cpp)
