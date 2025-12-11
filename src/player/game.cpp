#include "game.h"

void gameLoop() {
    // 玩家1和玩家2对象
    Player player1;
    Player player2;
    BuildingChain bc;

    // 玩家1的回合
    player1.addResource("wood", 3);  // 假设玩家1初始有 3 木材
    int cost = Cost::calculateResourceCost(player1, player2, "wood");
    if (player1.spendResource("wood", cost)) {
        std::cout << "Player 1 bought wood!" << std::endl;
    }

    // 检查是否能连锁建造
    if (bc.canBuild("Wooden Factory")) {
        bc.buildBuilding("Wooden Factory");
        std::cout << "Player 1 built a Wooden Factory!" << std::endl;
    }
}
