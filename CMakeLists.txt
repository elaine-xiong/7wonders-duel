cmake_minimum_required(VERSION 3.10)

# 项目名称
project(7wonders-duel VERSION 1.0)

# 设置 C++ 标准为 C++17 (支持更多现代特性，如 std::optional, std::variant)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含头文件的路径，设置为 src 目录
# 这样在代码中引用头文件时，可以直接写 #include "core/Game.h"
include_directories(src)

# 自动查找 src 目录下所有的 .cpp 和 .h 文件
# 注意：如果有新成员添加了新文件，通常需要重新运行 cmake .. 命令
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

# 生成可执行文件
# 可执行文件名为 7WondersDuel，由上面找到的所有源文件编译而成
add_executable(7WondersDuel ${SOURCES} ${HEADERS})

# 如果是 Windows 环境，尝试解决中文乱码问题 (可选)
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0600)
endif()

if(MSVC)
    add_compile_options(/utf-8)
endif()
