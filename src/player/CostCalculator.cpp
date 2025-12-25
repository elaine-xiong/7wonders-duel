#include "CostCalculator.h"
#include <algorithm>

// 辅助函数

bool CostCalculator::isTradableResource(Resource resource) {
    // 褐色资源：木头、砖块、矿石、石头
    // 灰色资源：玻璃、布料、纸莎草
    return resource == Resource::WOOD || 
           resource == Resource::BRICK || 
           resource == Resource::ORE || 
           resource == Resource::STONE ||
           resource == Resource::GLASS || 
           resource == Resource::CLOTH || 
           resource == Resource::PAPYRUS;
}

//交易成本计算

int CostCalculator::calculateTradeCost(const Player& buyer, const Player& seller, Resource resource) {
    // 基础成本为 2
    int baseCost = 2;
    
    // 对手拥有该类资源的产出卡数量
    int sellerCardCount = seller.getResourceProducingCardCount(resource);
    
    // 最终成本 = 2 + 对手该类卡数量
    return baseCost + sellerCardCount;
}

// 建造成本计算

CostCalculator::BuildCostResult CostCalculator::calculateBuildCost(
    const Player& player, 
    const Player& opponent, 
    const std::map<Resource, int>& cardCost
) {
    BuildCostResult result;
    result.canBuild = true;
    result.totalCoinCost = 0;

    // 遍历卡牌所需的所有资源
    for (const auto& [resource, requiredAmount] : cardCost) {
        // 如果是金币成本，直接累加
        if (resource == Resource::COIN) {
            result.totalCoinCost += requiredAmount;
            continue;
        }

        // 检查玩家拥有的资源数量
        int ownedAmount = player.getResource(resource);
        
        if (ownedAmount >= requiredAmount) {
            // 资源足够，无需购买
            continue;
        }

        // 资源不足，计算缺口
        int shortage = requiredAmount - ownedAmount;
        result.missingResources[resource] = shortage;

        // 检查是否可以交易购买
        if (isTradableResource(resource)) {
            // 可以从对手购买
            int tradeCostPerUnit = calculateTradeCost(player, opponent, resource);
            int totalTradeCost = tradeCostPerUnit * shortage;
            
            result.resourcesToBuy[resource] = shortage;
            result.totalCoinCost += totalTradeCost;
        } else {
            // 不可交易资源（如科技符号、军事等），无法建造
            result.canBuild = false;
            return result;
        }
    }

    // 检查玩家是否有足够金币支付
    if (player.getCoins() < result.totalCoinCost) {
        result.canBuild = false;
    }

    return result;
}

// 检查是否能建造

bool CostCalculator::canAffordWithTrade(
    const Player& player, 
    const Player& opponent, 
    const std::map<Resource, int>& cardCost
) {
    BuildCostResult result = calculateBuildCost(player, opponent, cardCost);
    return result.canBuild;
}

// 执行建造 

bool CostCalculator::executeBuild(
    Player& player, 
    const Player& opponent, 
    const std::map<Resource, int>& cardCost
) {
    // 先计算成本
    BuildCostResult result = calculateBuildCost(player, opponent, cardCost);
    
    if (!result.canBuild) {
        return false;  // 无法建造
    }

    // 扣除自有资源
    for (const auto& [resource, requiredAmount] : cardCost) {
        if (resource == Resource::COIN) {
            continue;  // 金币稍后统一扣除
        }

        int ownedAmount = player.getResource(resource);
        int amountToSpend = std::min(ownedAmount, requiredAmount);
        
        if (amountToSpend > 0) {
            player.spendResource(resource, amountToSpend);
        }
    }

    // 扣除金币（包括卡牌金币成本和交易成本）
    if (result.totalCoinCost > 0) {
        if (!player.spendCoins(result.totalCoinCost)) {
            return false;  // 金币不足（理论上不应该发生）
        }
    }

    return true;
}
