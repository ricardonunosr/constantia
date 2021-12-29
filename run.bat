@echo off

rem run with argument "vs" if you want to start visual studio to debug

IF "%1"=="vs" (
    devenv build\Debug\Example.exe
) ELSE (
    pushd build
    START /D ..\data Debug\Example.exe
    popd
)