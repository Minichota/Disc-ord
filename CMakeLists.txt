cmake_minimum_required (VERSION 3.6)
project(disc-bot)

add_executable(disc-bot main.cpp disc.cpp parsing.cpp bag.cpp)

add_subdirectory(extern/sleepy-discord)
target_link_libraries(disc-bot sleepy-discord)
