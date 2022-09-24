cmake_minimum_required(VERSION 3.21.1)
set(LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(Constantia LANGUAGES ${LANGUAGES})

#GLFW additions
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(SOURCES
        src/main.cpp
        src/application.cpp
        src/shader.cpp
        src/types.cpp
        src/vertex_array.cpp
        src/vertex_buffer.cpp
        src/index_buffer.cpp
        src/texture.cpp
        src/camera.cpp
        src/model.cpp
        src/window.cpp
        src/editor.cpp
        # Examples
        src/examples/sponza/sponza.cpp
        src/examples/framebuffers/framebuffers.cpp
        #src/examples/ray-tracing/ray_tracing.cpp
        )

set(HEADERS
        include/constantia/application.h
        include/constantia/camera.h
        include/constantia/core.h
        include/constantia/types.h
        include/constantia/editor.h
        include/constantia/index_buffer.h
        include/constantia/model.h
        include/constantia/shader.h
        include/constantia/texture.h
        include/constantia/vertex_array.h
        include/constantia/vertex_buffer.h
        include/constantia/vertex_buffer_layout.h
        include/constantia/window.h
        include/constantia/layer.h
        # Examples
        src/examples/sponza/sponza.h
        src/examples/framebuffers/framebuffers.h
        #src/examples/ray-tracing/ray_tracing.h
        )

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

add_subdirectory(lib/glad)
add_subdirectory(lib/glfw)
add_subdirectory(lib/spdlog)
add_subdirectory(lib/imgui)
add_subdirectory(lib/tinyobjloader)

target_include_directories(${PROJECT_NAME}
        PUBLIC include/constantia
        PUBLIC lib/glad/include
        PUBLIC lib/glfw/include
        PUBLIC lib/spdlog/include
        PUBLIC lib/glm
        PUBLIC lib/stb
        PUBLIC lib/imgui
        PUBLIC lib/tinyobjloader/tiny_obj_loader.h
        )

target_link_libraries(${PROJECT_NAME}
        glad
        glfw
        spdlog
        imgui
        tinyobjloader
        pthread
        )
