#include "Player.h"
#include <algorithm>

// 构造函数

Player::Player(const std::string& playerName, PlayerType playerType) 
    : name(playerName), type(playerType), coins(7), militaryTokens(0), victoryPoints(0) {
    // 初始化玩家，起始金币为 7（根据游戏规则）
}

// 资源管理 

void Player::addResource(Resource resource, int amount) {
    if (amount > 0) {
        resources[resource] += amount;
    }
}

int Player::getResource(Resource resource) const {
    auto it = resources.find(resource);
    return it != resources.end() ? it->second : 0;
}

bool Player::spendResource(Resource resource, int amount) {
    if (getResource(resource) < amount) {
        return false;  // 资源不足
    }
    resources[resource] -= amount;
    return true;
}

bool Player::hasEnoughResource(Resource resource, int amount) const {
    return getResource(resource) >= amount;
}

// 金币管理 

int Player::getCoins() const {
    return coins;
}

bool Player::spendCoins(int amount) {
    if (coins < amount) {
        return false;  // 金币不足
    }
    coins -= amount;
    return true;
}

void Player::addCoins(int amount) {
    if (amount > 0) {
        coins += amount;
    }
}

// 卡牌管理 

void Player::addBuiltCard(const std::string& cardName, Color cardColor) {
    builtCards.push_back(cardName);
    cardsByColor[cardColor]++;
}

bool Player::hasBuilding(const std::string& cardName) const {
    return std::find(builtCards.begin(), builtCards.end(), cardName) != builtCards.end();
}

const std::vector<std::string>& Player::getBuiltCards() const {
    return builtCards;
}

int Player::getCardCountByColor(Color color) const {
    auto it = cardsByColor.find(color);
    return it != cardsByColor.end() ? it->second : 0;
}

// 资源产出卡统计（用于交易成本计算）

void Player::addResourceProducingCard(Resource resource) {
    resourceProducingCards[resource]++;
}

int Player::getResourceProducingCardCount(Resource resource) const {
    auto it = resourceProducingCards.find(resource);
    return it != resourceProducingCards.end() ? it->second : 0;
}

// 建造逻辑 

bool Player::canAffordCard(const std::map<Resource, int>& cost) const {
    // 检查玩家是否拥有足够资源建造卡牌（不考虑交易）
    for (const auto& [resource, amount] : cost) {
        if (resource == Resource::COIN) {
            if (coins < amount) return false;
        } else {
            if (getResource(resource) < amount) return false;
        }
    }
    return true;
}

bool Player::canBuildFreeByChain(const std::vector<std::string>& chainPrerequisites) const {
    // 检查是否满足连锁建造条件（拥有任意一个前置建筑即可）
    if (chainPrerequisites.empty()) return false;
    
    for (const auto& prerequisite : chainPrerequisites) {
        if (hasBuilding(prerequisite)) {
            return true;  // 满足连锁条件，可以免费建造
        }
    }
    return false;
}

bool Player::buildCard(const std::map<Resource, int>& cost) {
    // 先检查是否能支付
    if (!canAffordCard(cost)) {
        return false;
    }
    
    // 扣除资源和金币
    for (const auto& [resource, amount] : cost) {
        if (resource == Resource::COIN) {
            spendCoins(amount);
        } else {
            spendResource(resource, amount);
        }
    }
    
    return true;
}

//军事 

void Player::addMilitaryTokens(int amount) {
    militaryTokens += amount;
}

int Player::getMilitaryTokens() const {
    return militaryTokens;
}

bool Player::hasMilitaryVictory() const {
    // 军事胜利条件：军事标记达到某个阈值（根据规则，通常是己方标记到达对手首都）
    // 这里假设阈值为 9
    return militaryTokens >= 9 || militaryTokens <= -9;
}

//科技

void Player::addScienceSymbol(Resource symbol) {
    // 只添加科技符号
    if (symbol == Resource::SCIENCE_COMPASS || 
        symbol == Resource::SCIENCE_GEAR || 
        symbol == Resource::SCIENCE_TABLET) {
        scienceSymbols.insert(symbol);
    }
}

const std::set<Resource>& Player::getScienceSymbols() const {
    return scienceSymbols;
}

bool Player::hasScienceVictory() const {
    // 科技胜利条件1：收集6个不同的科技符号（每种各2个）
    if (scienceSymbols.size() >= 6) {
        return true;
    }
    
    // 科技胜利条件2：收集3对相同的科技符号（需要更详细的实现）
    // 这里简化处理，实际需要统计每种科技符号的数量
    // 可以在建造时维护一个 map<Resource, int> 来统计
    
    return false;
}

// 胜利点数 

void Player::addVictoryPoints(int amount) {
    victoryPoints += amount;
}

int Player::getVictoryPoints() const {
    return victoryPoints;
}

int Player::calculateFinalScore() const {
    // 最终得分 = 胜利点数 + 金币/3
    return victoryPoints + (coins / 3);
}
