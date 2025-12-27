#ifndef CARD_STRUCTURE_H
#define CARD_STRUCTURE_H

#include "Card.h"
#include <vector>
#include <map>
#include <set>
#include <memory>

class CardStructure {
private:
    std::vector<std::unique_ptr<Card>> cards;  // 存储所有20张卡牌
    std::map<int, std::vector<int>> unlocks;   // 结构：位置i 被拿走后，会尝试解锁这些位置
    std::map<int, int> dependency_count;       // 核心逻辑：位置i 还需要被解锁多少次才能拿（入度）
    std::set<int> accessible;                  // 当前真正可以被点击拿取的卡牌索引
    int current_age;
    // 内部硬编码工具
    void addDependency(int supporter, int target); 
    void setupDependenciesAndFaces(int age);

public:
    CardStructure(int age, std::vector<std::unique_ptr<Card>> deck);
    
    std::vector<int> getAccessible() const;
    std::unique_ptr<Card> takeCard(int pos);
    
    bool isEmpty() const;
    const Card* getCard(int pos) const; 
    int getAge() const { return current_age; }

};

#endif // CARD_STRUCTURE_H