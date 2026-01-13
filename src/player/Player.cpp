#include "Player.h"
#include "cards/Card.h"
#include "core/Game.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

// --- 构造函数 ---
// 初始化所有基础数值，确保不产生随机垃圾值
Player::Player(const std::string& playerName, PlayerType playerType) 
    : name(playerName), type(playerType), coins(7), 
      military_tokens(0), victory_points(0), built_wonders_count(0) {
}

// --- 经济管理 ---

void Player::add_coins(int amount) {
    // 允许传入负数进行扣款，并确保余额不会低于 0（规则书 P14 保护逻辑）
    coins += amount;
    if (coins < 0) coins = 0; 
}

bool Player::spend_coins(int amount) {
    if (coins < amount) return false;
    coins -= amount;
    return true;
}

// --- 资源产出与交易逻辑 ---

void Player::add_resource(Resource res, int amount, bool is_raw) {
    if (amount > 0) {
        resources[res] += amount;
        if (is_raw) raw_resources[res] += amount;
    }
}

int Player::get_resource(Resource res) const {
    auto it = resources.find(res);
    return (it != resources.end()) ? it->second : 0;
}

int Player::get_raw_resource(Resource res) const {
    auto it = raw_resources.find(res);
    return (it != raw_resources.end()) ? it->second : 0;
}

void Player::add_resource_choice(const std::set<Resource>& options) {
    if (!options.empty()) {
        wildcard_resources.push_back(options);
    }
}

void Player::set_fixed_trade_cost(Resource res, int cost) {
    fixed_trade_costs[res] = cost;
}

int Player::get_trade_cost(Resource res) const {
    // 如果玩家拥有对应的黄色“储备卡”，该资源的交易基础费固定为 1（规则书 P8）
    auto it = fixed_trade_costs.find(res);
    if (it != fixed_trade_costs.end()) {
        return it->second;
    }
    return 2; // 默认规则：基础费为 2
}

// --- 卡牌管理与统计 ---

void Player::add_built_card(std::unique_ptr<Card> card) {
    cards_by_color[card->color]++;
    built_cards.push_back(std::move(card));
}

bool Player::has_card(const std::string& cardName) const {
    for (const auto& card : built_cards) {
        if (card->name == cardName) return true;
    }
    return false;
}

int Player::get_card_count_by_color(Color color) const {
    auto it = cards_by_color.find(color);
    if (it != cards_by_color.end()) {
        return it->second;
    }
    return 0;
}

// 统计快捷函数实现 (用于时代 III / 公会卡关联计分)
int Player::count_brown() const  { return get_card_count_by_color(Color::BROWN); }
int Player::count_grey() const   { return get_card_count_by_color(Color::GREY); }
int Player::count_yellow() const { return get_card_count_by_color(Color::YELLOW); }
int Player::count_blue() const   { return get_card_count_by_color(Color::BLUE); }
int Player::count_green() const  { return get_card_count_by_color(Color::GREEN); }
int Player::count_red() const    { return get_card_count_by_color(Color::RED); }
int Player::count_purple() const { return get_card_count_by_color(Color::PURPLE); }

// --- 连锁符号逻辑 (Linking) ---

void Player::add_chain_symbol(LinkSymbol symbol) {
    if (symbol != LinkSymbol::NONE) {
        owned_link_symbols.insert(symbol);
    }
}

bool Player::has_chain_symbol(LinkSymbol symbol) const {
    return owned_link_symbols.count(symbol) > 0;
}

// --- 奇迹管理 (彻底修复 undefined reference 报错) ---

void Player::add_wonder(const Wonder& w) {
    wonders.push_back(w);
}

// 补全 get_wonder 实现
Wonder& Player::get_wonder(int idx) {
    if (idx < 0 || idx >= (int)wonders.size()) {
        throw std::out_of_range("Player::get_wonder - Index out of range");
    }
    return wonders[idx];
}

