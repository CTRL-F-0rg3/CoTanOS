cmake_minimum_required(VERSION 3.16)
project(loginscreen LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(loginscreen
    src/main.cpp
    src/glad/src/glad.c
)
target_include_directories(loginscreen PRIVATE src/glad/include src)
target_link_libraries(loginscreen PRIVATE glfw OpenGL::GL)