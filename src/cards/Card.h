#pragma once
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
    
    // --- 新增：存储该卡牌产出的资源列表（如木头、石头等） ---
    std::vector<Resource> producedResources;

    bool is_face_up = true; //添加:卡牌是否正面朝上，默认正面
    std::function<void(Player& self, Game& game)> effect;  // 效果 lambda（简化，多玩家版本忽略对手）
    // --- 更新构造函数，增加 producedResources 参数 ---
    Card(std::string n, int a, Color c, std::map<Resource, int> co, 
         std::vector<std::string> pre, std::string pro, 
         std::vector<Resource> res = {}) // 默认值为空
        : name(n), age(a), color(c), cost(co), 
          chain_prerequisites(pre), chain_provides(pro), 
          producedResources(res) {}

    Card(std::string n, int a, Color c, std::map<Resource, int> co, std::vector<std::string> pre, std::string pro);
    bool can_build_free(const Player& player) const;
};

// 工厂函数声明
std::vector<std::unique_ptr<Card>> createAllCards();
