#ifndef CONSOLE_VIEW_H
#define CONSOLE_VIEW_H

#include <vector>
#include <string>


#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void clearScreen();

// 前向声明，减少物理依赖并加快编译速度
class CardStructure;
class Card;
class Player;
class Game;

/**
 * ConsoleView 类：负责将游戏逻辑状态渲染为控制台文本
 */
class ConsoleView {
public:
    // --- 结构显示 ---
    
    /**
     * 核心函数：根据当前时代（1, 2, 3）绘制不同的卡牌布局
     */
    void display_structure(const CardStructure& structure);

    // --- 状态显示 ---
    
    /**
     * 显示玩家面板：金币、胜利点、资源产出以及科技进度
     */
    void display_player_status(const Player& player);

    /**
     * 显示军事冲突条：绘制 0-18 的刻度并标出棋子位置
     */
    void display_board(const Game& game);

    /**
     * 通用消息提示：用于显示“建造成功”、“钱不够”等反馈
     */
    void display_message(const std::string& message);

private:
    /**
     * 内部辅助：格式化单个卡牌位置的显示
     * - 如果牌已取走：显示 "[   ]"
     * - 如果背面朝上：显示 "[ ? ]"
     * - 如果可选牌：显示 "*ABC*" (星号包围)
     * - 如果普通牌：显示 "[ABC]"
     */
    std::string format_card_slot(int pos, const CardStructure& s);
};

#endif // CONSOLE_VIEW_H