#include <iostream>
#include <vector>
#include <string>

// 这是一个测试用的 main 函数
// 以后我们会在这里初始化 Game 类并启动游戏循环

int main() {
    // 设置控制台输出编码，防止乱码（根据需要调整）
    #ifdef _WIN32
        system("chcp 65001"); // 切换 Windows 控制台到 UTF-8
    #endif

    std::cout << "========================================" << std::endl;
    std::cout << "       7 Wonders Duel - LO02 Project    " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Environment check: Success!" << std::endl;
    std::cout << "C++ Standard Version: " << __cplusplus << std::endl;

    std::cout << "\nReady for team collaboration..." << std::endl;

    // 模拟等待用户输入，防止窗口一闪而过
    std::cout << "Press Enter to exit...";
    std::cin.get();

    return 0;
}
