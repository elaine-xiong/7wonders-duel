#pragma once
#include <vector>
#include <memory>
#include <string>
#include "Types.h" // 核心：包含所有枚举，如 ProgressToken
#include "cards/Card.h"
#include "cards/Wonder.h"

// 前向声明
class Board;
class Player;
class CardStructure;
class Controller;

class Game {
private:
    static Game* instance;
    Game(); 

    std::unique_ptr<Board> board;
    std::vector<std::shared_ptr<Player>> players;
    std::unique_ptr<CardStructure> cardStructure;
    
    int current_age;
    int current_player_idx;
    bool is_game_over;
    bool extra_turn_triggered; 

    std::vector<std::unique_ptr<Card>> discard_pile; 
    std::vector<ProgressToken> progress_token_pool;   
    int total_wonders_built = 0; // 新增：记录全场已建成的奇迹总数

    // 内部私有辅助
    void setup_age_structure(int age);
    void handle_turn_switch();
    void distribute_wonders(); 

public:
    static Game& getInstance();
    Game(const Game&) = delete;
    void operator=(const Game&) = delete;

    void init(); 
    void run();  
    void end_age(); 

    // --- 核心动作 (对齐 snake_case) ---
    bool take_card(int pos, Player& player);
    bool build_wonder(int wonder_idx, int pos, Player& player);
    void discard_for_coins(int pos, Player& player);

    // --- 状态检查 ---
    bool check_supremacy_victory(); 
    void check_science_victory(Player& p);

    // --- Getter & Setter (对齐 snake_case) ---
    Board* get_board() { return board.get(); }
    Player* get_current_player();
    
    // 获取当前非回合玩家
    Player* get_opponent(); 
    // 获取指定玩家的对手 (解决 ctrller.cpp 报错)
    Player* get_opponent(Player& p); 
    Player* get_player(int index);

    CardStructure& get_structure() { return *cardStructure; } 
    int get_current_age() const { return current_age; }
    
    // --- 供 Wonder/Card 调用的回调接口 ---
    void set_extra_turn(bool status) { extra_turn_triggered = status; }
    void move_pawn(int steps); 
    
    // 交互触发
    void trigger_progress_token_selection(Player& p, int count);
    void trigger_build_from_discard(Player& p);
    
    // 获取弃牌堆视图
    std::vector<Card*> get_discard_pile_view(); 

    ~Game() = default;
};