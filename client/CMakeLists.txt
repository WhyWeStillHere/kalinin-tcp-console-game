cmake_minimum_required(VERSION 3.10)

project(client)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(client main.cpp client.h client.cpp terminal_manager.h terminal_manager.cpp painter.cpp painter.h screen_buffer.h screen_buffer.cpp output_interface.h output_interface.cpp input_interface.h input_interface.cpp interface_manager.h inteface_manager.cpp)