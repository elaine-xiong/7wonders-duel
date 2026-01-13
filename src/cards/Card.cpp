#include "Card.h"
#include "player/Player.h"
#include "core/Game.h"
#include <algorithm>

Card::Card(std::string n, int a, Color c) 
    : name(std::move(n)), age(a), color(c), is_face_up(false) {
    special_reward = SpecialReward();
}

bool Card::can_be_free(const Player& p) const {
    // 检查基础连锁
    if (link_prerequisite != LinkSymbol::NONE && p.has_chain_symbol(link_prerequisite)) return true;
    
    // 检查多选一方案中的连锁
    for (const auto& opt : cost_options) {
        if (opt.link != LinkSymbol::NONE && p.has_chain_symbol(opt.link)) return true;
    }
    
    return false;
}

void Card::apply_effect(Player& p, Game& g) const {
    // 1. 基础数值
    if (victory_points > 0) p.add_victory_points(victory_points);
    if (shields > 0) {
        int total_shields = shields;
        // 进度标记 STRATEGY (军事策略): 每张红卡额外获得 1 个盾牌
        if (p.has_progress_token(ProgressToken::STRATEGY)) {
            total_shields += 1;
        }
        g.move_pawn(total_shields);
    }
    if (science_symbol >= Resource::ARMILLARY && science_symbol <= Resource::LAW) {
        if (p.add_science_symbol(science_symbol)) {
            g.trigger_progress_token_selection(p, 1);
        }
    }

    // 2. 连锁符号
    if (link_provides != LinkSymbol::NONE) p.add_chain_symbol(link_provides);

    // 3. 特殊收益
    if (special_reward.active) {
        int count = 0;
        if (special_reward.count_wonders) {
            count = p.count_wonder_stages();
            if (special_reward.count_both) count = std::max(count, g.get_opponent(p)->count_wonder_stages());
        } else if (name == "Moneylenders Guild") {
            // 放贷者公会：每3个金币1分（取最高资产者）
            int own = p.get_coins();
            int opp = g.get_opponent(p)->get_coins();
            count = (special_reward.count_both ? std::max(own, opp) : own) / 3;
        } else {
            int own = p.get_card_count_by_color(special_reward.target_color);
            int opp = g.get_opponent(p)->get_card_count_by_color(special_reward.target_color);
            
            // 特殊处理：船主公会同时计算褐色和灰色
            if (name == "Shipowners Guild") {
                own += p.get_card_count_by_color(Color::GREY);
                opp += g.get_opponent(p)->get_card_count_by_color(Color::GREY);
            }
            
            count = special_reward.count_both ? std::max(own, opp) : own;
        }
        p.add_coins(count * special_reward.coins_per_card);
    }

    // 4. 即时 Lambda
    if (immediate_func) immediate_func(p, g);
}

// --- 辅助创建器 ---
std::unique_ptr<Card> make_raw(std::string n, int a, Resource r, int gold = 0) {
    auto c = std::make_unique<Card>(n, a, Color::BROWN);
    if (gold > 0) c->cost = {{Resource::COIN, gold}};
    c->immediate_func = [r](Player& p, Game& g){ p.add_resource(r, 1, true); };
    return c;
}

