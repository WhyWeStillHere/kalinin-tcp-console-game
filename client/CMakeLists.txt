cmake_minimum_required(VERSION 3.10)

project(client)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(client main.cpp client.h client.cpp terminal_manager.h terminal_manager.cpp painter.cpp painter.h screen_buffer.h screen_buffer.cpp interfaces/output_interface.h interfaces/output_interface.cpp interfaces/input_interface.h interfaces/input_interface.cpp interfaces/interface_manager.h interfaces/inteface_manager.cpp)