#include "member2.h"
#include "player/game.h"
#include <stdexcept>  // 用于std::runtime_error

// Card类实现
Card::Card(std::string n, int a, Color c, std::map<Resource, int> co,
           std::vector<std::string> pre, std::string pro)
        : name(n), age(a), color(c), cost(co),
          chain_prerequisites(pre), chain_provides(pro) {}

bool Card::can_build_free(const Player& player) const {
    for (const auto& pre : chain_prerequisites) {
        if (player.has_card(pre)) return true;
    }
    return false;
}

// Wonder类实现
Wonder::Wonder(std::string n, std::map<Resource, int> co,
               const std::function<void(Player&, Player&, Game&)>& eff)
        : name(n), cost(co), effect(eff) {}

// CardStructure类实现
void CardStructure::setup_dependencies(int age) {
    if (age == 1) {
        covers[0] = {5, 6};  // 底部0号位置解锁5号和6号位置
        // ... 按规则图示填充所有20个位置的依赖关系
        accessible = {0,1,2,3,4};  // 初始可获取：底部5张卡牌
    }
    // 第二时代（倒金字塔）、第三时代（混合模式）同理
}

CardStructure::CardStructure(int age, std::vector<std::unique_ptr<Card>> deck)
        : current_age(age), cards(std::move(deck)) {
    setup_dependencies(age);
    // 按时代模式设置卡牌正反面（如第一时代：底部5张朝上，上一层4张朝下等）
}

std::vector<int> CardStructure::get_accessible() const {
    return std::vector<int>(accessible.begin(), accessible.end());
}

std::unique_ptr<Card> CardStructure::take_card(int pos) {
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

bool CardStructure::is_empty() const {
    return accessible.empty() && cards.empty();
}

int CardStructure::get_age() const {
    return current_age;
}

const Card* CardStructure::get_card(int pos) const {
    return cards[pos].get();
}