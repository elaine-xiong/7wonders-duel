@echo off
echo Compiling 7WondersDuel...
cl.exe /std:c++17 /EHsc /utf-8 /I src src\main.cpp src\cards\*.cpp src\core\*.cpp src\player\*.cpp src\view\*.cpp /Fe:7WondersDuel.exe
if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo Running game...
    .\7WondersDuel.exe
) else (
    echo Build failed.
)
