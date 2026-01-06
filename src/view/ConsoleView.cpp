#include "ConsoleView.h"
#include "../cards/CardStructure.h"
#include "../cards/Card.h"
#include "../player/Player.h"
#include "../core/Game.h"
#include "../core/Board.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

void ConsoleView::display_structure(const CardStructure& structure) {
    int age = structure.get_age();
    std::cout << "\n================ AGE " << age << " STRUCTURE ================\n";

    if (age == 1) {
        // Age I: 正金字塔 (底部 6 -> 顶部 2)
        std::cout << std::setw(20) << "" << format_card_slot(18, structure) << " " << format_card_slot(19, structure) << "\n";
        std::cout << std::setw(17) << "" << format_card_slot(15, structure) << " " << format_card_slot(16, structure) << " " << format_card_slot(17, structure) << "\n";
        std::cout << std::setw(14) << "" << format_card_slot(11, structure) << " " << format_card_slot(12, structure) << " " << format_card_slot(13, structure) << " " << format_card_slot(14, structure) << "\n";
        std::cout << std::setw(11) << "" << format_card_slot(6, structure) << " " << format_card_slot(7, structure) << " " << format_card_slot(8, structure) << " " << format_card_slot(9, structure) << " " << format_card_slot(10, structure) << "\n";
        std::cout << std::setw(8) << "" << format_card_slot(0, structure) << " " << format_card_slot(1, structure) << " " << format_card_slot(2, structure) << " " << format_card_slot(3, structure) << " " << format_card_slot(4, structure) << " " << format_card_slot(5, structure) << "\n";
    } 
    else if (age == 2) {
        // Age II: 倒金字塔 (顶部 2 -> 底部 6)
        std::cout << std::setw(8) << "" << format_card_slot(14, structure) << " " << format_card_slot(15, structure) << " " << format_card_slot(16, structure) << " " << format_card_slot(17, structure) << " " << format_card_slot(18, structure) << " " << format_card_slot(19, structure) << "\n";
        std::cout << std::setw(11) << "" << format_card_slot(9, structure) << " " << format_card_slot(10, structure) << " " << format_card_slot(11, structure) << " " << format_card_slot(12, structure) << " " << format_card_slot(13, structure) << "\n";
        std::cout << std::setw(14) << "" << format_card_slot(5, structure) << " " << format_card_slot(6, structure) << " " << format_card_slot(7, structure) << " " << format_card_slot(8, structure) << "\n";
        std::cout << std::setw(17) << "" << format_card_slot(2, structure) << " " << format_card_slot(3, structure) << " " << format_card_slot(4, structure) << "\n";
        std::cout << std::setw(20) << "" << format_card_slot(0, structure) << " " << format_card_slot(1, structure) << "\n";
    }
    else if (age == 3) {
        // Age III: 括号型布局 (对齐 CardStructure.cpp 的 2-3-4-2-4-3-2 逻辑)
        // 从最顶层 18,19 往下画，直到最底层的初始可选牌 0,1
        std::cout << std::setw(20) << "" << format_card_slot(18, structure) << " " << format_card_slot(19, structure) << " (TOP)\n";
        std::cout << std::setw(17) << "" << format_card_slot(15, structure) << " " << format_card_slot(16, structure) << " " << format_card_slot(17, structure) << "\n";
        std::cout << std::setw(14) << "" << format_card_slot(11, structure) << " " << format_card_slot(12, structure) << " " << format_card_slot(13, structure) << " " << format_card_slot(14, structure) << "\n";
        std::cout << std::setw(20) << "" << format_card_slot(9, structure) << " " << format_card_slot(10, structure) << " (MID)\n";
        std::cout << std::setw(14) << "" << format_card_slot(5, structure) << " " << format_card_slot(6, structure) << " " << format_card_slot(7, structure) << " " << format_card_slot(8, structure) << "\n";
        std::cout << std::setw(17) << "" << format_card_slot(2, structure) << " " << format_card_slot(3, structure) << " " << format_card_slot(4, structure) << "\n";
        std::cout << std::setw(20) << "" << format_card_slot(0, structure) << " " << format_card_slot(1, structure) << " (START)\n";
    }

    // 打印底部 ID 提示
    std::cout << "\nAccessible Card IDs: ";
    auto acc = structure.get_accessible();
    for (int id : acc) {
        const Card* c = structure.get_card(id);
        if(c) std::cout << id << ":" << c->name << "  ";
    }
    std::cout << "\n======================================================\n";
}

void ConsoleView::display_player_status(const Player& player) {
    std::cout << "\n[ PLAYER: " << player.get_name() << " ]\n";
    std::cout << "Coins: " << player.get_coins() << "g | VP: " << player.get_victory_points() << "\n";
    std::cout << "Production: W:" << player.get_resource(Resource::WOOD) 
              << " C:" << player.get_resource(Resource::CLAY) 
              << " S:" << player.get_resource(Resource::STONE) 
              << " G:" << player.get_resource(Resource::GLASS) 
              << " P:" << player.get_resource(Resource::PAPYRUS) << "\n";
}

void ConsoleView::display_board(const Game& game) {
    // 之前可能调用了 getBoard()，现在统一为 get_board()
    int pos = const_cast<Game&>(game).get_board()->get_pawn_position();
    std::cout << "\nMILITARY: [P1] ";
    for(int i=0; i<=18; ++i) {
        if(i == pos) std::cout << "X"; // 棋子
        else if(i == 6 || i == 3 || i == 12 || i == 15) std::cout << "!"; // 惩罚位
        else if(i == 9) std::cout << "|"; // 中心
        else std::cout << ".";
    }
    std::cout << " [P2] (Pos:" << pos << ")\n";
}

void ConsoleView::display_message(const std::string& message) {
    std::cout << ">> MESSAGE: " << message << "\n";
}

std::string ConsoleView::format_card_slot(int pos, const CardStructure& s) {
    const Card* card = s.get_card(pos);
    if (!card) return "[   ]"; 

    auto acc = s.get_accessible();
    bool is_acc = (std::find(acc.begin(), acc.end(), pos) != acc.end());

    if (!card->is_face_up) return "[ ? ]";

    // 截取名称前三位，如果是可选牌则用星号包围
    std::string name = card->name.substr(0, 3);
    return is_acc ? "*" + name + "*" : "[" + name + "]";
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}