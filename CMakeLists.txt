cmake_minimum_required(VERSION 4.0)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(extern/glad) # <-- new line

add_subdirectory(extern/glfw)

add_subdirectory(extern/glm)

add_subdirectory(game)
