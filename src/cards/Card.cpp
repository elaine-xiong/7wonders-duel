#include "Card.h"
#include "../player/Player.h"  // 假设包含 add_resource, add_victory_points, add_shield, add_science_symbol, has_card, count_neighbor_brown 等
#include "../core/Game.h"    // 假设包含 check_science_victory 等

Card::Card(std::string n, int a, Color c, std::map<Resource, int> co, std::vector<std::string> pre, std::string pro)
    : name(n), age(a), color(c), cost(co), chain_prerequisites(pre), chain_provides(pro) {}

bool Card::can_build_free(const Player& player) const {
    for (const auto& pre : chain_prerequisites) {
        if (player.has_card(pre)) return true;
    }
    return false;
}

std::vector<std::unique_ptr<Card>> createAllCards() {
    std::vector<std::unique_ptr<Card>> cards;

    // 时代 I 卡牌 (49 张)
    // 棕色 (基本原料)
    cards.push_back(std::make_unique<Card>("Lumber Yard", 1, Color::BROWN, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::WOOD, 1); }));
    cards.push_back(std::make_unique<Card>("Ore Vein", 1, Color::BROWN, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::ORE, 1); }));
    cards.push_back(std::make_unique<Card>("Clay Pool", 1, Color::BROWN, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::BRICK, 1); }));
    cards.push_back(std::make_unique<Card>("Stone Pit", 1, Color::BROWN, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::STONE, 1); }));
    cards.push_back(std::make_unique<Card>("Timber Yard", 1, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource_choice({Resource::WOOD, Resource::STONE}, 1); }));
    cards.push_back(std::make_unique<Card>("Clay Pit", 1, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource_choice({Resource::BRICK, Resource::ORE}, 1); }));
    cards.push_back(std::make_unique<Card>("Excavation", 1, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource_choice({Resource::STONE, Resource::BRICK}, 1); }));
    cards.push_back(std::make_unique<Card>("Forest Cave", 1, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource_choice({Resource::WOOD, Resource::ORE}, 1); }));
    cards.push_back(std::make_unique<Card>("Tree Farm", 1, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource_choice({Resource::WOOD, Resource::BRICK}, 1); }));
    cards.push_back(std::make_unique<Card>("Mine", 1, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource_choice({Resource::ORE, Resource::STONE}, 1); }));

    // 灰色 (手工艺品)
    cards.push_back(std::make_unique<Card>("Loom", 1, Color::GREY, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::CLOTH, 1); }));
    cards.push_back(std::make_unique<Card>("Glassworks", 1, Color::GREY, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::GLASS, 1); }));
    cards.push_back(std::make_unique<Card>("Press", 1, Color::GREY, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::PAPYRUS, 1); }));

    // 蓝色 (市政)
    cards.push_back(std::make_unique<Card>("Pawnshop", 1, Color::BLUE, {}, {}, "", [&](Player& s, Game& g) { s.add_victory_points(3); }));
    cards.push_back(std::make_unique<Card>("Altar", 1, Color::BLUE, {}, {}, "moon", [&](Player& s, Game& g) { s.add_victory_points(2); }));  // 提供 moon 符号
    cards.push_back(std::make_unique<Card>("Baths", 1, Color::BLUE, {{Resource::STONE, 1}}, {}, "vase", [&](Player& s, Game& g) { s.add_victory_points(3); }));  // 提供 vase
    cards.push_back(std::make_unique<Card>("Theater", 1, Color::BLUE, {}, {}, "mask", [&](Player& s, Game& g) { s.add_victory_points(2); }));  // 提供 mask

    // 黄色 (商业)
    cards.push_back(std::make_unique<Card>("Tavern", 1, Color::YELLOW, {}, {}, "", [&](Player& s, Game& g) { s.add_coins(5); }));
    cards.push_back(std::make_unique<Card>("East Trading Post", 1, Color::YELLOW, {}, {}, "", [&](Player& s, Game& g) { s.reduce_trade_cost_left(1); }));  // 右邻折扣
    cards.push_back(std::make_unique<Card>("West Trading Post", 1, Color::YELLOW, {}, {}, "", [&](Player& s, Game& g) { s.reduce_trade_cost_right(1); }));  // 左邻折扣
    cards.push_back(std::make_unique<Card>("Marketplace", 1, Color::YELLOW, {}, {}, "", [&](Player& s, Game& g) { s.reduce_trade_cost_both(1); }));  // 两侧折扣

    // 红色 (军事)
    cards.push_back(std::make_unique<Card>("Stockade", 1, Color::RED, {{Resource::WOOD, 1}}, {}, "", [&](Player& s, Game& g) { s.add_shield(1); }));
    cards.push_back(std::make_unique<Card>("Barracks", 1, Color::RED, {{Resource::ORE, 1}}, {}, "", [&](Player& s, Game& g) { s.add_shield(1); }));
    cards.push_back(std::make_unique<Card>("Guard Tower", 1, Color::RED, {{Resource::BRICK, 1}}, {}, "", [&](Player& s, Game& g) { s.add_shield(1); }));

    // 绿色 (科技)
    cards.push_back(std::make_unique<Card>("Apothecary", 1, Color::GREEN, {{Resource::CLOTH, 1}}, {}, "COMPASS", [&](Player& s, Game& g) { s.add_science_symbol(Resource::SCIENCE_COMPASS); g.check_science_victory(s); }));
    cards.push_back(std::make_unique<Card>("Workshop", 1, Color::GREEN, {{Resource::GLASS, 1}}, {}, "GEAR", [&](Player& s, Game& g) { s.add_science_symbol(Resource::SCIENCE_GEAR); g.check_science_victory(s); }));
    cards.push_back(std::make_unique<Card>("Scriptorium", 1, Color::GREEN, {{Resource::PAPYRUS, 1}}, {}, "TABLET", [&](Player& s, Game& g) { s.add_science_symbol(Resource::SCIENCE_TABLET); g.check_science_victory(s); }));

    // 时代 II 卡牌 (49 张)
    // 棕色
    cards.push_back(std::make_unique<Card>("Sawmill", 2, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::WOOD, 2); }));
    cards.push_back(std::make_unique<Card>("Quarry", 2, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::STONE, 2); }));
    cards.push_back(std::make_unique<Card>("Brickyard", 2, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::BRICK, 2); }));
    cards.push_back(std::make_unique<Card>("Foundry", 2, Color::BROWN, {{Resource::COIN, 1}}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::ORE, 2); }));

    // 灰色
    cards.push_back(std::make_unique<Card>("Loom", 2, Color::GREY, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::CLOTH, 1); }));
    cards.push_back(std::make_unique<Card>("Glassworks", 2, Color::GREY, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::GLASS, 1); }));
    cards.push_back(std::make_unique<Card>("Press", 2, Color::GREY, {}, {}, "", [&](Player& s, Game& g) { s.add_resource(Resource::PAPYRUS, 1); }));

    // 蓝色
    cards.push_back(std::make_unique<Card>("Aqueduct", 2, Color::BLUE, {{Resource::STONE, 3}}, {"Baths"}, "", [&](Player& s, Game& g) { s.add_victory_points(5); }));
    cards.push_back(std::make_unique<Card>("Temple", 2, Color::BLUE, {{Resource::WOOD, 1}, {Resource::BRICK, 1}, {Resource::GLASS, 1}}, {"Altar"}, "", [&](Player& s, Game& g) { s.add_victory_points(3); }));
    cards.push_back(std::make_unique<Card>("Statue", 2, Color::BLUE, {{Resource::ORE, 2}, {Resource::WOOD, 1}}, {"Theater"}, "", [&](Player& s, Game& g) { s.add_victory_points(4); }));
    cards.push_back(std::make_unique<Card>("Courthouse", 2, Color::BLUE, {{Resource::BRICK, 2}, {Resource::CLOTH, 1}}, {"Scriptorium"}, "", [&](Player& s, Game& g) { s.add_victory_points(4); }));

    // 黄色
    cards.push_back(std::make_unique<Card>("Vineyard", 2, Color::YELLOW, {}, {}, "", [&](Player& s, Game& g) { s.add_coins(s.count_neighbor_brown() + s.count_own_brown()); }));
    cards.push_back(std::make_unique<Card>("Bazar", 2, Color::YELLOW, {}, {}, "", [&](Player& s, Game& g) { s.add_coins(2 * (s.count_neighbor_grey() + s.count_own_grey())); }));
    cards.push_back(std::make_unique<Card>("Caravansery", 2, Color::YELLOW, {{Resource::WOOD, 2}}, {}, "", [&](Player& s, Game& g) { s.add_resource_choice({Resource::WOOD, Resource::STONE, Resource::BRICK, Resource::ORE}, 1); }));
    cards.push_back(std::make_unique<Card>("Forum", 2, Color::YELLOW, {{Resource::BRICK, 2}}, {}, "", [&](Player& s, Game& g) { s.add_resource_choice({Resource::GLASS, Resource::CLOTH, Resource::PAPYRUS}, 1); }));

    // 红色
    cards.push_back(std::make_unique<Card>("Walls", 2, Color::RED, {{Resource::STONE, 3}}, {}, "", [&](Player& s, Game& g) { s.add_shield(2); }));
    cards.push_back(std::make_unique<Card>("Training Ground", 2, Color::RED, {{Resource::ORE, 2}, {Resource::WOOD, 1}}, {}, "", [&](Player& s, Game& g) { s.add_shield(2); }));
    cards.push_back(std::make_unique<Card>("Stables", 2, Color::RED, {{Resource::BRICK, 1}, {Resource::WOOD, 1}, {Resource::ORE, 1}}, {"Apothecary"}, "", [&](Player& s, Game& g) { s.add_shield(2); }));
    cards.push_back(std::make_unique<Card>("Archery Range", 2, Color::RED, {{Resource::WOOD, 2}, {Resource::ORE, 1}}, {"Workshop"}, "", [&](Player& s, Game& g) { s.add_shield(2); }));

    // 绿色
    cards.push_back(std::make_unique<Card>("Dispensary", 2, Color::GREEN, {{Resource::GLASS, 1}, {Resource::ORE, 2}}, {"Apothecary"}, "COMPASS", [&](Player& s, Game& g) { s.add_science_symbol(Resource::SCIENCE_COMPASS); g.check_science_victory(s); }));
    cards.push_back(std::make_unique<Card>("Laboratory", 2, Color::GREEN, {{Resource::BRICK, 2}, {Resource::PAPYRUS, 1}}, {"Workshop"}, "GEAR", [&](Player& s, Game& g) { s.add_science_symbol(Resource::SCIENCE_GEAR); g.check_science_victory(s); }));
    cards.push_back(std::make_unique<Card>("Library", 2, Color::GREEN, {{Resource::STONE, 2}, {Resource::CLOTH, 1}}, {"Scriptorium"}, "TABLET", [&](Player& s, Game& g) { s.add_science_symbol(Resource::SCIENCE_TABLET); g.check_science_victory(s); }));
    cards.push_back(std::make_unique<Card>("School", 2, Color::GREEN, {{Resource::WOOD, 1}, {Resource::PAPYRUS, 1}}, {}, "TABLET", [&](Player& s, Game& g) { s.add_science_symbol(Resource::SCIENCE_TABLET); g.check_science_victory(s); }));

    // 时代 III 卡牌 (50 张)
    // 蓝色
    cards.push_back(std::make_unique<Card>("Pantheon", 3, Color::BLUE, {{Resource::BRICK, 2}, {Resource::ORE, 1}, {Resource::CLOTH, 1}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}}, {"Temple"}, "", [&](Player& s, Game& g) { s.add_victory_points(7); }));
    cards.push_back(std::make_unique<Card>("Gardens", 3, Color::BLUE, {{Resource::BRICK, 3}, {Resource::WOOD, 2}}, {"Statue"}, "", [&](Player& s, Game& g) { s.add_victory_points(5); }));
    cards.push_back(std::make_unique<Card>("Town Hall", 3, Color::BLUE, {{Resource::STONE, 2}, {Resource::ORE, 1}, {Resource::GLASS, 1}}, {}, "", [&](Player& s, Game& g) { s.add_victory_points(6); }));
    cards.push_back(std::make_unique<Card>("Palace", 3, Color::BLUE, {{Resource::STONE, 1}, {Resource::WOOD, 1}, {Resource::ORE, 1}, {Resource::BRICK, 1}, {Resource::GLASS, 1}, {Resource::CLOTH, 1}, {Resource::PAPYRUS, 1}}, {}, "", [&](Player& s, Game& g) { s.add_victory_points(8); }));
    cards.push_back(std::make_unique<Card>("Senate", 3, Color::BLUE, {{Resource::WOOD, 2}, {Resource::STONE, 1}, {Resource::ORE, 1}}, {"Library"}, "", [&](Player& s, Game& g) { s.add_victory_points(6); }));

    // 黄色
    cards.push_back(std::make_unique<Card>("Haven", 3, Color::YELLOW, {{Resource::WOOD, 1}, {Resource::ORE, 1}, {Resource::CLOTH, 1}}, {"Forum"}, "", [&](Player& s, Game& g) { s.add_coins(s.count_brown()); s.add_victory_points(s.count_brown()); }));
    cards.push_back(std::make_unique<Card>("Lighthouse", 3, Color::YELLOW, {{Resource::STONE, 1}, {Resource::GLASS, 1}}, {"Caravansery"}, "", [&](Player& s, Game& g) { s.add_coins(s.count_yellow()); s.add_victory_points(s.count_yellow()); }));
    cards.push_back(std::make_unique<Card>("Chamber of Commerce", 3, Color::YELLOW, {{Resource::BRICK, 2}, {Resource::PAPYRUS, 1}}, {}, "", [&](Player& s, Game& g) { s.add_coins(s.count_grey() * 2); s.add_victory_points(s.count_grey()); }));
    cards.push_back(std::make_unique<Card>("Arena", 3, Color::YELLOW, {{Resource::ORE, 1}, {Resource::STONE, 2}}, {"Dispensary"}, "", [&](Player& s, Game& g) { s.add_coins(s.count_wonder_stages() * 3); s.add_victory_points(s.count_wonder_stages()); }));

    // 红色
    cards.push_back(std::make_unique<Card>("Fortifications", 3, Color::RED, {{Resource::STONE, 1}, {Resource::ORE, 3}}, {"Walls"}, "", [&](Player& s, Game& g) { s.add_shield(3); }));
    cards.push_back(std::make_unique<Card>("Circus", 3, Color::RED, {{Resource::STONE, 3}}, {"Training Ground"}, "", [&](Player& s, Game& g) { s.add_shield(3); }));
    cards.push_back(std::make_unique<Card>("Arsenal", 3, Color::RED, {{Resource::WOOD, 2}, {Resource::ORE, 1}}, {}, "", [&](Player& s, Game& g) { s.add_shield(3); }));
    cards.push_back(std::make_unique<Card>("Siege Workshop", 3, Color::RED, {{Resource::BRICK, 3}, {Resource::WOOD, 1}}, {"Laboratory"}, "", [&](Player& s, Game& g) { s.add_shield(3); }));

    // 绿色
    cards.push_back(std::make_unique<Card>("University", 3, Color::GREEN, {{Resource::WOOD, 2}, {Resource::PAPYRUS, 1}, {Resource::GLASS, 1}}, {"School"}, "COMPASS", [&](Player& s, Game& g) { s.add_science_symbol(Resource::SCIENCE_COMPASS); g.check_science_victory(s); }));
    cards.push_back(std::make_unique<Card>("Observatory", 3, Color::GREEN, {{Resource::ORE, 2}, {Resource::GLASS, 1}, {Resource::CLOTH, 1}}, {"Laboratory"}, "GEAR", [&](Player& s, Game& g) { s.add_science_symbol(Resource::SCIENCE_GEAR); g.check_science_victory(s); }));
    cards.push_back(std::make_unique<Card>("Study", 3, Color::GREEN, {{Resource::WOOD, 1}, {Resource::PAPYRUS, 1}, {Resource::CLOTH, 1}}, {"Library"}, "TABLET", [&](Player& s, Game& g) { s.add_science_symbol(Resource::SCIENCE_TABLET); g.check_science_victory(s); }));
    cards.push_back(std::make_unique<Card>("Lodge", 3, Color::GREEN, {{Resource::BRICK, 2}, {Resource::CLOTH, 1}, {Resource::PAPYRUS, 1}}, {"Dispensary"}, "COMPASS", [&](Player& s, Game& g) { s.add_science_symbol(Resource::SCIENCE_COMPASS); g.check_science_victory(s); }));
    cards.push_back(std::make_unique<Card>("Academy", 3, Color::GREEN, {{Resource::STONE, 3}, {Resource::GLASS, 1}}, {"School"}, "GEAR", [&](Player& s, Game& g) { s.add_science_symbol(Resource::SCIENCE_GEAR); g.check_science_victory(s); }));

    // 紫色 (行会)
    cards.push_back(std::make_unique<Card>("Workers Guild", 3, Color::PURPLE, {{Resource::ORE, 2}, {Resource::BRICK, 1}, {Resource::STONE, 1}, {Resource::WOOD, 1}}, {}, "", [&](Player& s, Game& g) { s.add_victory_points(s.count_neighbor_brown()); }));
    cards.push_back(std::make_unique<Card>("Craftsmens Guild", 3, Color::PURPLE, {{Resource::ORE, 2}, {Resource::STONE, 2}}, {}, "", [&](Player& s, Game& g) { s.add_victory_points(2 * s.count_neighbor_grey()); }));
    cards.push_back(std::make_unique<Card>("Traders Guild", 3, Color::PURPLE, {{Resource::GLASS, 1}, {Resource::CLOTH, 1}, {Resource::PAPYRUS, 1}}, {}, "", [&](Player& s, Game& g) { s.add_victory_points(s.count_neighbor_yellow()); }));
    cards.push_back(std::make_unique<Card>("Philosophers Guild", 3, Color::PURPLE, {{Resource::BRICK, 3}, {Resource::CLOTH, 1}, {Resource::PAPYRUS, 1}}, {}, "", [&](Player& s, Game& g) { s.add_victory_points(s.count_neighbor_green()); }));
    cards.push_back(std::make_unique<Card>("Spy Guild", 3, Color::PURPLE, {{Resource::BRICK, 3}, {Resource::GLASS, 1}}, {}, "", [&](Player& s, Game& g) { s.add_victory_points(s.count_neighbor_red()); }));
    cards.push_back(std::make_unique<Card>("Strategists Guild", 3, Color::PURPLE, {{Resource::ORE, 2}, {Resource::STONE, 1}, {Resource::CLOTH, 1}}, {}, "", [&](Player& s, Game& g) { s.add_victory_points(s.count_neighbor_defeat_tokens()); }));
    cards.push_back(std::make_unique<Card>("Shipowners Guild", 3, Color::PURPLE, {{Resource::WOOD, 3}, {Resource::PAPYRUS, 1}, {Resource::GLASS, 1}}, {}, "", [&](Player& s, Game& g) { s.add_victory_points(s.count_own_brown() + s.count_own_grey() + s.count_own_purple()); }));
    cards.push_back(std::make_unique<Card>("Scientists Guild", 3, Color::PURPLE, {{Resource::WOOD, 2}, {Resource::ORE, 2}, {Resource::PAPYRUS, 1}}, {}, "", [&](Player& s, Game& g) { s.add_science_symbol_choice(); }));  // 选择一个符号
    cards.push_back(std::make_unique<Card>("Magistrates Guild", 3, Color::PURPLE, {{Resource::WOOD, 3}, {Resource::STONE, 1}, {Resource::CLOTH, 1}}, {}, "", [&](Player& s, Game& g) { s.add_victory_points(s.count_neighbor_blue()); }));
    cards.push_back(std::make_unique<Card>("Builders Guild", 3, Color::PURPLE, {{Resource::STONE, 2}, {Resource::BRICK, 2}, {Resource::GLASS, 1}}, {}, "", [&](Player& s, Game& g) { s.add_victory_points(s.count_all_wonder_stages()); }));

    return cards;
}