#include "Game.h"
#include "Board.h"
#include "player/Player.h"
#include "player/CostCalculator.h"
#include "cards/Card.h"
#include "cards/CardStructure.h"
#include "view/Ctrller.h"
#include <iostream>
#include <algorithm>
#include <random>

// 1. 核心修复：初始化单例静态指针 (解决 Error 1 / ld 报错)
Game* Game::instance = nullptr;

Game& Game::getInstance() {
    if (instance == nullptr) {
        instance = new Game();
    }
    return *instance;
}

Game::Game() : board(std::make_unique<Board>()), 
               current_age(1), 
               current_player_idx(0), 
               is_game_over(false), 
               extra_turn_triggered(false) {}

void Game::init() {
    std::cout << "[Game] Initializing 7 Wonders Duel..." << std::endl;
    
    // 初始化玩家
    players.clear();
    players.push_back(std::make_shared<Player>("Player 1"));
    players.push_back(std::make_shared<Player>("Player 2"));

    // 规则书 P6：初始金币为 7
    // Player 构造函数中已经初始化为 7，此处无需再加
    /* 
    for(auto& p : players) {
        p->add_coins(7); 
    }
    */

    // 注解掉自动分发逻辑，改为由 Controller 手动执行挑选阶段
    // distribute_wonders();

    // 初始化科技标记 (10种)
    std::vector<ProgressToken> all_tokens = {
        ProgressToken::AGRICULTURE, ProgressToken::ARCHITECTURE, ProgressToken::ECONOMY,
        ProgressToken::LAW, ProgressToken::MASONRY, ProgressToken::MATHEMATICS,
        ProgressToken::PHILOSOPHY, ProgressToken::STRATEGY, ProgressToken::THEOLOGY,
        ProgressToken::URBANISM
    };
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(all_tokens.begin(), all_tokens.end(), g);
    
    // 5个放在板子上，剩余5个作为备选池 (针对大图书馆等效果)
    std::vector<ProgressToken> board_tokens(all_tokens.begin(), all_tokens.begin() + 5);
    progress_token_pool.assign(all_tokens.begin() + 5, all_tokens.end());
    board->setup_progress_tokens(board_tokens);

    // 构建初始时代布局
    setup_age_structure(1);
}

void Game::distribute_wonders() {
    auto all_wonders = createAllWonders();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(all_wonders.begin(), all_wonders.end(), g);
    
    // 规则 P7：给 P1 前 4 个，P2 后 4 个
    for(int i = 0; i < 4; ++i) players[0]->add_wonder(all_wonders[i]);
    for(int i = 4; i < 8; ++i) players[1]->add_wonder(all_wonders[i]);
}

void Game::setup_age_structure(int age) {
    auto all_cards = createAllCards();
    std::vector<std::unique_ptr<Card>> age_deck;

    std::random_device rd;
    std::mt19937 g(rd());

    if (age == 1 || age == 2) {
        // 时代 I & II: 从 23 张中随机取 20 张
        for (auto& c : all_cards) {
            if (c->age == age) age_deck.push_back(std::move(c));
        }
        std::shuffle(age_deck.begin(), age_deck.end(), g);
        if (age_deck.size() > 20) age_deck.resize(20);
    } else if (age == 3) {
        // 时代 III: 17张时代III卡 + 3张随机公会卡
        std::vector<std::unique_ptr<Card>> age_iii_normal;
        std::vector<std::unique_ptr<Card>> guilds;

        for (auto& c : all_cards) {
            if (c->age == 3) {
                if (c->color == Color::PURPLE) guilds.push_back(std::move(c));
                else age_iii_normal.push_back(std::move(c));
            }
        }

        std::shuffle(age_iii_normal.begin(), age_iii_normal.end(), g);
        std::shuffle(guilds.begin(), guilds.end(), g);

        // 取 17 张普通，3 张公会
        if (age_iii_normal.size() > 17) age_iii_normal.resize(17);
        if (guilds.size() > 3) guilds.resize(3);

        for (auto& c : age_iii_normal) age_deck.push_back(std::move(c));
        for (auto& c : guilds) age_deck.push_back(std::move(c));
        
        std::shuffle(age_deck.begin(), age_deck.end(), g);
    }

    std::cout << "[DEBUG] Loading Age " << age << ", Final deck size: " << age_deck.size() << std::endl;

    if (age_deck.size() != 20) {
        std::cerr << "Fatal Error: Age deck must be exactly 20 cards!" << std::endl;
        exit(1);
    }
    
    cardStructure = std::make_unique<CardStructure>(age, std::move(age_deck));
}

