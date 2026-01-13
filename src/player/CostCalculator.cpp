#include "CostCalculator.h"
#include "player/Player.h"
#include "cards/Card.h"
#include <algorithm>
#include <map>

bool CostCalculator::is_tradable_resource(Resource res) {
    return (res == Resource::WOOD || res == Resource::CLAY || res == Resource::STONE ||
            res == Resource::GLASS || res == Resource::PAPYRUS);
}

int CostCalculator::calculate_trade_cost(const Player& buyer, const Player& seller, Resource res) {
    int base = buyer.get_trade_cost(res);
    // 如果已有储备卡 (Base = 1)，按规则固定为 1 (规则书 P8)
    if (base == 1) return 1;
    
    // 基础费用 2 + 对手城市中该资源符号的数量 (规则书 P8)
    // 注意：仅统计褐色和灰色卡牌的产出 (get_raw_resource)
    return 2 + seller.get_raw_resource(res);
}

CostCalculator::BuildCostResult CostCalculator::calculate_specific_cost(
    const Player& player, const Player& opponent, 
    const std::map<Resource, int>& cost_map, 
    LinkSymbol link,
    int resource_discount
) {
    BuildCostResult result;
    result.can_build = true;
    result.total_coin_cost = 0;

    // 1. 连锁免费判定
    if (link != LinkSymbol::NONE && player.has_chain_symbol(link)) {
        result.is_free_by_chain = true;
        return result;
    }

    // 2. 初始金币成本
    auto it_coin = cost_map.find(Resource::COIN);
    if (it_coin != cost_map.end()) {
        result.total_coin_cost += it_coin->second;
    }

    // 3. 建立临时缺口统计 Map
    std::map<Resource, int> current_shortages;
    for (auto const& [res, req] : cost_map) {
        if (res == Resource::COIN) continue;
        int produced = player.get_resource(res);
        if (produced < req) {
            current_shortages[res] = req - produced;
        }
    }

    // --- 新增：进度标记减免 (MASONRY/ARCHITECTURE) ---
    int discount_left = resource_discount;
    for (auto& [res, amt] : current_shortages) {
        if (discount_left <= 0) break;
        int used = std::min(amt, discount_left);
        amt -= used;
        discount_left -= used;
    }

    // 4. 多选一资源抵扣 (Safe Loop)
    const auto& wildcards = player.get_wildcard_resources();
    for (const auto& options : wildcards) {
        bool used_this_wildcard = false;
        for (Resource opt : options) {
            if (!used_this_wildcard && current_shortages.count(opt) && current_shortages[opt] > 0) {
                current_shortages[opt]--;
                used_this_wildcard = true; 
            }
        }
    }

    // 5. 计算剩余缺口的购买费
    for (auto const& [res, amt] : current_shortages) {
        if (amt <= 0) continue;
        if (is_tradable_resource(res)) {
            int price = calculate_trade_cost(player, opponent, res);
            int fee = price * amt;
            result.total_coin_cost += fee;
            result.trade_coin_cost += fee;
        } else {
            result.can_build = false; 
            return result;
        }
    }

    // 6. 最终余额判定
    if (player.get_coins() < result.total_coin_cost) {
        result.can_build = false;
    }

    return result;
}

CostCalculator::BuildCostResult CostCalculator::calculate_build_cost(
    const Player& player, const Player& opponent, const Card& card
) {
    int discount = 0;
    if (card.color == Color::BLUE && player.has_progress_token(ProgressToken::MASONRY)) {
        discount = 2;
    }

    // 优先处理多选一方案 (cost_options)
    if (!card.cost_options.empty()) {
        BuildCostResult best_result;
        best_result.can_build = false;
        best_result.total_coin_cost = 2000000000; // 足够大的值

        for (const auto& opt : card.cost_options) {
            BuildCostResult current = calculate_specific_cost(player, opponent, opt.resources, opt.link, discount);
            
            // 如果能通过连锁直接免费，则是最优解
            if (current.is_free_by_chain) return current;

            // 寻找最便宜的方案
            if (current.can_build && current.total_coin_cost < best_result.total_coin_cost) {
                best_result = current;
            }
        }
        return best_result;
    }

    // 如果没有 cost_options，回退到 legacy 的单方案逻辑
    return calculate_specific_cost(player, opponent, card.cost, card.link_prerequisite, discount);
}

bool CostCalculator::can_afford_with_trade(const Player& player, const Player& opponent, const Card& card) {
    return calculate_build_cost(player, opponent, card).can_build;
}

bool CostCalculator::execute_build(Player& player, Player& opponent, const Card& card) {
    BuildCostResult res = calculate_build_cost(player, opponent, card);
    if (!res.can_build) return false;
    
    // 执行扣款
    if (res.total_coin_cost > 0) {
        if (!player.spend_coins(res.total_coin_cost)) return false;
    }

    // ECONOMY (经济) 判定: 购买资源的钱付给对手
    if (res.trade_coin_cost > 0 && opponent.has_progress_token(ProgressToken::ECONOMY)) {
        opponent.add_coins(res.trade_coin_cost);
    }

    // URBANISM (城市化): 如果是通过连锁符号免费建造，奖励 4 元
    if (res.is_free_by_chain && player.has_progress_token(ProgressToken::URBANISM)) {
        player.add_coins(4);
    }

    return true;
}

bool CostCalculator::execute_wonder_build(Player& player, Player& opponent, const std::map<Resource, int>& wonder_cost) {
    int discount = 0;
    if (player.has_progress_token(ProgressToken::ARCHITECTURE)) {
        discount = 2;
    }
    // 奇迹没有连锁符号，所以 link 传入 NONE
    BuildCostResult res = calculate_specific_cost(player, opponent, wonder_cost, LinkSymbol::NONE, discount);
    if (!res.can_build) return false;
    
    if (res.total_coin_cost > 0) {
        if (!player.spend_coins(res.total_coin_cost)) return false;
    }

    // ECONOMY (经济) 判定
    if (res.trade_coin_cost > 0 && opponent.has_progress_token(ProgressToken::ECONOMY)) {
        opponent.add_coins(res.trade_coin_cost);
    }

    return true;
}