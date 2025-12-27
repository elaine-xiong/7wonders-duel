#pragma once
#include <vector>
#include <memory>
#include <string>

// 前向声明，减少头文件相互包含导致的编译错误
class Board;
class Player;
class CardStructure;
class Controller;
class Card;

class Game {
private:
    // 1. 单例模式私有成员
    static Game* instance;
    Game(); // 私有构造函数

    // 2. 核心组件
    std::unique_ptr<Board> board;
    std::vector<std::shared_ptr<Player>> players;
    std::unique_ptr<CardStructure> cardStructure;
    
    // 3. 游戏状态变量
    int currentAge;
    int currentPlayerIdx;
    bool isGameOver;
    bool extraTurnTriggered; // 用于某些奇迹提供的“连续行动”效果

public:
    // 4. 单例获取接口
    static Game& getInstance();
    
    // 5. 禁止拷贝和赋值（单例模式标准操作）
    Game(const Game&) = delete;
    void operator=(const Game&) = delete;

    // 6. 生命周期管理
    void init(); // 初始化玩家、金币、初始时代布局
    void run();  // 游戏主循环
    void endAge(); // 时代切换逻辑（如Age 1结束进入Age 2）

    // 7. 回合逻辑处理
    void playTurn(Controller& controller);

    // 8. 核心游戏动作（供 Controller 调用）
    // 返回 bool 可以让 Controller 知道操作是否成功执行
    bool takeCard(int pos, Player& player);
    void buildWonder(int wonderIdx, int pos, Player& player);
    void discardForCoins(int pos, Player& player);

    // 9. 胜利条件检查
    bool checkSupremacyVictory(); // 检查军事压制或科技压制

    // 10. 辅助工具接口（Getter/Setter）
    Board* getBoard() { return board.get(); }
    Player* getCurrentPlayer();
    Player* getOpponent();
    CardStructure& getStructure() { return *cardStructure; } 
    
    int getCurrentAge() const { return currentAge; }
    void setIsGameOver(bool status) { isGameOver = status; }
    void setExtraTurn(bool status) { extraTurnTriggered = status; }

    void movePawn(int steps); // 执行军事推进，steps 永远为正，方向由函数内部根据当前玩家判断

    // 析构函数（单例清理）
    ~Game() = default;
};