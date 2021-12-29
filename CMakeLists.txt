cmake_minimum_required(VERSION 3.22.1)

project(Example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
# TODO(Ricardo): CMakePresets.json
set(CMAKE_BUILD_TYPE Debug)
#GLFW additions
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(SOURCES 
    src/main.cpp
    src/shader.cpp
    src/shader.h
    src/core.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(lib/glad)
add_subdirectory(lib/glfw)

target_include_directories(${PROJECT_NAME}
    PUBLIC lib/glad/include
    PUBLIC lib/glfw/include
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC 
        glad
        glfw
)



