# Constantia

This project is my personal general game engine to learn graphics and game engine related stuff. It's named constantia cause i intent to be working on it constantly and not abandon it ( long term ). This project is originated from my first project in game engine development named [Valiance](https://github.com/ricardonunosr/valiance) that was very much copied/inspired from [Hazel](https://github.com/TheCherno/Hazel) and i wanted to start fresh and relearn the basics.

Requeriments
- [CMake](https://cmake.org/)

Dependencies
- [glfw](https://github.com/glfw/glfw)
- [assimp](https://github.com/assimp/assimp)
- [glad2](https://github.com/Dav1dde/glad/tree/glad2)
- [glm](https://github.com/g-truc/glm)
- [imgui](https://github.com/ocornut/imgui)
- [spdlog](https://github.com/gabime/spdlog)
- [stb](https://github.com/nothings/stb)

# Getting started

- Clone the repo
```bash
git clone --recursive https://github.com/ricardonunosr/constantia
```
- If you cloned non-recursively
```bash
git submodule update --init
```
- Change to the directory
```bash
cd constantia
```
- Use CMake to build repo
```bash
mkdir build
cmake -S . -B build
cmake --build build
./build/Constantia
```
