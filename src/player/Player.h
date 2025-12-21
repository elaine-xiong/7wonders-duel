#ifndef PLAYER_H
#define PLAYER_H

#include <map>
#include "ResourceType.h"

class Player {
private:
    std::map<ResourceType, int> resources;   // 玩家资源
    int coins;                               // 金币

public:
    Player();

    // 资源管理
    void addResource(ResourceType resource, int amount);
    int getResource(ResourceType resource) const;

    // 金币管理
    int getCoins() const;
    bool spendCoins(int amount);
    void addCoins(int amount);
};

#endif // PLAYER_H