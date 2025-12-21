#ifndef CARD_H
#define CARD_H

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>

enum class Color { BROWN, GREY, BLUE, YELLOW, RED, GREEN, PURPLE };
enum class Resource { WOOD, BRICK, ORE, STONE, GLASS, CLOTH, PAPYRUS, SHIELD, SCIENCE_COMPASS, SCIENCE_GEAR, SCIENCE_TABLET, COIN, VP };

class Player;  // 前向声明
class Game;    // 前向声明

class Card {
public:
    std::string name;
    int age;
    Color color;
    std::map<Resource, int> cost;
    std::vector<std::string> chain_prerequisites;  // 前提卡牌名称
    std::string chain_provides;  // 提供的链式符号（例如 "COMPASS"）
    std::function<void(Player& self, Game& game)> effect;  // 效果 lambda（简化，多玩家版本忽略对手）

    Card(std::string n, int a, Color c, std::map<Resource, int> co, std::vector<std::string> pre, std::string pro);
    bool can_build_free(const Player& player) const;
};

// 工厂函数声明
std::vector<std::unique_ptr<Card>> createAllCards();

#endif // CARD_H