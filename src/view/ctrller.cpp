#include "../view/Ctrller.h"
#include "ConsoleView.h"
#include "../core/Game.h"
#include "../player/Player.h"
#include "../cards/CardStructure.h"
#include "../cards/Wonder.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <thread> 
#include <chrono>
#include <random>

Controller::Controller(Game& game) : game(game), view(std::make_unique<ConsoleView>()) {}
Controller::~Controller() = default;

void Controller::draft_wonders(int round, std::vector<Wonder>& pool) {
    clearScreen();
    std::cout << "======================================================\n";
    std::cout << "          WONDER SELECTION PHASE - ROUND " << round << " \n";
    std::cout << "======================================================\n";

    // 我们获取玩家实例指针
    Player* p1 = game.get_current_player(); 
    Player* p2 = game.get_opponent(*p1);

    // 每一轮选 4 张
    std::vector<Wonder> current_round;
    for(int i=0; i<4; ++i) {
        if(!pool.empty()) {
            current_round.push_back(pool.back());
            pool.pop_back();
        }
    }

    auto pick = [&](Player* picker, int& count) {
        while(true) {
            std::cout << "\n[ " << picker->get_name() << " ] remains to pick " << count << " wonder(s).\n";
            std::cout << "Available wonders in this pool:\n";
            for(size_t i=0; i<current_round.size(); ++i) {
                std::cout << i << ". " << current_round[i].name << "\n";
            }
            std::cout << "Choice: ";
            int choice;
            if(!(std::cin >> choice) || choice < 0 || (size_t)choice >= current_round.size()) {
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                continue;
            }
            picker->add_wonder(current_round[choice]);
            current_round.erase(current_round.begin() + choice);
            count--;
            break;
        }
    };

    // 确定谁先选
    // 规则：第一轮 P1先，第二轮 P2先
    Player* first = (round == 1) ? p1 : p2;
    Player* second = (round == 1) ? p2 : p1;

    // 1-2-1 挑选流程
    int c1 = 1, c2 = 2;
    pick(first, c1);
    pick(second, c2); pick(second, c2);
    
    // 最后一个自动归第一个玩家
    if(!current_round.empty()) {
        std::cout << "\n[ " << first->get_name() << " ] automatically receives: " << current_round[0].name << "\n";
        first->add_wonder(current_round[0]);
    }
    
    std::this_thread::sleep_for(std::chrono::seconds(2));
    clearScreen();
}

void Controller::player_turn(Player& player) {
    bool turn_finished = false;

    while (!turn_finished) {
        // 1. 每一回合开始前（或重试时），显示当前的全局战况
        view->display_board(game);
        view->display_player_status(*game.get_player(0)); 
        view->display_player_status(*game.get_player(1));
        
        view->display_structure(game.get_structure());   // 显示卡牌金字塔

        std::cout << "\n[ " << player.get_name() << "'s Turn ]\n";
        std::cout << "Enter Card ID to select, or -1 to refresh: ";
        
        int card_pos;
        if (!(std::cin >> card_pos)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            clearScreen();
            continue;
        }

        if (card_pos == -1) {
            clearScreen();
            continue;
        }

        // 验证卡牌
        const CardStructure& structure = game.get_structure();
        const Card* selected_card = structure.get_card(card_pos);
        if (!selected_card) {
            view->display_message("Invalid ID: Slot is empty.");
            std::this_thread::sleep_for(std::chrono::seconds(1));
            clearScreen();
            continue;
        }

        // 检查是否被压住
        auto acc = structure.get_accessible();
        if (std::find(acc.begin(), acc.end(), card_pos) == acc.end()) {
            view->display_message("Action Failed: Card is blocked by others!");
            std::this_thread::sleep_for(std::chrono::seconds(1));
            clearScreen();
            continue;
        }

        // 2. 选择动作
        std::cout << "Select Action for card [" << selected_card->name << "]:\n";
        std::cout << "1. Build Building\n";
        std::cout << "2. Discard for Coins\n";
        std::cout << "3. Construct Wonder\n";
        std::cout << "Choice: ";
        
        int action;
        if (!(std::cin >> action)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            clearScreen();
            continue;
        }

        std::string selected_card_name = selected_card->name;
        bool action_success = false;

        switch (action) {
            case 1: // 建造
                if (game.take_card(card_pos, player)) {
                    view->display_message("Successfully built: " + selected_card_name);
                    action_success = true;
                } else {
                    view->display_message("Action Failed: Not enough resources or coins!");
                }
                break;

            case 2: // 弃牌
                game.discard_for_coins(card_pos, player);
                view->display_message("Card discarded. You gained coins.");
                action_success = true;
                break;

            case 3: // 建奇迹
                {
                    const auto& wonders = player.get_wonders();
                    std::cout << "Select a Wonder to build:\n";
                    for (size_t i = 0; i < wonders.size(); ++i) {
                        std::cout << i << ". " << wonders[i].name 
                                  << (wonders[i].is_built ? " (Built)" : " (Available)") << "\n";
                    }
                    std::cout << "Choice (0-3): ";
                    int w_idx;
                    if (!(std::cin >> w_idx) || w_idx < 0 || w_idx >= (int)wonders.size()) {
                        std::cin.clear();
                        std::cin.ignore(1000, '\n');
                        view->display_message("Invalid wonder selection.");
                        break;
                    }

                    if (wonders[w_idx].is_built) {
                        view->display_message("Wonder already built!");
                        break;
                    }

                    if (game.build_wonder(w_idx, card_pos, player)) {
                        view->display_message("Wonder constructed successfully.");
                        action_success = true;
                    } else {
                        view->display_message("Action Failed: Cannot build wonder (Check resources)!");
                    }
                }
                break;

            default:
                view->display_message("Invalid action choice.");
                break;
        }

        if (action_success) {
            turn_finished = true;
            // 回合成功结束，短暂停留让玩家看结果
            std::this_thread::sleep_for(std::chrono::seconds(2));
        } else {
            // 动作失败，停留1秒后重试
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        clearScreen(); 
    }
}

// 供 Game 触发的特殊交互显示
void Controller::show_message(const std::string& msg) {
    view->display_message(msg);
}