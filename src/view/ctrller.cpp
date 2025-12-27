#include "ctrller.h"
#include "ConsoleView.h"  // 包含 ConsoleView 的定义

Controller::Controller(Game& g) : game(g) {}

// 玩家回合处理
void Controller::player_turn(Player& player) {
    // 展示卡牌结构和可获取卡牌
    auto& structure = game.get_structure();
    view.display_structure(structure, structure.get_accessible());
    // 展示玩家资源
    std::cout << "你的资源：" << player.print_resources() << "\n";
    // 提示操作指令
    std::cout << "请选择操作（take <位置> | wonder <奇迹编号> <卡牌位置> | discard <位置>）：";
    std::string input;
    std::getline(std::cin, input);
    // 解析输入（示例："take 3" 表示选取3号位置卡牌）
    std::stringstream ss(input);
    std::string cmd;
    ss >> cmd;
    if (cmd == "take") {
        int pos;
        ss >> pos;
        game.take_card(pos, player);  // 调用Game类方法选取卡牌并应用效果
    } else if (cmd == "wonder") {
        int wonder_num, pos;
        ss >> wonder_num >> pos;
        // 校验奇迹是否可建造，将指定位置卡牌置于奇迹之下
        game.build_wonder(wonder_num - 1, pos, player);
    } else if (cmd == "discard") {
        int pos;
        ss >> pos;
        game.discard_for_coins(pos, player);  // 弃牌获得金币
    } else {
        std::cout << "无效指令，请重新输入！\n";
        player_turn(player);  // 重新提示输入
    }
}