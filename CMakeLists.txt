cmake_minimum_required(VERSION 3.10)

project(console_game)
cmake_policy(SET CMP0079 NEW)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)
add_subdirectory(server)
add_subdirectory(client)

target_link_libraries(game_server library)
target_link_libraries(client library)