int Player::count_wonder_stages() const {
    return built_wonders_count;
}

// 补全 increment_wonder_count 实现
void Player::increment_wonder_count() {
    built_wonders_count++;
}

// --- 科技、军事与特殊效果 ---

bool Player::add_science_symbol(Resource symbol) {
    // 判定是否属于科技符号区间 (ARMILLARY 到 LAW)
    if (symbol >= Resource::ARMILLARY && symbol <= Resource::LAW) {
        science_symbols[symbol]++;
        // 如果某种符号达到 2 个，表示形成了一对，触发 Progress Token 选择
        if (science_symbols[symbol] == 2) {
            return true;
        }
    }
    return false;
}

int Player::get_unique_science_count() const {
    // 返回 set 的大小即为不同科技符号的数量 (规则书 P12)
    // 注意：LAW 标记在获得时已通过 add_science_symbol 加入了 map，所以 size() 已包含它
    return static_cast<int>(science_symbols.size());
}

void Player::destroy_card_by_color(Color color) {
    // 奇迹破坏效果：减少对手某色卡牌计数
    auto it = cards_by_color.find(color);
    if (it != cards_by_color.end() && it->second > 0) {
        it->second--;
        std::cout << "[Effect] " << name << " lost a card of color " << (int)color << std::endl;
    }
}

// --- 最终结算 ---

int Player::calculate_final_score(const Player& opponent) const {
    // 1. 基础胜利点数 (来自蓝卡、红卡、绿卡[已有VP]、奇迹[已有VP]、科技标记[已有VP]的直接加分)
    int total = victory_points;

    // 2. 现金换分：每 3 元换 1 分 (规则书 P13)
    total += (coins / 3);

    // 3. 科技标记加分：MATHEMATICS (数学) 每个标记 3 分
    if (has_progress_token(ProgressToken::MATHEMATICS)) {
        total += (static_cast<int>(owned_tokens.size()) * 3);
    }

    // 4. 公会卡计分 (紫色卡)
    for (const auto& card : built_cards) {
        if (card->color == Color::PURPLE) {
            const auto& sr = card->special_reward;
            if (!sr.active) continue;

            int my_count = 0;
            int opp_count = 0;

            if (sr.count_wonders) {
                my_count = this->built_wonders_count;
                opp_count = opponent.get_built_wonders_count();
            } else if (card->name == "Moneylenders Guild") {
                my_count = this->coins / 3;
                opp_count = opponent.get_coins() / 3;
            } else {
                // 通用逻辑：按颜色统计
                my_count = this->get_card_count_by_color(sr.target_color);
                opp_count = opponent.get_card_count_by_color(sr.target_color);
                
                // 处理双色情况 (船主公会: 棕+灰)
                if (sr.secondary_target_color != Color::NONE) {
                    my_count += this->get_card_count_by_color(sr.secondary_target_color);
                    opp_count += opponent.get_card_count_by_color(sr.secondary_target_color);
                }
            }

            // 公会卡计分规则：取双方中数量较多的一方
            total += std::max(my_count, opp_count) * sr.vp_per_card;
        }
    }

    return total;
}

void Player::add_progress_token(ProgressToken token) {
    owned_tokens.insert(token);
    // 立即执行即时性效果
    if (token == ProgressToken::PHILOSOPHY) {
        add_victory_points(7);
    }
    // AGRICULTURE (农业): 立即获得 6 元，并提供 4 VP (结尾算)
    if (token == ProgressToken::AGRICULTURE) {
        add_coins(6);
        add_victory_points(4);
    }
    // LAW (法律): 获得一个额外的科学符号 (天平)
    if (token == ProgressToken::LAW) {
        add_science_symbol(Resource::LAW);
    }
}

bool Player::has_progress_token(ProgressToken token) const {
    return owned_tokens.count(token) > 0;
}

int Player::get_military_tokens() const {
    return military_tokens;
}