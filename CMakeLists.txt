cmake_minimum_required(VERSION 3.12)
set(LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(Constantia LANGUAGES ${LANGUAGES})

if (MSVC)
    # warning level 4 and all warnings as errors
    # add_compile_options(/W4 /WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -ftime-trace -Wno-unused-parameter -Wno-write-strings -Wno-unused-variable)
endif()

#GLFW additions
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(SOURCES
        src/main.cpp
        )

set(HEADERS
        src/camera.h
        src/model.h
        src/opengl_renderer.h
        src/idk_math.h
        src/memory.h
        src/vendor/tiny_obj_loader.h
        src/vendor/stb_image.h
        )

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

add_subdirectory(lib/glad)
add_subdirectory(lib/glfw)
add_subdirectory(lib/imgui)

target_include_directories(${PROJECT_NAME}
        PUBLIC src/
        )

target_include_directories(${PROJECT_NAME} SYSTEM
        PUBLIC lib/glad/include
        PUBLIC lib/glfw/include
        PUBLIC lib/imgui
        )

target_link_libraries(${PROJECT_NAME}
        glad
        glfw
        imgui
        )

target_compile_options(glad PRIVATE "-w")
target_compile_options(glfw PRIVATE "-w")
target_compile_options(imgui PRIVATE "-w")
