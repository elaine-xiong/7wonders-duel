#include "../view/Ctrller.h"
#include "ConsoleView.h"
#include "../core/Game.h"
#include "../player/Player.h"
#include "../cards/CardStructure.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <thread> 
#include <chrono>

Controller::Controller(Game& game) : game(game), view(std::make_unique<ConsoleView>()) {}
Controller::~Controller() = default;
void Controller::player_turn(Player& player) {
    bool turn_finished = false;

    
    // 1. 每一回合开始前，显示当前的全局战况
    view->display_board(game);
    view->display_player_status(*game.get_opponent(player)); // 显示对手状态
    view->display_player_status(player);                   // 显示自己状态
    view->display_structure(game.get_structure());         // 显示卡牌金字塔

    while (!turn_finished) {
        std::cout << "\n[ " << player.get_name() << "'s Turn ]\n";
        std::cout << "Enter Card ID to select, or -1 to see options: ";
        
        int card_pos;
        if (!(std::cin >> card_pos)) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }

        if (card_pos == -1) {
            // 重新显示布局
            view->display_structure(game.get_structure());
            continue;
        }

        // 验证卡牌是否可取
        // const Card* selected_card = game.get_structure().get_card(card_pos);
        const CardStructure& structure = game.get_structure();
        const Card* selected_card = structure.get_card(card_pos);
        if (!selected_card) {
            view->display_message("Invalid ID: Slot is empty.");
            continue;
        }

        // 检查是否被压住
        auto acc = game.get_structure().get_accessible();
        if (std::find(acc.begin(), acc.end(), card_pos) == acc.end()) {
            view->display_message("Action Failed: Card is blocked by others!");
            continue;
        }

        // 2. 选择动作
        std::cout << "Select Action for card [" << selected_card->name << "]:\n";
        std::cout << "1. Build Building\n";
        std::cout << "2. Discard for Coins\n";
        std::cout << "3. Construct Wonder\n";
        std::cout << "Choice: ";
        
        int action;
        std::cin >> action;

        std :: string selected_card_name = selected_card->name;

        switch (action) {
            case 1: // 建造
                if (game.take_card(card_pos, player)) {
                    view->display_message("Successfully built: " + selected_card_name);
                    turn_finished = true;
                } else {
                    view->display_message("Action Failed: Not enough resources or coins!");
                }
                break;

            case 2: // 弃牌
                game.discard_for_coins(card_pos, player);
                view->display_message("Card discarded. You gained coins.");
                turn_finished = true;
                break;

            case 3: // 建奇迹
                // 暂时简化逻辑，wonderIdx 这里假设为 0，实际应从玩家拥有的奇迹中选
                game.build_wonder(0, card_pos, player); 
                view->display_message("Wonder construction attempted.");
                turn_finished = true;
                break;

            default:
                view->display_message("Invalid choice. Try again.");
                break;
        }
    // 等待5s时间玩家查看结果
    view->display_message("Wait 5 seconds to view results...");
    std::this_thread::sleep_for(std::chrono::seconds(5));  // Sleep for 3 seconds
    clearScreen(); // 清屏以准备下一回合
    }
}

// 供 Game 触发的特殊交互显示
void Controller::show_message(const std::string& msg) {
    view->display_message(msg);
}