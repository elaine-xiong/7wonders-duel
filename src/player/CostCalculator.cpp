#include "CostCalculator.h"

int CostCalculator::calculateResourceCost(const Player& buyer, const Player& seller, ResourceType resource) {
    int baseCost = 2;
    int sellerResourceCount = seller.getResource(resource);
    return baseCost + sellerResourceCount;
}