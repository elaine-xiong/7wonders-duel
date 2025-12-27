#pragma once
#include <vector>
#include <iostream>
//定义冲突与惩罚

class Board {
private:
    // 冲突棋子的位置。
    // 假设 0 是左边玩家(P1)的首都，18 是右边玩家(P2)的首都，9 是初始中间位置。
    // 规则参考: 
    int conflictPawnPosition; 
    
    // 军事标记：[0,1]对应P1侧，[2,3]对应P2侧
    // 索引0: 2元(pos 6), 索引1: 5元(pos 3) | 索引2: 2元(pos 12), 索引3: 5元(pos 15)
    bool militaryTokens[4]; 

    // 场上的科技标记（Progress Tokens）。规则参考: 
    // 假设用简单的 int ID 或 enum 代表不同的科技
    std::vector<int> progressTokens;

public:
    Board();
    
    // 移动棋子。amount > 0 向右(P2)移动，amount < 0 向左(P1)移动
    // 返回 true 如果导致某方立即获胜（到达首都）
    // 规则参考: 
    bool movePawn(int amount, Player& p1, Player& p2);
    
    int getPawnPosition() const;
    
    // 获取当前的军事分数（游戏结束时用）
    // 规则参考: 
    int getMilitaryPoints(int playerIndex) const;

    // TODO: 添加管理科技标记的方法
};