// --- 核心动作 1：购买/建造卡牌 ---
bool Game::take_card(int pos, Player& player) {
    const Card* card_ptr = cardStructure->get_card(pos);
    if (!card_ptr) return false;

    // 支付逻辑（调用 CostCalculator，含连锁检查）
    if (!CostCalculator::execute_build(player, *get_opponent(player), *card_ptr)) {
        return false;
    }

    // 从金字塔移走卡牌并获取所有权
    std::unique_ptr<Card> card = cardStructure->take_card(pos);

    // 执行结构化效果 (VP, 盾牌, 符号)
    card->apply_effect(player, *this);
    player.add_built_card(std::move(card));

    if (check_supremacy_victory()) return true;

    handle_turn_switch();
    return true;
} 

// --- 核心动作 2：弃牌换钱 ---
void Game::discard_for_coins(int pos, Player& player) {
    std::unique_ptr<Card> card = cardStructure->take_card(pos);
    if (!card) return;

    // 规则 P10：基础 2 金 + 拥有的黄卡数量
    int gain = 2 + player.count_yellow();
    player.add_coins(gain);
    
    discard_pile.push_back(std::move(card));
    std::cout << "[Game] " << player.get_name() << " gained " << gain << " coins." << std::endl;
    
    handle_turn_switch();
}

// --- 核心动作 3：建造奇迹 ---
bool Game::build_wonder(int wonder_idx, int pos, Player& player) {
    if (total_wonders_built >= 7) {
        std::cout << "[Game] Limit Reacted: 7 Wonders already built. You cannot build the 8th." << std::endl;
        return false;
    }

    Wonder& wonder = player.get_wonder(wonder_idx);
    
    // 检查奇迹状态及金字塔是否有地基
    if (wonder.is_built || !cardStructure->get_card(pos)) return false;

    // 检查费用
    if (!CostCalculator::execute_wonder_build(player, *get_opponent(player), wonder.cost)) {
        return false;
    }

    // 取走卡牌作为地基（面朝下）
    std::unique_ptr<Card> foundation = cardStructure->take_card(pos);
    discard_pile.push_back(std::move(foundation));

    // 应用奇迹结构化效果
    player.add_victory_points(wonder.victory_points);
    if (wonder.shields > 0) move_pawn(wonder.shields);
    
    // 执行 Lambda 效果
    if (wonder.effect) {
        wonder.effect(player, *get_opponent(), *this);
    }

    wonder.is_built = true;
    player.increment_wonder_count();
    total_wonders_built++;

    handle_turn_switch();
    return true;
}

void Game::handle_turn_switch() {
    if (extra_turn_triggered) {
        std::cout << ">>> EXTRA TURN! <<<" << std::endl;
        extra_turn_triggered = false; 
    } else {
        current_player_idx = (current_player_idx + 1) % 2;
    }
}

void Game::move_pawn(int steps) {
    // P1 推向 P2 (+), P2 推向 P1 (-)
    int direction = (current_player_idx == 0) ? 1 : -1;
    if (board->move_pawn(steps * direction, *players[0], *players[1])) {
        is_game_over = true;
    }
}

bool Game::check_supremacy_victory() {
    // 军事压制
    if (board->get_pawn_position() <= 0 || board->get_pawn_position() >= 18) return true;
    // 科技压制
    if (get_current_player()->get_unique_science_count() >= 6) return true;
    return false;
}

