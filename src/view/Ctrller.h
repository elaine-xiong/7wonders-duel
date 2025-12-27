#pragma once
#include "../core/Game.h"         // 包含 Game 的定义
#include "../player/Player.h"       // 包含 Player 的定义
#include <iostream>
#include <sstream>
#include <string>

class ConsoleView;  // 前向声明 ConsoleView 类
class Game;         // 前向声明 Game 类
class Player;       // 前向声明 Player 类

class Controller {
private:
    ConsoleView view;
    Game& game;  // 引用成员1的Game类

public:
    Controller(Game& g);
    // 玩家回合处理
    void player_turn(Player& player);
};