#include "CardStructure.h"
#include <stdexcept>

CardStructure::CardStructure(int age, std::vector<std::unique_ptr<Card>> deck) 
    : current_age(age), cards(std::move(deck)) {
    // Age 1, 2 是20张，Age 3 也是20张（注意：Age3原本是20时代卡+3公会共23张，
    // 但为了逻辑统一，假设传入的deck已经是筛选好的20张）
    if (cards.size() != 20) throw std::runtime_error("Deck size error");
    setupDependenciesAndFaces(age);
}

void CardStructure::addDependency(int supporter, int target) {
    unlocks[supporter].push_back(target);
    dependency_count[target]++;
}

void CardStructure::setupDependenciesAndFaces(int age) {
    unlocks.clear();
    dependency_count.clear();
    accessible.clear();

    if (age == 1) {
        // Age I: 正金字塔 6-5-4-3-2 (20张)
        // 层级: L1(0-5), L2(6-10), L3(11-14), L4(15-17), L5(18-19)
        // 初始可选: 最底层 L1
        for (int i = 0; i <= 5; ++i) accessible.insert(i);

        // 依赖关系 (下层支撑上层)
        // L1 -> L2
        addDependency(0,6); addDependency(1,6); addDependency(1,7); addDependency(2,7);
        addDependency(2,8); addDependency(3,8); addDependency(3,9); addDependency(4,9);
        addDependency(4,10); addDependency(5,10);
        // L2 -> L3
        addDependency(6,11); addDependency(7,11); addDependency(7,12); addDependency(8,12);
        addDependency(8,13); addDependency(9,13); addDependency(9,14); addDependency(10,14);
        // L3 -> L4
        addDependency(11,15); addDependency(12,15); addDependency(12,16); addDependency(13,16);
        addDependency(13,17); addDependency(14,17);
        // L4 -> L5
        addDependency(15,18); addDependency(16,18); addDependency(16,19); addDependency(17,19);

        // 面朝向
        for (int i=0; i<20; ++i) {
            if (i <= 5) cards[i]->is_face_up = true;       // L1 上
            else if (i <= 10) cards[i]->is_face_up = false; // L2 下
            else if (i <= 14) cards[i]->is_face_up = true;  // L3 上
            else if (i <= 17) cards[i]->is_face_up = false; // L4 下
            else cards[i]->is_face_up = true;               // L5 上
        }

    } else if (age == 2) {
        // Age II: 倒金字塔 2-3-4-5-6 (20张)
        // 层级: L1(0-1), L2(2-4), L3(5-8), L4(9-13), L5(14-19)
        // 初始可选: 最底层 L1 (也就是视觉上的最尖端)
        for (int i = 0; i <= 1; ++i) accessible.insert(i);

        // L1 -> L2
        addDependency(0,2); addDependency(0,3); addDependency(1,3); addDependency(1,4);
        // L2 -> L3
        addDependency(2,5); addDependency(2,6); addDependency(3,6); addDependency(3,7); addDependency(4,7); addDependency(4,8);
        // L3 -> L4
        addDependency(5,9); addDependency(5,10); addDependency(6,10); addDependency(6,11); 
        addDependency(7,11); addDependency(7,12); addDependency(8,12); addDependency(8,13);
        // L4 -> L5
        addDependency(9,14); addDependency(10,14); addDependency(10,15); addDependency(11,15);
        addDependency(11,16); addDependency(11,17); addDependency(12,17); addDependency(12,18);
        addDependency(13,18); addDependency(13,19);

        // 面朝向
        for (int i=0; i<20; ++i) {
            if (i <= 1) cards[i]->is_face_up = true;
            else if (i <= 4) cards[i]->is_face_up = false;
            else if (i <= 8) cards[i]->is_face_up = true;
            else if (i <= 13) cards[i]->is_face_up = false;
            else cards[i]->is_face_up = true;
        }

    } else if (age == 3) {
        // Age III: 类似括号的对称布局 (20张)
        // 简化模型: 0,1(底) -> 2,3,4 -> 5,6,7,8 -> 9,10(中) -> 11,12,13,14 -> 15,16,17 -> 18,19(顶)
        for (int i = 0; i <= 1; ++i) accessible.insert(i);

        // 核心支撑逻辑
        addDependency(0,2); addDependency(0,3); addDependency(1,3); addDependency(1,4); // L1-L2
        addDependency(2,5); addDependency(3,6); addDependency(3,7); addDependency(4,8); // L2-L3
        addDependency(5,9); addDependency(6,9); addDependency(7,10); addDependency(8,10); // L3-L4
        addDependency(9,11); addDependency(9,12); addDependency(10,13); addDependency(10,14); // L4-L5
        addDependency(11,15); addDependency(12,15); addDependency(12,16); 
        addDependency(13,16); addDependency(13,17); addDependency(14,17); // L5-L6
        addDependency(15,18); addDependency(16,18); addDependency(16,19); addDependency(17,19); // L6-L7

        // 面朝向
        for (int i=0; i<20; ++i) {
            if (i <= 1) cards[i]->is_face_up = true;
            else if (i <= 4) cards[i]->is_face_up = false;
            else if (i <= 8) cards[i]->is_face_up = true;
            else if (i <= 10) cards[i]->is_face_up = false;
            else if (i <= 14) cards[i]->is_face_up = true;
            else if (i <= 17) cards[i]->is_face_up = false;
            else cards[i]->is_face_up = true;
        }
    }
}

std::unique_ptr<Card> CardStructure::takeCard(int pos) {
    if (accessible.find(pos) == accessible.end()) 
        throw std::runtime_error("Card is blocked by others!");

    auto card = std::move(cards[pos]);
    accessible.erase(pos);

    // 检查这个位置解锁了谁
    if (unlocks.count(pos)) {
        for (int target : unlocks[pos]) {
            dependency_count[target]--; 
            // 只有当压着它的所有牌都被拿走（入度为0）
            if (dependency_count[target] == 0) {
                accessible.insert(target);
                // 翻开新露出的卡牌
                if (!cards[target]->is_face_up) {
                    cards[target]->is_face_up = true;
                }
            }
        }
    }
    return card;
}

std::vector<int> CardStructure::getAccessible() const {
    return std::vector<int>(accessible.begin(), accessible.end());
}

bool CardStructure::isEmpty() const {
    return accessible.empty(); // 当没有可拿的牌时时代结束
}

const Card* CardStructure::getCard(int pos) const {
    if (pos < 0 || pos >= 20) return nullptr;
    return cards[pos].get();
}