std::vector<std::unique_ptr<Card>> createAllCards() {
    std::vector<std::unique_ptr<Card>> cards;

    // ======================== AGE I (23 Cards) ========================
    cards.push_back(make_raw("Lumber Yard", 1, Resource::WOOD));
    cards.push_back(make_raw("Logging Camp", 1, Resource::WOOD, 1));
    cards.push_back(make_raw("Clay Pool", 1, Resource::CLAY));
    cards.push_back(make_raw("Clay Pit", 1, Resource::CLAY, 1));
    cards.push_back(make_raw("Quarry", 1, Resource::STONE));
    cards.push_back(make_raw("Stone Pit", 1, Resource::STONE, 1));
    
    auto glass1 = std::make_unique<Card>("Glassworks", 1, Color::GREY);
    glass1->cost = {{Resource::COIN, 1}}; glass1->immediate_func = [](Player& p, Game& g){ p.add_resource(Resource::GLASS, 1, true); };
    cards.push_back(std::move(glass1));

    auto press1 = std::make_unique<Card>("Press", 1, Color::GREY);
    press1->cost = {{Resource::COIN, 1}}; press1->immediate_func = [](Player& p, Game& g){ p.add_resource(Resource::PAPYRUS, 1, true); };
    cards.push_back(std::move(press1));

    auto altar = std::make_unique<Card>("Altar", 1, Color::BLUE);
    altar->victory_points = 3; altar->link_provides = LinkSymbol::MOON;
    cards.push_back(std::move(altar));

    auto theater = std::make_unique<Card>("Theater", 1, Color::BLUE);
    theater->victory_points = 3; theater->link_provides = LinkSymbol::MASK;
    cards.push_back(std::move(theater));

    auto baths = std::make_unique<Card>("Baths", 1, Color::BLUE);
    baths->cost = {{Resource::STONE, 1}}; baths->victory_points = 3; baths->link_provides = LinkSymbol::DROP;
    cards.push_back(std::move(baths));

    auto workshop = std::make_unique<Card>("Workshop", 1, Color::GREEN);
    workshop->cost = {{Resource::PAPYRUS, 1}}; workshop->science_symbol = Resource::WHEEL; workshop->victory_points = 1; 
    cards.push_back(std::move(workshop));

    auto apothecary = std::make_unique<Card>("Apothecary", 1, Color::GREEN);
    apothecary->cost = {{Resource::GLASS, 1}}; apothecary->science_symbol = Resource::WHEEL; apothecary->victory_points = 1; 
    cards.push_back(std::move(apothecary));

    auto scriptorium = std::make_unique<Card>("Scriptorium", 1, Color::GREEN);
    scriptorium->cost = {{Resource::COIN, 2}}; scriptorium->science_symbol = Resource::QUILL; scriptorium->link_provides = LinkSymbol::BOOK;
    cards.push_back(std::move(scriptorium));

    auto pharmacist = std::make_unique<Card>("Pharmacist", 1, Color::GREEN);
    pharmacist->cost = {{Resource::COIN, 2}}; pharmacist->science_symbol = Resource::MORTAR; pharmacist->link_provides = LinkSymbol::GEAR;
    cards.push_back(std::move(pharmacist));

    auto tavern = std::make_unique<Card>("Tavern", 1, Color::YELLOW);
    tavern->immediate_func = [](Player& p, Game& g){ p.add_coins(4); }; tavern->link_provides = LinkSymbol::VASE;
    cards.push_back(std::move(tavern));

    auto stone_res = std::make_unique<Card>("Stone Reserve", 1, Color::YELLOW);
    stone_res->cost = {{Resource::COIN, 3}}; stone_res->immediate_func = [](Player& p, Game& g){ p.set_fixed_trade_cost(Resource::STONE, 1); };
    cards.push_back(std::move(stone_res));

    auto clay_res = std::make_unique<Card>("Clay Reserve", 1, Color::YELLOW);
    clay_res->cost = {{Resource::COIN, 3}}; clay_res->immediate_func = [](Player& p, Game& g){ p.set_fixed_trade_cost(Resource::CLAY, 1); };
    cards.push_back(std::move(clay_res));

    auto wood_res = std::make_unique<Card>("Wood Reserve", 1, Color::YELLOW);
    wood_res->cost = {{Resource::COIN, 3}}; wood_res->immediate_func = [](Player& p, Game& g){ p.set_fixed_trade_cost(Resource::WOOD, 1); };
    cards.push_back(std::move(wood_res));

    auto guard_tower = std::make_unique<Card>("Guard Tower", 1, Color::RED);
    guard_tower->shields = 1;
    cards.push_back(std::move(guard_tower));

    auto stables = std::make_unique<Card>("Stables", 1, Color::RED);
    stables->cost = {{Resource::WOOD, 1}};stables->shields = 1;stables->link_provides = LinkSymbol::HORSESHOE;
    cards.push_back(std::move(stables));

    auto garrison = std::make_unique<Card>("Garrison", 1, Color::RED);
    garrison->cost = {{Resource::CLAY, 1}};garrison->shields = 1;garrison->link_provides = LinkSymbol::SWORD;
    cards.push_back(std::move(garrison));

    auto palisade = std::make_unique<Card>("Palisade", 1, Color::RED);
    palisade->cost = {{Resource::COIN, 2}};palisade->shields = 1;palisade->link_provides = LinkSymbol::TOWER;
    cards.push_back(std::move(palisade));

    // ======================== AGE II (23 Cards) ========================
    auto sawmill = std::make_unique<Card>("Sawmill", 2, Color::BROWN);
    sawmill->cost = {{Resource::COIN, 2}}; sawmill->immediate_func = [](Player& p, Game& g){ p.add_resource(Resource::WOOD, 2, true); };
    cards.push_back(std::move(sawmill));

    auto brickyard = std::make_unique<Card>("Brickyard", 2, Color::BROWN);
    brickyard->cost = {{Resource::COIN, 2}}; brickyard->immediate_func = [](Player& p, Game& g){ p.add_resource(Resource::CLAY, 2, true); };
    cards.push_back(std::move(brickyard));

    auto shelf = std::make_unique<Card>("Shelf Quarry", 2, Color::BROWN);
    shelf->cost = {{Resource::COIN, 2}}; shelf->immediate_func = [](Player& p, Game& g){ p.add_resource(Resource::STONE, 2, true); };
    cards.push_back(std::move(shelf));

    auto glass_blower = std::make_unique<Card>("Glass Blower", 2, Color::GREY);
    glass_blower->immediate_func = [](Player& p, Game& g){ p.add_resource(Resource::GLASS, 1, true); };
    cards.push_back(std::move(glass_blower));

    auto drying_room = std::make_unique<Card>("Drying Room", 2, Color::GREY);
    drying_room->immediate_func = [](Player& p, Game& g){ p.add_resource(Resource::PAPYRUS, 1, true); };
    cards.push_back(std::move(drying_room));

    auto courthouse = std::make_unique<Card>("Courthouse", 2, Color::BLUE);
    courthouse->cost = {{Resource::WOOD, 2}, {Resource::GLASS, 1}};courthouse->victory_points = 5;
    cards.push_back(std::move(courthouse));

    auto statue = std::make_unique<Card>("Statue", 2, Color::BLUE);
    statue->cost = {{Resource::CLAY, 2}}; statue->link_prerequisite = LinkSymbol::MASK; statue->victory_points = 4; statue->link_provides = LinkSymbol::COLUMN;
    cards.push_back(std::move(statue));

    auto temple = std::make_unique<Card>("Temple", 2, Color::BLUE);
    temple->cost = {{Resource::WOOD, 1}, {Resource::PAPYRUS, 1}}; temple->link_prerequisite = LinkSymbol::MOON; temple->victory_points = 4; temple->link_provides = LinkSymbol::SUN;
    cards.push_back(std::move(temple));

    auto aqueduct = std::make_unique<Card>("Aqueduct", 2, Color::BLUE);
    aqueduct->cost_options = {Card::CostOption({{Resource::STONE, 3}}), Card::CostOption(LinkSymbol::DROP)};aqueduct->victory_points = 5;
    cards.push_back(std::move(aqueduct));

    auto library = std::make_unique<Card>("Library", 2, Color::GREEN);
    library->cost = {{Resource::STONE, 1}, {Resource::WOOD, 1}, {Resource::GLASS, 1}}; library->link_prerequisite = LinkSymbol::BOOK; library->science_symbol = Resource::QUILL; library->victory_points = 2;
    cards.push_back(std::move(library));

    auto dispensary = std::make_unique<Card>("Dispensary", 2, Color::GREEN);
    dispensary->cost = {{Resource::CLAY, 2}, {Resource::STONE, 1}}; dispensary->link_prerequisite = LinkSymbol::GEAR; dispensary->science_symbol = Resource::MORTAR;
    cards.push_back(std::move(dispensary));

    auto forum = std::make_unique<Card>("Forum", 2, Color::YELLOW);
    forum->cost = {{Resource::CLAY, 1}, {Resource::COIN, 3}}; forum->immediate_func = [](Player& p, Game& g){ p.add_resource_choice({Resource::GLASS, Resource::PAPYRUS}); }; forum->link_provides = LinkSymbol::BARREL;
    cards.push_back(std::move(forum));

    auto brewery = std::make_unique<Card>("Brewery", 2, Color::YELLOW);
    brewery->link_prerequisite = LinkSymbol::BARREL; brewery->immediate_func = [](Player& p, Game& g){ p.add_coins(6); };
    cards.push_back(std::move(brewery));

    auto caravansery = std::make_unique<Card>("Caravansery", 2, Color::YELLOW);
    caravansery->cost = {{Resource::GLASS, 1}, {Resource::PAPYRUS, 1}, {Resource::COIN, 2}}; 
    caravansery->immediate_func = [](Player& p, Game& g){ p.add_resource_choice({Resource::WOOD, Resource::CLAY, Resource::STONE}); };
    cards.push_back(std::move(caravansery));

    auto customs_house = std::make_unique<Card>("Customs House", 2, Color::YELLOW);
    customs_house->cost = {{Resource::COIN, 4}}; 
    customs_house->immediate_func = [](Player& p, Game& g){ p.set_fixed_trade_cost(Resource::GLASS, 1); p.set_fixed_trade_cost(Resource::PAPYRUS, 1); };
    cards.push_back(std::move(customs_house));

    auto walls = std::make_unique<Card>("Walls", 2, Color::RED);
    walls->cost = {{Resource::STONE, 2}}; walls->shields = 2;
    cards.push_back(std::move(walls));

    auto rostrum = std::make_unique<Card>("Rostrum", 2, Color::BLUE);
    rostrum->cost = {{Resource::WOOD, 1}, {Resource::STONE, 1}};rostrum->victory_points = 4; rostrum->link_provides = LinkSymbol::TEMPLE;
    cards.push_back(std::move(rostrum));

    auto horse_breeders = std::make_unique<Card>("Horse Breeders", 2, Color::RED);
    horse_breeders->cost = {{Resource::CLAY, 1}, {Resource::WOOD, 1}}; horse_breeders->link_prerequisite = LinkSymbol::HORSESHOE; horse_breeders->shields = 1;
    cards.push_back(std::move(horse_breeders));

    auto barracks = std::make_unique<Card>("Barracks", 2, Color::RED);
    barracks->cost = {{Resource::COIN, 3}}; barracks->link_prerequisite = LinkSymbol::SWORD; barracks->shields = 1;
    cards.push_back(std::move(barracks));

    auto archery_range = std::make_unique<Card>("Archery Range", 2, Color::RED);
    archery_range->cost = {{Resource::WOOD, 1}, {Resource::STONE, 1}, {Resource::PAPYRUS, 1}}; archery_range->shields = 2; archery_range->link_provides = LinkSymbol::TARGET;
    cards.push_back(std::move(archery_range));

    auto parade_ground = std::make_unique<Card>("Parade Ground", 2, Color::RED);
    parade_ground->cost = {{Resource::CLAY, 2}, {Resource::GLASS, 1}}; parade_ground->shields = 2; parade_ground->link_provides = LinkSymbol::HELMET;
    cards.push_back(std::move(parade_ground));

    auto school = std::make_unique<Card>("School", 2, Color::GREEN);
    school->cost = {{Resource::WOOD, 1}, {Resource::PAPYRUS, 2}}; school->science_symbol = Resource::WHEEL; school->victory_points = 1; school->link_provides = LinkSymbol::HARP;
    cards.push_back(std::move(school));

    auto laboratory = std::make_unique<Card>("Laboratory", 2, Color::GREEN);
    laboratory->cost = {{Resource::WOOD, 1}, {Resource::GLASS, 2}}; laboratory->science_symbol = Resource::ARMILLARY; laboratory->victory_points = 1; 
    cards.push_back(std::move(laboratory));


    // ======================== AGE III (20 Cards) ========================
    auto palace = std::make_unique<Card>("Palace", 3, Color::BLUE);
    palace->cost = {{Resource::STONE, 1}, {Resource::CLAY, 1}, {Resource::WOOD, 1}, {Resource::GLASS, 2}};palace->victory_points = 7;
    cards.push_back(std::move(palace));

    auto town_hall = std::make_unique<Card>("Town Hall", 3, Color::BLUE);
    town_hall->cost = {{Resource::STONE, 3}, {Resource::WOOD, 2}};town_hall->victory_points = 7;
    cards.push_back(std::move(town_hall));

    auto obelisk = std::make_unique<Card>("Obelisk", 3, Color::BLUE);
    obelisk->cost = {{Resource::STONE, 2}, {Resource::GLASS, 1}};obelisk->victory_points = 5;
    cards.push_back(std::move(obelisk));

    auto senate = std::make_unique<Card>("Senate", 3, Color::BLUE);
    senate->cost_options = {Card::CostOption({{Resource::CLAY, 2}, {Resource::STONE, 1}, {Resource::PAPYRUS, 1}}), Card::CostOption(LinkSymbol::TEMPLE)};senate->victory_points = 5;
    cards.push_back(std::move(senate));

    auto pantheon = std::make_unique<Card>("Pantheon", 3, Color::BLUE);
    pantheon->cost_options = {Card::CostOption({{Resource::CLAY, 1}, {Resource::WOOD, 1}, {Resource::PAPYRUS, 2}}), Card::CostOption(LinkSymbol::SUN)};pantheon->victory_points = 6;
    cards.push_back(std::move(pantheon));

    auto gardens = std::make_unique<Card>("Gardens", 3, Color::BLUE);
    gardens->cost_options = {Card::CostOption({{Resource::CLAY, 2}, {Resource::WOOD, 2}}), Card::CostOption(LinkSymbol::COLUMN)};
    gardens->victory_points = 6;
    cards.push_back(std::move(gardens));

    auto fortifications = std::make_unique<Card>("Fortifications", 3, Color::RED);
    fortifications->cost_options = {Card::CostOption({{Resource::STONE, 2}, {Resource::CLAY, 1}, {Resource::PAPYRUS, 1}}), Card::CostOption(LinkSymbol::TOWER)};
    fortifications->shields = 2;
    cards.push_back(std::move(fortifications));

    auto arsenal = std::make_unique<Card>("Arsenal", 3, Color::RED);
    arsenal->cost = {{Resource::WOOD, 2}, {Resource::CLAY, 3}};
    arsenal->shields = 3;
    cards.push_back(std::move(arsenal));

    auto pretorium = std::make_unique<Card>("Pretorium", 3, Color::RED);
    pretorium->cost = {{Resource::COIN, 8}};pretorium->shields = 3;
    cards.push_back(std::move(pretorium));

    auto siege_workshop = std::make_unique<Card>("Siege Workshop", 3, Color::RED);
    siege_workshop->cost_options = {Card::CostOption({{Resource::WOOD, 3}, {Resource::GLASS, 1}}), Card::CostOption(LinkSymbol::TARGET)};
    siege_workshop->shields = 2;
    cards.push_back(std::move(siege_workshop));

    auto circus = std::make_unique<Card>("Circus", 3, Color::RED);
    circus->cost_options = {Card::CostOption({{Resource::CLAY, 2}, {Resource::STONE, 2}}), Card::CostOption(LinkSymbol::HELMET)};
    circus->shields = 2;
    cards.push_back(std::move(circus));

    auto academy = std::make_unique<Card>("Academy", 3, Color::GREEN);
    academy->cost = {{Resource::WOOD, 1}, {Resource::STONE, 1}, {Resource::GLASS, 2}};academy->science_symbol = Resource::SUNDIAL;
    academy->victory_points = 3;
    cards.push_back(std::move(academy));

    auto study = std::make_unique<Card>("Study", 3, Color::GREEN);
    study->cost = {{Resource::WOOD, 2}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}};
    study->science_symbol = Resource::SUNDIAL;
    study->victory_points = 3;
    cards.push_back(std::move(study));

    auto university = std::make_unique<Card>("University", 3, Color::GREEN);
    university->cost_options = {Card::CostOption({{Resource::WOOD, 1}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}}), Card::CostOption(LinkSymbol::HARP)};
    university->science_symbol = Resource::ARCH;
    university->victory_points = 2;
    cards.push_back(std::move(university));

    auto observatory = std::make_unique<Card>("Observatory", 3, Color::GREEN);
    observatory->cost_options = {Card::CostOption({{Resource::STONE, 1}, {Resource::PAPYRUS, 2}}), Card::CostOption(LinkSymbol::LAMP)};
    observatory->science_symbol = Resource::ARCH;
    observatory->victory_points = 2;
    cards.push_back(std::move(observatory));

    auto chambre_of_commerce = std::make_unique<Card>("Chambre of Commerce", 3, Color::YELLOW);
    chambre_of_commerce->cost = {{Resource::PAPYRUS, 2}};
    chambre_of_commerce->victory_points = 3;
    chambre_of_commerce->immediate_func = [](Player& p, Game& g) {
        int grey_cards = p.get_card_count_by_color(Color::GREY);
        p.add_coins(3 * grey_cards);
    };
    cards.push_back(std::move(chambre_of_commerce));

    auto port = std::make_unique<Card>("Port", 3, Color::YELLOW);
    port->cost = {{Resource::WOOD, 1}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}};
    port->victory_points = 3;
    port->immediate_func = [](Player& p, Game& g) {
        int brown_cards = p.get_card_count_by_color(Color::BROWN);
        p.add_coins(2 * brown_cards);
    };
    cards.push_back(std::move(port));

    auto armory = std::make_unique<Card>("Armory", 3, Color::YELLOW);
    armory->cost = {{Resource::STONE, 2}, {Resource::GLASS, 1}};
    armory->victory_points = 3;
    armory->immediate_func = [](Player& p, Game& g) {
        int red_cards = p.get_card_count_by_color(Color::RED);
        p.add_coins(1 * red_cards);
    };
    cards.push_back(std::move(armory));

    auto lighthouse = std::make_unique<Card>("Lighthouse", 3, Color::YELLOW);
    lighthouse->cost_options = {Card::CostOption({{Resource::CLAY, 2}, {Resource::GLASS, 1}}), Card::CostOption(LinkSymbol::VASE)};
    lighthouse->victory_points = 3;
    lighthouse->immediate_func = [](Player& p, Game& g) {
        int yellow_cards = p.get_card_count_by_color(Color::YELLOW);
        p.add_coins(1 * yellow_cards);
    };
    cards.push_back(std::move(lighthouse));

    auto arena = std::make_unique<Card>("Arena", 3, Color::YELLOW);
    arena->cost_options = {Card::CostOption({{Resource::WOOD, 1}, {Resource::STONE, 1}, {Resource::CLAY, 1}}), Card::CostOption(LinkSymbol::BARREL)};
    arena->immediate_func = [](Player& p, Game& g) {
        int wonders = p.count_wonder_stages();
        p.add_coins(2 + 2 * wonders);
    };
    cards.push_back(std::move(arena));


    // ======================== GUILDS (7 Cards) ========================
    auto builders = std::make_unique<Card>("Builders Guild", 3, Color::PURPLE);
    builders->cost = {{Resource::STONE, 2}, {Resource::CLAY, 1}, {Resource::WOOD, 1}, {Resource::GLASS, 1}};
    builders->special_reward = Card::SpecialReward(true, Color::PURPLE, 0, 2, true, true);
    cards.push_back(std::move(builders));

    auto scientists = std::make_unique<Card>("Scientists Guild", 3, Color::PURPLE);
    scientists->cost = {{Resource::WOOD, 2}, {Resource::CLAY, 2}};
    scientists->special_reward = Card::SpecialReward(true, Color::GREEN, 0, 1, false, true);
    cards.push_back(std::move(scientists));

    auto tacticians = std::make_unique<Card>("Tacticians Guild", 3, Color::PURPLE);
    tacticians->cost = {{Resource::STONE, 2}, {Resource::CLAY, 1}, {Resource::PAPYRUS, 1}};
    tacticians->special_reward = Card::SpecialReward(true, Color::RED, 0, 1, false, true);
    cards.push_back(std::move(tacticians));

    auto merchants = std::make_unique<Card>("Merchants Guild", 3, Color::PURPLE);
    merchants->cost = {{Resource::WOOD, 1}, {Resource::CLAY, 1}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}};
    merchants->special_reward = Card::SpecialReward(true, Color::YELLOW, 0, 1, false, true);
    cards.push_back(std::move(merchants));

    auto shipowners_guild = std::make_unique<Card>("Shipowners Guild", 3, Color::PURPLE);
    shipowners_guild->cost = {{Resource::STONE, 1}, {Resource::CLAY, 1}, {Resource::GLASS, 1}, {Resource::PAPYRUS, 1}};
    shipowners_guild->special_reward = Card::SpecialReward(true, Color::BROWN, 0, 1, false, true, Color::GREY);
    cards.push_back(std::move(shipowners_guild));

    auto moneylenders_guild = std::make_unique<Card>("Moneylenders Guild", 3, Color::PURPLE);
    moneylenders_guild->cost = {{Resource::STONE, 2}, {Resource::WOOD, 2}};
    moneylenders_guild->special_reward = Card::SpecialReward(true, Color::NONE, 0, 1, false, true); // 每3金1分
    cards.push_back(std::move(moneylenders_guild));

    auto magistrates_guild = std::make_unique<Card>("Magistrates Guild", 3, Color::PURPLE);
    magistrates_guild->cost = {{Resource::WOOD, 2}, {Resource::CLAY, 1}, {Resource::PAPYRUS, 1}};
    magistrates_guild->special_reward = Card::SpecialReward(true, Color::BLUE, 0, 1, false, true);
    cards.push_back(std::move(magistrates_guild));

    return cards;
}