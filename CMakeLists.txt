cmake_minimum_required(VERSION 3.21)
project(untitled)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(extern/glad) # <-- new line

add_subdirectory(extern/glfw)

add_subdirectory(extern/glm)

add_subdirectory(extern/box2d)

add_subdirectory(code/game)

add_subdirectory(extern/soloud)

add_subdirectory(extern/tinygltf)




