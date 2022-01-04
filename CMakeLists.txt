cmake_minimum_required(VERSION 3.21.1)

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
    src/camera.cpp
    src/camera.h
    src/vertex_buffer_layout.h
    src/vertex_buffer.h
    src/vertex_buffer.cpp
    src/vertex_array.h
    src/vertex_array.cpp
    src/texture.h
    src/texture.cpp
    src/index_buffer.h
    src/index_buffer.cpp
    src/mesh.h
    src/mesh.cpp
    src/model.h
    src/model.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(lib/glad)
add_subdirectory(lib/glfw)
add_subdirectory(lib/spdlog)
add_subdirectory(lib/imgui)
add_subdirectory(lib/assimp)

target_include_directories(${PROJECT_NAME}
    PUBLIC lib/glad/include
    PUBLIC lib/glfw/include
    PUBLIC lib/spdlog/include
    PUBLIC lib/glm
    PUBLIC lib/stb
    PUBLIC lib/imgui
    PUBLIC lib/assimp/include/assimp
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC 
        glad
        glfw
        spdlog
        imgui
        assimp
)



