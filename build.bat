call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64
cmake -G"Ninja" -DCMAKE_BUILD_TYPE=Debug -S . -B build
cmake --build ./build/
