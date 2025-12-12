#pragma once
#include <string>
#include <map>
#include <vector>
#include <set>
#include <memory>
#include <functional>

// 前向声明
class Player;
class Game;

// 卡牌颜色枚举
enum class Color { BROWN, GREY, BLUE, YELLOW, RED, GREEN, PURPLE };

// 资源/符号枚举
enum class Resource {
    CLAY, WOOD, STONE, GLASS, PAPYRUS, COIN, SHIELD,
    SYMBOL_COMPASS, SYMBOL_TABLET, SYMBOL_GEAR /* 其他符号 */
};

class Card {
public:
    std::string name;          // 卡牌名称
    int age;                   // 所属时代
    Color color;               // 卡牌颜色
    std::map<Resource, int> cost;  // 建造成本
    std::vector<std::string> chain_prerequisites;  // 连锁前置卡牌名称
    std::string chain_provides;  // 该卡牌可解锁的连锁卡牌
    bool is_face_up = true;    // 是否正面朝上
    // 卡牌效果
    std::function<void(Player& self, Player& opponent, Game& game)> effect;

    // 构造函数
    Card(std::string n, int a, Color c, std::map<Resource, int> co,
         std::vector<std::string> pre, std::string pro);

    // 检查是否可免费建造
    bool can_build_free(const Player& player) const;
};

class Wonder {
public:
    std::string name;  // 奇迹名称
    std::map<Resource, int> cost;  // 建造成本
    // 奇迹效果
    std::function<void(Player& self, Player& opponent, Game& game)> effect;

    // 构造函数
    Wonder(std::string n, std::map<Resource, int> co,
           const std::function<void(Player&, Player&, Game&)>& eff);
};

class CardStructure {
private:
    std::vector<std::unique_ptr<Card>> cards;  // 存储20张卡牌
    std::map<int, std::vector<int>> covers;    // 覆盖关系映射
    std::set<int> accessible;                  // 当前可获取的卡牌位置
    int current_age;                           // 当前时代

    // 按时代初始化依赖关系
    void setup_dependencies(int age);

public:
    // 构造函数：接收时代和洗牌后的卡牌组
    CardStructure(int age, std::vector<std::unique_ptr<Card>> deck);

    // 获取当前可获取的卡牌位置
    std::vector<int> get_accessible() const;

    // 选取卡牌：返回智能指针，解锁被覆盖的卡牌
    std::unique_ptr<Card> take_card(int pos);

    // 检查卡牌结构是否为空
    bool is_empty() const;

    // 辅助方法：获取当前时代
    int get_age() const;

    // 辅助方法：获取指定位置的卡牌
    const Card* get_card(int pos) const;
};
