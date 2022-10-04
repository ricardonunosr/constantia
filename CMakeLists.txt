cmake_minimum_required(VERSION 3.21.1)
set(LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(Constantia LANGUAGES ${LANGUAGES})

if (MSVC)
    # warning level 4 and all warnings as errors
    add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror -Wno-unused-parameter -Wno-write-strings -Wno-unused-variable)
endif()

#GLFW additions
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(SOURCES
        src/main.cpp
        src/types.cpp
        src/camera.cpp
        src/model.cpp
        src/opengl_renderer.cpp
        )

set(HEADERS
        src/camera.h
        src/types.h
        src/model.h
        src/layer.h
        src/opengl_renderer.h
        # Examples
        src/examples/sponza/sponza.h
        #src/examples/framebuffers/framebuffers.h
        )

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

add_subdirectory(lib/glad)
add_subdirectory(lib/glfw)
add_subdirectory(lib/spdlog)
add_subdirectory(lib/imgui)
add_subdirectory(lib/tinyobjloader)

target_include_directories(${PROJECT_NAME}
        PUBLIC src/
        )

target_include_directories(${PROJECT_NAME} SYSTEM
        PUBLIC lib/glad/include
        PUBLIC lib/glfw/include
        PUBLIC lib/spdlog/include
        PUBLIC lib/glm
        PUBLIC lib/stb
        PUBLIC lib/imgui
        PUBLIC lib/tinyobjloader
        )

target_link_libraries(${PROJECT_NAME}
        glad
        glfw
        spdlog
        imgui
        tinyobjloader
        pthread
        )

# TODO(ricardo): I HATE CMAKE... find better way to add third-party libs
# TODO(ricardo): Remove spdlog
target_compile_options(glad PRIVATE "-w")
target_compile_options(glfw PRIVATE "-w")
target_compile_options(spdlog PRIVATE "-w")
target_compile_options(imgui PRIVATE "-w")
target_compile_options(tinyobjloader PRIVATE "-w")