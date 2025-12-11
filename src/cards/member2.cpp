#include <string>
#include <map>
#include <vector>
#include <functional>  // 用于Lambda表达式形式的效果实现
#include "player/game.h"

// 卡牌颜色枚举
enum class Color { BROWN, GREY, BLUE, YELLOW, RED, GREEN, PURPLE };
// 资源/符号枚举
enum class Resource { CLAY, WOOD, STONE, GLASS, PAPYRUS, COIN, SHIELD,
    SYMBOL_COMPASS, SYMBOL_TABLET, SYMBOL_GEAR /* 其他符号 */ };

class Card {
public:
    std::string name;          // 卡牌名称
    int age;                   // 所属时代
    Color color;               // 卡牌颜色
    std::map<Resource, int> cost;  // 建造成本（如：{WOOD, 2} 表示需2木材）
    std::vector<std::string> chain_prerequisites;  // 连锁前置卡牌名称（满足任一即可免费建造）
    std::string chain_provides;  // 该卡牌可解锁的连锁卡牌
    bool is_face_up = true;    // 是否正面朝上
    // 卡牌效果（Lambda表达式，接收当前玩家、对手玩家和游戏实例）
    std::function<void(Player& self, Player& opponent, Game& game)> effect;

    // 构造函数
    Card(std::string n, int a, Color c, std::map<Resource, int> co,
         std::vector<std::string> pre, std::string pro)
            : name(n), age(a), color(c), cost(co),
              chain_prerequisites(pre), chain_provides(pro) {}

    // 检查是否可免费建造（满足任一前置卡牌条件）
    bool can_build_free(const Player& player) const {
        for (const auto& pre : chain_prerequisites) {
            if (player.has_card(pre)) return true;
        }
        return false;
    }
};

// 可按颜色/效果派生或特化类（如绿色科技卡：向玩家的科技集合添加符号）
class Wonder {
public:
    std::string name;  // 奇迹名称
    std::map<Resource, int> cost;  // 建造成本
    // 奇迹效果（如：额外回合：g.set_extra_turn(true)）
    std::function<void(Player& self, Player& opponent, Game& game)> effect;

    // 构造函数
    Wonder(std::string n, std::map<Resource, int> co, auto eff)
            : name(n), cost(co), effect(eff) {}
};

#include <vector>
#include <set>
#include <map>
#include <memory>  // 用于std::unique_ptr

class CardStructure {
private:
    std::vector<std::unique_ptr<Card>> cards;  // 存储20张卡牌（智能指针管理）
    std::map<int, std::vector<int>> covers;    // 覆盖关系映射：位置i覆盖的卡牌列表（选取后解锁）
    std::set<int> accessible;                  // 当前可获取的卡牌位置
    int current_age;                           // 当前时代

    // 按时代初始化依赖关系
    void setup_dependencies(int age) {
        if (age == 1) {
            covers[0] = {5, 6};  // 底部0号位置解锁5号和6号位置
            // ... 按规则图示填充所有20个位置的依赖关系
            accessible = {0,1,2,3,4};  // 初始可获取：底部5张卡牌
        }
        // 第二时代（倒金字塔）、第三时代（混合模式）同理
    }

public:
    // 构造函数：接收时代和洗牌后的卡牌组
    CardStructure(int age, std::vector<std::unique_ptr<Card>> deck)
            : current_age(age), cards(std::move(deck)) {
        setup_dependencies(age);
        // 按时代模式设置卡牌正反面（如第一时代：底部5张朝上，上一层4张朝下等）
    }

    // 获取当前可获取的卡牌位置
    std::vector<int> get_accessible() const {
        return std::vector<int>(accessible.begin(), accessible.end());
    }

    // 选取卡牌：返回智能指针，解锁被覆盖的卡牌
    std::unique_ptr<Card> take_card(int pos) {
        if (accessible.find(pos) == accessible.end()) {
            throw std::runtime_error("卡牌不可获取");
        }
        auto card = std::move(cards[pos]);
        accessible.erase(pos);
        // 解锁当前卡牌覆盖的所有卡牌
        for (int unlocked : covers[pos]) {
            accessible.insert(unlocked);
            if (!cards[unlocked]->is_face_up) {
                cards[unlocked]->is_face_up = true;  // 展示卡牌
            }
        }
        return card;
    }

    // 检查卡牌结构是否为空（简化版）
    bool is_empty() const {
        return accessible.empty() && cards.empty();
    }

    // 辅助方法：获取当前时代（供界面展示）
    int get_age() const { return current_age; }

    // 辅助方法：获取指定位置的卡牌（供界面展示和AI判断）
    const Card* get_card(int pos) const {
        return cards[pos].get();
    }
};