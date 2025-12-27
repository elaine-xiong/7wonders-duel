#pragma once
#include "../cards/CardStructure.h"
#include "../cards/Card.h"           // 包含 Card 的定义
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>  // 用于 std::find

class CardStructure;  // 前向声明 CardStructure 类
class Card;           // 前向声明 Card 类

class ConsoleView {
public:
    // 展示卡牌金字塔结构
    void display_structure(const CardStructure& structure, const std::vector<int>& accessible);

private:
    // 格式化卡牌显示（背面朝上显示"???"，可获取卡牌前缀加"*"）
    std::string format_card(int pos, const CardStructure& s, const std::vector<int>& acc);
};