#pragma once

#include "Player.h"
#include "ResourceType.h"

class CostCalculator {
public:
    static int calculateResourceCost(const Player& buyer, const Player& seller, ResourceType resource);
};
