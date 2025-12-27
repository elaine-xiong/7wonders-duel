#include "Wonder.h"
#include "../core/Game.h"
Wonder::Wonder(std::string n, std::map<Resource, int> co, auto eff) : name(n), cost(co), effect(eff) {}

// 工厂函数创建所有奇迹
std::vector<Wonder> createAllWonders() {
    std::vector<Wonder> wonders;
    wonders.emplace_back("The Appian Way", {{Resource::STONE, 2}, {Resource::CLAY, 1}, {Resource::WOOD, 1}}, [&](Player& s, Player& o, Game& g) { o.addCoins(-3); g.setExtraTurn(true); s.addVictoryPoints(3); });
    wonders.emplace_back("Circus Maximus", {{Resource::STONE, 2}, {Resource::CLAY, 1}}, [&](Player& s, Player& o, Game& g) { /* 破坏对手灰卡 */ g.movePawn(1); s.addVictoryPoints(3); });
    wonders.emplace_back("The Colossus", {{Resource::CLAY, 3}, {Resource::WOOD, 2}}, [&](Player& s, Player& o, Game& g) { g.movePawn(2); s.addVictoryPoints(3); });
    wonders.emplace_back("The Great Library", {{Resource::STONE, 2}, {Resource::WOOD, 2}}, [&](Player& s, Player& o, Game& g) { /* 抽3进展标记 */ s.addVictoryPoints(4); });
    wonders.emplace_back("The Great Lighthouse", {{Resource::STONE, 2}, {Resource::WOOD, 1}}, [&](Player& s, Player& o, Game& g) { s.add_resource_choice({Resource::STONE, Resource::CLAY, Resource::WOOD}); s.addVictoryPoints(4); });
    wonders.emplace_back("The Hanging Gardens", {{Resource::STONE, 1}, {Resource::CLAY, 2}}, [&](Player& s, Player& o, Game& g) { s.addCoins(6); g.setExtraTurn(true); s.addVictoryPoints(3); });
    wonders.emplace_back("The Mausoleum", {{Resource::CLAY, 2}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}}, [&](Player& s, Player& o, Game& g) { /* 从弃牌堆免费建一张 */ s.addVictoryPoints(2); });
    wonders.emplace_back("Piraeus", {{Resource::WOOD, 2}, {Resource::PAPYRUS, 1}}, [&](Player& s, Player& o, Game& g) { s.add_resource_choice({Resource::GLASS, Resource::PAPYRUS}); g.setExtraTurn(true); s.addVictoryPoints(2); });
    wonders.emplace_back("The Pyramids", {{Resource::STONE, 3}, {Resource::WOOD, 2}}, [&](Player& s, Player& o, Game& g) { s.addVictoryPoints(9); });
    wonders.emplace_back("The Sphinx", {{Resource::CLAY, 1}, {Resource::GLASS, 1}}, [&](Player& s, Player& o, Game& g) { g.setExtraTurn(true); s.addVictoryPoints(6); });
    wonders.emplace_back("The Statue of Zeus", {{Resource::WOOD, 2}, {Resource::PAPYRUS, 1}}, [&](Player& s, Player& o, Game& g) { /* 破坏对手棕卡 */ g.movePawn(1); s.addVictoryPoints(3); });
    wonders.emplace_back("The Temple of Artemis", {{Resource::STONE, 1}, {Resource::WOOD, 1}, {Resource::PAPYRUS, 1}}, [&](Player& s, Player& o, Game& g) { s.addCoins(12); g.setExtraTurn(true); });
    return wonders;
}