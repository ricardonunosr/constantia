cmake_minimum_required(VERSION 3.21.1)
set(LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(Constantia LANGUAGES ${LANGUAGES})

#GLFW additions
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#ASSIMP
#set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF)
#set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF)
#set(ASSIMP_BUILD_OBJ_IMPORTER ON)

set(SOURCES 
    src/main.cpp
    src/application.cpp
    src/shader.cpp
    src/vertex_array.cpp
    src/vertex_buffer.cpp
    src/index_buffer.cpp
    src/texture.cpp
    src/camera.cpp
    src/mesh.cpp
    src/model.cpp
    src/window.cpp
    src/renderer.cpp
    src/editor.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(lib/glad)
add_subdirectory(lib/glfw)
add_subdirectory(lib/spdlog)
add_subdirectory(lib/imgui)
add_subdirectory(lib/assimp)

target_include_directories(${PROJECT_NAME}
    PUBLIC include/constantia
    PUBLIC lib/glad/include
    PUBLIC lib/glfw/include
    PUBLIC lib/spdlog/include
    PUBLIC lib/glm
    PUBLIC lib/stb
    PUBLIC lib/imgui
    PUBLIC lib/assimp/include/assimp
)

target_link_libraries(${PROJECT_NAME} 
        glad
        glfw
        spdlog
        imgui
        assimp
)