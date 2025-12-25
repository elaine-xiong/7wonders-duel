#ifndef COSTCALCULATOR_H
#define COSTCALCULATOR_H

#include "Player.h"
#include <map>
#include <vector>

/**
 * CostCalculator 类：计算建造成本和交易成本
 * 职责：经济系统核心逻辑
 * 
 * 核心功能：
 * 1. 计算交易成本：购买资源费用 = 2 + 对手拥有该类褐色/灰色卡数量
 * 2. 计算总建造成本：包括自有资源、需要购买的资源、金币成本
 * 3. 判断是否能建造卡牌
 */
class CostCalculator {
public:
    /**
     * 计算从对手购买单个资源的成本
     * @param buyer 购买方玩家
     * @param seller 出售方玩家（对手）
     * @param resource 资源类型
     * @return 购买该资源的金币成本
     * 
     * 规则：购买资源费用 = 2 + 对手拥有该类褐色/灰色卡数量
     */
    static int calculateTradeCost(const Player& buyer, const Player& seller, Resource resource);

    /**
     * 计算建造卡牌的总成本（包括需要购买的资源）
     * @param player 玩家
     * @param opponent 对手
     * @param cardCost 卡牌成本
     * @return 一个结构体，包含是否能建造、需要的金币、资源缺口等信息
     */
    struct BuildCostResult {
        bool canBuild;                          // 是否能建造
        int totalCoinCost;                      // 总金币成本
        std::map<Resource, int> missingResources;  // 缺少的资源
        std::map<Resource, int> resourcesToBuy;    // 需要购买的资源
    };

    static BuildCostResult calculateBuildCost(
        const Player& player, 
        const Player& opponent, 
        const std::map<Resource, int>& cardCost
    );

    /**
     * 检查玩家是否能建造卡牌（考虑交易）
     * @param player 玩家
     * @param opponent 对手
     * @param cardCost 卡牌成本
     * @return 是否能建造
     */
    static bool canAffordWithTrade(
        const Player& player, 
        const Player& opponent, 
        const std::map<Resource, int>& cardCost
    );

    /**
     * 执行建造（扣除资源和金币）
     * @param player 玩家
     * @param opponent 对手
     * @param cardCost 卡牌成本
     * @return 是否成功
     */
    static bool executeBuild(
        Player& player, 
        const Player& opponent, 
        const std::map<Resource, int>& cardCost
    );

private:
    //判断资源是否为褐色/灰色资源（可交易资源）
    static bool isTradableResource(Resource resource);
};

#endif // COSTCALCULATOR_H
