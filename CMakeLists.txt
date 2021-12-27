cmake_minimum_required(VERSION 3.15)

project(Example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
# TODO(Ricardo): CMakePresets.json
set(CMAKE_BUILD_TYPE Debug)

add_executable(${PROJECT_NAME} src/main.cpp)

include_directories(${PROJECT_NAME}
    PUBLIC lib/glad/include/glad
    PUBLIC lib/glad/include/KHR 
    PUBLIC lib/glfw/include
)

target_link_libraries(${PROJECT_NAME}
    glad
)

add_subdirectory(lib/glad)