void Game::run() {
    init();
    Controller controller(*this); 

    // 官方规则：开局一次性进行两轮奇迹挑选
    auto wonder_pool = createAllWonders();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(wonder_pool.begin(), wonder_pool.end(), g);

    std::cout << "\n--- Wonder Selection Phase: Round 1 ---" << std::endl;
    controller.draft_wonders(1, wonder_pool);
    std::cout << "\n--- Wonder Selection Phase: Round 2 ---" << std::endl;
    controller.draft_wonders(2, wonder_pool);

    while (!is_game_over && current_age <= 3) {
        while (!cardStructure->is_empty() && !is_game_over) { 
            controller.player_turn(*get_current_player());
            if (check_supremacy_victory()) {
                is_game_over = true;
                break;
            }
        }

        if (!is_game_over) {
            current_age++;
            if (current_age <= 3) {
                std::cout << "\n--- AGE " << (current_age - 1) << " ENDED. Determination of first player for Age " << current_age << " ---" << std::endl;
                
                int pos = board->get_pawn_position();
                int chooser_idx = -1;
                
                if (pos < 9) chooser_idx = 0;      // 棋子在 P1 侧，P1 选择
                else if (pos > 9) chooser_idx = 1; // 棋子在 P2 侧，P2 选择
                else {
                    // 棋子在中点，由刚才出最后一张牌的玩家选择
                    // 在 handle_turn_switch 之后，current_player_idx 指向的是没出牌的那个人
                    chooser_idx = (current_player_idx + 1) % 2;
                }

                std::cout << players[chooser_idx]->get_name() << ", choose who starts Age " << current_age << ":" << std::endl;
                std::cout << "0: " << players[0]->get_name() << "\n1: " << players[1]->get_name() << "\nChoice: ";
                int choice;
                if (!(std::cin >> choice) || (choice != 0 && choice != 1)) {
                    std::cin.clear();
                    std::cin.ignore(1000, '\n');
                    choice = chooser_idx; // 默认自己先
                }
                current_player_idx = choice;
                
                std::cout << "\n--- Starting Age " << current_age << " ---" << std::endl;
                setup_age_structure(current_age);
            }
        }
    }
    
    if (board->get_pawn_position() <= 0) {
        std::cout << "\nMILITARY SUPREMACY! " << players[1]->get_name() << " Wins!" << std::endl;
        return;
    }
    if (board->get_pawn_position() >= 18) {
        std::cout << "\nMILITARY SUPREMACY! " << players[0]->get_name() << " Wins!" << std::endl;
        return;
    }
    if (players[0]->get_unique_science_count() >= 6) {
        std::cout << "\nSCIENTIFIC SUPREMACY! " << players[0]->get_name() << " Wins!" << std::endl;
        return;
    }
    if (players[1]->get_unique_science_count() >= 6) {
        std::cout << "\nSCIENTIFIC SUPREMACY! " << players[1]->get_name() << " Wins!" << std::endl;
        return;
    }

    int score0 = players[0]->calculate_final_score(*players[1]) + board->get_military_vp(0);
    int score1 = players[1]->calculate_final_score(*players[0]) + board->get_military_vp(1);

    std::cout << "\n--- GAME OVER! ---\n";
    std::cout << players[0]->get_name() << " Final Score: " << score0 << "\n";
    std::cout << players[1]->get_name() << " Final Score: " << score1 << "\n";

    if (score0 > score1) {
        std::cout << "Winner: " << players[0]->get_name() << "!" << std::endl;
    } else if (score1 > score0) {
        std::cout << "Winner: " << players[1]->get_name() << "!" << std::endl;
    } else {
        std::cout << "It's a Tie!" << std::endl;
    }
}

// --- Getter 组 (对齐 snake_case) ---

Player* Game::get_current_player() { return players[current_player_idx].get(); }
Player* Game::get_opponent() { return players[(current_player_idx + 1) % 2].get(); }

Player* Game::get_opponent(Player& p) {
    return (players[0].get() == &p) ? players[1].get() : players[0].get();
}

