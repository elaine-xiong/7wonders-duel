#ifndef CARD_STRUCTURE_H
#define CARD_STRUCTURE_H

#include "Card.h"
#include <vector>
#include <map>
#include <set>
#include <memory>

class CardStructure {
private:
    std::vector<std::unique_ptr<Card>> cards;  // 20张
    std::map<int, std::vector<int>> unlocks;  // 位置i解锁这些位置
    std::set<int> accessible;  // 当前可拿位置
    int current_age;

    void setupDependenciesAndFaces(int age);  // 硬编码依赖和面朝

public:
    CardStructure(int age, std::vector<std::unique_ptr<Card>> deck);
    std::vector<int> getAccessible() const;
    std::unique_ptr<Card> takeCard(int pos);
    bool isEmpty() const;
    const Card* getCard(int pos) const;  // 用于UI查看
    int getAge() const { return current_age; }
};

#endif // CARD_STRUCTURE_H