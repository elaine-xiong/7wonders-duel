// #include <iostream>
// #include <map>
// #include <set>
// #include <string>
// #include "../cards/member2.h"

// enum class ResourceType {
//     Wood, Stone, Clay, Ore, Papyrus, Glass, Textile
// };

// class Player {
// private:
//     std::map<ResourceType, int> resources;   // 永久资源能力
//     std::map<Color, int> card_count;         // 各颜色卡数量
//     int coins;                               // 金币（唯一可消耗）

// public:
//     Player() : coins(7) {}

//     // ===== 资源 =====
//     void addResource(ResourceType r, int n = 1) {
//         resources[r] += n;
//     }

//     int getResource(ResourceType r) const {
//         auto it = resources.find(r);
//         return it == resources.end() ? 0 : it->second;
//     }

//     // ===== 金币 =====
//     int getCoins() const { return coins; }

//     bool spendCoins(int n) {
//         if (coins < n) return false;
//         coins -= n;
//         return true;
//     }

//     void addCoins(int n) {
//         coins += n;
//     }

//     // ===== 卡牌 =====
//     void addCard(Color c) {
//         card_count[c]++;
//     }

//     int getCardCount(Color c) const {
//         auto it = card_count.find(c);
//         return it == card_count.end() ? 0 : it->second;
//     }

//     int getResourceAmount(const ResourceType& resourceType) const {
//         auto it = resources.find(resourceType);
//         return it != resources.end() ? it->second : 0;
// 	}

//     int getCardCost() const {
//         try {
//             return card_count.at(Color::BROWN) / card_count.at(Color::GREY);
//         } catch (...) {
// 			return 0;
// 	}
// }};


// class Cost {
// public:
// 	static int calculateResourceCost(const Player& player, const Player& opponent, const ResourceType& resourceType) {
// 		// 基础费用 + 对手资源卡数量
// 		int baseCost = 2;
// 		int opponentCardCount = opponent.getCardCost();  // 假设对方卡数量即为其资源量
// 		return baseCost + opponentCardCount;
// 	}
// };

// class BuildingChain {
// private:
// 	std::set<std::string> builtBuildings;
// public:
// 	void buildBuilding(const std::string& building) {
// 		if (canBuild(building)) {
// 			builtBuildings.insert(building);
// 		}
// 	}

// 	bool canBuild(const std::string& building) const {
// 		// 如果需要的前置建筑已存在，则可以建造
// 		if (building == "Wooden Factory" && builtBuildings.count("Woodcutting") > 0) {
// 			return true;
// 		}
// 		return false;
// 	}
// };