#include "Board.h"
#include "../player/Player.h"
#include <cmath>
//完善惩罚逻辑

Board::Board() {
    conflictPawnPosition = 9; // 初始在中间 
    for(int i=0; i<4; i++) militaryTokens[i] = true; // 4个标记都在
}

bool Board::movePawn(int amount, Player& p1, Player& p2) {
    conflictPawnPosition += amount;
    
    // 限制边界
    if (conflictPawnPosition < 0) conflictPawnPosition = 0;
    if (conflictPawnPosition > 18) conflictPawnPosition = 18;

    // 检查 Looting 惩罚 (只在第一次跨越时触发)
    // 向 P1 首都移动 (amount < 0)，检查 P1 侧标记
    if (amount < 0) {
        if (conflictPawnPosition <= 6 && militaryTokens[0]) {
            p1.loseCoins(2); militaryTokens[0] = false;
            std::cout << "[Board] P1 lost 2 coins due to military pressure!\n";
        }
        if (conflictPawnPosition <= 3 && militaryTokens[1]) {
            p1.loseCoins(5); militaryTokens[1] = false;
            std::cout << "[Board] P1 lost 5 coins due to military pressure!\n";
        }
    } 
    // 向 P2 首都移动 (amount > 0)，检查 P2 侧标记
    else if (amount > 0) {
        if (conflictPawnPosition >= 12 && militaryTokens[2]) {
            p2.loseCoins(2); militaryTokens[2] = false;
            std::cout << "[Board] P2 lost 2 coins due to military pressure!\n";
        }
        if (conflictPawnPosition >= 15 && militaryTokens[3]) {
            p2.loseCoins(5); militaryTokens[3] = false;
            std::cout << "[Board] P2 lost 5 coins due to military pressure!\n";
        }
    }

    return (conflictPawnPosition == 0 || conflictPawnPosition == 18);
}

int Board::getPawnPosition() const {
    return conflictPawnPosition;
}

int Board::getMilitaryPoints(int playerIndex) const {
    int dist = (playerIndex == 0) ? (9 - conflictPawnPosition) : (conflictPawnPosition - 9);
    if (dist <= 0) return 0;
    if (dist >= 1 && dist <= 2) return 2;
    if (dist >= 3 && dist <= 5) return 5;
    if (dist >= 6) return 10;
    return 0;
}