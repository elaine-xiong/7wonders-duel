#include "CardStructure.h"
#include <stdexcept>

CardStructure::CardStructure(int age, std::vector<std::unique_ptr<Card>> deck) : current_age(age), cards(std::move(deck)) {
    if (cards.size() != 20) throw std::runtime_error("Deck must have 20 cards");
    setupDependenciesAndFaces(age);
}

void CardStructure::setupDependenciesAndFaces(int age) {
    // 硬编码每个时代布局（基于规则和截图）
    if (age == 1) {  // Age I: 正金字塔，底部5面朝上，下一层4面朝下，3上，2下，1上（总20? 实际是特定重叠图案，假设位置0-19）
        // 示例依赖: 底部0-4可访问
        accessible = {0,1,2,3,4};
        for (int i = 0; i < 5; ++i) cards[i]->is_face_up = true;
        // 解锁: 0解锁5,6; 1解锁6,7; 2解锁7,8; 3解锁8,9; 4解锁9,10
        unlocks[0] = {5,6};
        unlocks[1] = {6,7};
        unlocks[2] = {7,8};
        unlocks[3] = {8,9};
        unlocks[4] = {9,10};
        // 继续上层, 如5解锁11,12; ... 直到19
        // 面朝: 假设交替, 5-10面朝下, 11-15上, 等. 根据截图调整
        for (int i = 5; i < 11; ++i) cards[i]->is_face_up = false;
        // ... 完整硬编码所有
    } else if (age == 2) {  // Age II: 倒置金字塔
        accessible = {0,1,2};  // 假设顶部初始
        // 依赖倒置, 上解锁下
        unlocks[0] = {3,4};
        // ...
    } else if (age == 3) {  // Age III: 混合, 包含公会
        // 类似, 包括紫色公会
    }
}

std::vector<int> CardStructure::getAccessible() const {
    return std::vector<int>(accessible.begin(), accessible.end());
}

std::unique_ptr<Card> CardStructure::takeCard(int pos) {
    if (accessible.find(pos) == accessible.end()) throw std::runtime_error("Not accessible");
    auto card = std::move(cards[pos]);
    accessible.erase(pos);
    for (int unlocked : unlocks[pos]) {
        accessible.insert(unlocked);
        if (!cards[unlocked]->is_face_up) cards[unlocked]->is_face_up = true;  // 揭示
    }
    return card;
}

bool CardStructure::isEmpty() const {
    return cards.empty();
}

const Card* CardStructure::getCard(int pos) const {
    return cards[pos].get();
}