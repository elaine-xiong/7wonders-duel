#ifndef CARD_H
#define CARD_H

#include "Types.h"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>

class Player;
class Game;

class Card {
public:
    using CardEffect = std::function<void(Player&, Game&)>; //即时效应奖励

    // --- 特殊奖励 (用于黄色/紫色卡) ---
    struct SpecialReward {
        bool active;          
        Color target_color;   
        Color secondary_target_color; // 新增：用于船主公会 (BROWN + GREY)
        int coins_per_card;   
        int vp_per_card;      
        bool count_wonders;   
        bool count_both;      

        SpecialReward() : active(false), target_color(Color::BROWN), secondary_target_color(Color::NONE), 
                         coins_per_card(0), vp_per_card(0), count_wonders(false), count_both(false) {}
        
        SpecialReward(bool a, Color c, int coins, int vp, bool wonders, bool both = false, Color c2 = Color::NONE)
            : active(a), target_color(c), secondary_target_color(c2), 
              coins_per_card(coins), vp_per_card(vp), count_wonders(wonders), count_both(both) {}
    };

    // --- 不同成本选项 ---
    struct CostOption {
        std::map<Resource, int> resources;
        LinkSymbol link = LinkSymbol::NONE;
        CostOption() = default;
        CostOption(std::map<Resource, int> r) : resources(std::move(r)), link(LinkSymbol::NONE) {}
        CostOption(LinkSymbol l) : resources({}), link(l) {}
    };

    // --- 基础属性 ---
    std::string name;
    int age; //卡牌所属时代
    Color color;
    std::map<Resource, int> cost; //建造卡牌需要的资源和资源数量
    std::vector<CostOption> cost_options; //不同成本选项
    
    // --- 奖励效果（用于红色/绿色卡）---
    int victory_points = 0; //卡牌提供的胜利点数
    int shields = 0;  //红色牌提供的军事盾牌数量
    Resource science_symbol = Resource::COIN; // 绿色牌提供的科学符号，若非科技牌则为 COIN

    // --- 连锁系统 ---
    LinkSymbol link_prerequisite = LinkSymbol::NONE; 
    LinkSymbol link_provides = LinkSymbol::NONE;

    SpecialReward special_reward;
    CardEffect immediate_func; // 即时奖励
    bool is_face_up = false; //卡牌是否正面超上，金字塔结构判断

    Card(std::string n, int a, Color c);

    bool can_be_free(const Player& p) const; //通过连锁反应有的卡牌免费建造
    void apply_effect(Player& p, Game& g) const; //触发即时奖励
};

std::vector<std::unique_ptr<Card>> createAllCards();

#endif