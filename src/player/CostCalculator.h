#ifndef COSTCALCULATOR_H
#define COSTCALCULATOR_H

#include "Player.h"
#include "ResourceType.h"

class CostCalculator {
public:
    static int calculateResourceCost(const Player& buyer, const Player& seller, ResourceType resource);
};

#endif // COSTCALCULATOR_H