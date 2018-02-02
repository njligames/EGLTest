
cmake_minimum_required(VERSION 3.10.0)
project(EGLTest CXX)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR})

find_package(OpenGL REQUIRED COMPONENTS OpenGL EGL)

add_executable(EGLTest main.cpp)
target_link_libraries(EGLTest PRIVATE OpenGL::OpenGL OpenGL::EGL)

