#pragma once

#include <string>
#include <map>
#include <functional>
#include "../player/Player.h"

class Wonder {
public:
    std::string name;
    std::map<Resource, int> cost;
    std::function<void(Player& self, Player& opponent, Game& game)> effect;

    Wonder(std::string n, std::map<Resource, int> co, auto eff);
};
