#include <iostream>
#include <map>
#include <set>
#include <string>

//熊在12.7把money改成了coins

class Player {
private:
    std::map<std::string, int> resources;  // 资源字典
    int coins;  // 钱数，或者某种代币
public:
    // 构造函数初始化玩家资源
    Player() {
        resources["wood"] = 0;
        resources["stone"] = 0;
        resources["coins"] = 7; // 默认初始钱数
    }
    
    // 增加资源
    void addResource(const std::string& resource, int amount) {
        resources[resource] += amount;
    }
    // 减少资源
    bool spendResource(const std::string& resource, int amount) {
        if (resources[resource] >= amount) {
            resources[resource] -= amount;
            return true;
        }
        return false;  // 如果资源不足，返回 false
    }

    // 查询资源数量
    int getResourceAmount(const std::string& resource) const {
        return resources.at(resource);
    }
    void updateResources() {
        // 假设黄卡产生 1 木材
        addResource("wood", 1);  
        // 可根据实际规则进一步修改
    }
};

class Cost {
public:
    static int calculateResourceCost(const Player& player, const Player& opponent, const std::string& resourceType) {
        // 基础费用 + 对手资源卡数量
        int baseCost = 2;
        int opponentCardCount = opponent.getResourceAmount(resourceType);  // 假设对方卡数量即为其资源量
        return baseCost + opponentCardCount;
    }
};

class BuildingChain {
private:
    std::set<std::string> builtBuildings;
public:
    void buildBuilding(const std::string& building) {
        if (canBuild(building)) {
            builtBuildings.insert(building);
        }
    }

    bool canBuild(const std::string& building) const {
        // 如果需要的前置建筑已存在，则可以建造
        if (building == "Wooden Factory" && builtBuildings.count("Woodcutting") > 0) {
            return true;
        }
        return false;
    }
};