Player* Game::get_player(int index) {
    if (index < 0 || index >= (int)players.size()) return nullptr;
    return players[index].get();
}

// --- 奇迹效果回调接口 ---

std::string get_token_name(ProgressToken t) {
    switch (t) {
        case ProgressToken::AGRICULTURE: return "Agriculture (+6 coins, 4 VP)";
        case ProgressToken::ARCHITECTURE: return "Architecture (Wonder resource discount)";
        case ProgressToken::ECONOMY: return "Economy (Profit from opponent trade)";
        case ProgressToken::LAW: return "Law (Science symbol)";
        case ProgressToken::MASONRY: return "Masonry (Blue card resource discount)";
        case ProgressToken::MATHEMATICS: return "Mathematics (3 VP per token)";
        case ProgressToken::PHILOSOPHY: return "Philosophy (7 VP)";
        case ProgressToken::STRATEGY: return "Strategy (+1 Shield per army)";
        case ProgressToken::THEOLOGY: return "Theology (All wonders provide extra turn)";
        case ProgressToken::URBANISM: return "Urbanism (4 coins per linked build)";
        default: return "Unknown";
    }
}

void Game::trigger_progress_token_selection(Player& p, int count) {
    std::vector<ProgressToken> choices;
    bool from_board = (count == 1);

    if (from_board) {
        choices = board->get_active_progress_tokens();
    } else {
        choices = progress_token_pool;
        if (choices.size() > (size_t)count) choices.resize(count);
    }

    if (choices.empty()) {
        std::cout << "[INFO] No Progress Tokens available to select." << std::endl;
        return;
    }

    std::cout << "\n*** SELECT PROGRESS TOKEN for " << p.get_name() << " ***" << std::endl;
    for (int i = 0; i < (int)choices.size(); ++i) {
        std::cout << i << ": " << get_token_name(choices[i]) << "\n";
    }
    std::cout << "Enter choice index: ";
    int idx;
    if (!(std::cin >> idx) || idx < 0 || idx >= (int)choices.size()) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        return;
    }

    ProgressToken selected = choices[idx];
    p.add_progress_token(selected);
    
    // 立即执行一次性效果
    if (selected == ProgressToken::AGRICULTURE) {
        p.add_coins(6);
    }
    
    if (from_board) {
        board->remove_progress_token(selected);
    } else {
        auto it = std::find(progress_token_pool.begin(), progress_token_pool.end(), selected);
        if (it != progress_token_pool.end()) progress_token_pool.erase(it);
    }
    
    std::cout << "[Game] " << p.get_name() << " acquired: " << get_token_name(selected) << std::endl;
}

void Game::trigger_build_from_discard(Player& p) {
    if (discard_pile.empty()) {
        std::cout << "[Effect] Discard pile is empty. No card to build." << std::endl;
        return;
    }

    std::cout << "\n============================================\n";
    std::cout << "   EFFECT: BUILD FROM DISCARD PILE\n";
    std::cout << "============================================\n";
    for (int i = 0; i < (int)discard_pile.size(); ++i) {
        std::cout << i << ": " << discard_pile[i]->name << " (Age " << discard_pile[i]->age << ")\n";
    }
    std::cout << "\nEnter choice: ";
    int choice;
    if (!(std::cin >> choice) || choice < 0 || choice >= (int)discard_pile.size()) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "[Effect] Invalid choice. Effect skipped." << std::endl;
        return;
    }

    auto card = std::move(discard_pile[choice]);
    discard_pile.erase(discard_pile.begin() + choice);

    std::cout << "[Effect] Building " << card->name << "..." << std::endl;
    card->apply_effect(p, *this);
    p.add_built_card(std::move(card));
}

std::vector<Card*> Game::get_discard_pile_view() {
    std::vector<Card*> view;
    for(auto& c : discard_pile) view.push_back(c.get());
    return view;
}

void Game::check_science_victory(Player& p) {
    if (p.get_unique_science_count() >= 6) is_game_over = true;
}