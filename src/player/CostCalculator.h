#ifndef COSTCALCULATOR_H
#define COSTCALCULATOR_H

#include <map>
#include <vector>
#include "../Types.h"

// 前向声明，减少物理依赖并防止循环引用
class Player;
class Card;
enum class Resource;

/**
 * CostCalculator 类：处理建造成本计算的核心逻辑
 * 职责：计算资源缺口、处理多选一资源抵扣、计算交易费用、验证支付能力
 */
class CostCalculator {
public:
    /**
     * BuildCostResult: 存储建造成本计算的详细结果
     */
    struct BuildCostResult {
        bool can_build;                 // 最终判定：玩家当前是否能买得起/建得起
        bool is_free_by_chain;          // 是否通过白色连锁符号（LinkSymbol）免费建造
        int total_coin_cost;            // 总金币支出（卡牌标价 + 资源购买费）
        int trade_coin_cost;            // 仅资源购买费（用于 ECONOMY 标记）

        // 构造函数初始化默认值
        BuildCostResult() 
            : can_build(true), is_free_by_chain(false), total_coin_cost(0), trade_coin_cost(0) {}
    };

    /**
     * 计算从对手购买单个缺口资源的单价
     * 逻辑：如果玩家有Reserve（储备）卡则为1，否则为 2 + 对手对应的资源产出卡数
     */
    static int calculate_trade_cost(const Player& buyer, const Player& seller, Resource resource);

    /**
     * 核心计算函数：分析一张卡牌的完整建造成本
     * 流程：连锁判定 -> 基础金币统计 -> 资源缺口统计 -> 多选一资源抵扣 -> 交易费折算
     */
    static BuildCostResult calculate_build_cost(const Player& player, const Player& opponent, const Card& card);

    /**
     * 简易接口：仅判定玩家是否能负担得起这张牌（不涉及扣款）
     */
    static bool can_afford_with_trade(const Player& player, const Player& opponent, const Card& card);

    /**
     * 执行建造逻辑：计算成本，并从 Player 账户中扣除相应金币
     * @return 扣款是否成功（如果期间余额不足会返回 false）
     */
    static bool execute_build(Player& player, Player& opponent, const Card& card);

    /**
     * 执行奇迹建造逻辑：由于奇迹没有连锁符号且是特定的资源需求列表，
     * 所以需要一个直接接受资源 map 的接口。
     */
    static bool execute_wonder_build(Player& player, Player& opponent, const std::map<Resource, int>& wonder_cost);

private:
    static bool is_tradable_resource(Resource resource);

    // 新增：计算具体某种资源 map + 连锁符号组合的成本
    static BuildCostResult calculate_specific_cost(const Player& player, const Player& opponent, 
                                                 const std::map<Resource, int>& cost_map, 
                                                 LinkSymbol link,
                                                 int resource_discount = 0);
};

#endif // COSTCALCULATOR_H