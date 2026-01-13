#pragma once
#include <string>
#include <memory>
#include <vector>

// 前向声明，避免循环包含
class Game;
class Player;
class ConsoleView;
class Wonder;

/**
 * Controller 类：作为游戏的中枢神经
 * 核心职责：
 * 1. 接收 Game 层的状态并指令 View 层进行渲染。
 * 2. 处理用户输入，并将其转化为对 Game 层的逻辑操作。
 * 3. 处理由于奇迹或特定卡牌触发的特殊交互（如选进展标记）。
 */
class Controller {
public:
    // 构造函数：关联单例 Game 引用
    Controller(Game& g);
    
    // 必须定义析构函数，因为使用了 unique_ptr 指向一个前向声明的类
    ~Controller();

    // 禁止拷贝
    Controller(const Controller&) = delete;
    Controller& operator=(const Controller&) = delete;

    /**
     * 核心流程：处理一个玩家的回合
     * 内部逻辑：显示界面 -> 循环等待输入 -> 执行动作 -> 验证成功 -> 退出
     */
    void player_turn(Player& player);

    /**
     * 初始奇迹挑选阶段 (Draft Phase)
     * @param round 挑选轮次 (1 或 2)
     * @param pool 全部的奇迹池 (12个)，函数内部会根据轮次从中截取
     */
    void draft_wonders(int round, std::vector<Wonder>& pool);

    /**
     * UI 反馈：由 Game 逻辑层直接调用
     * 例如：显示“某某玩家赢得了军事压制获胜！”
     */
    void show_message(const std::string& msg);

private:
    Game& game;                       // 引用 Game 单例
    std::unique_ptr<ConsoleView> view; // 负责渲染的视图层
};