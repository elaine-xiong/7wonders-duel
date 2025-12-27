#include "ConsoleView.h"

// 展示卡牌金字塔结构
void ConsoleView::display_structure(const CardStructure& structure, const std::vector<int>& accessible) {
    std::cout << "第" << structure.get_age() << "时代 卡牌结构：\n";
    // 按时代硬编码行布局（以第一时代为例）
    // 顶层（第5行）：居中显示
    std::cout << std::string(24, ' ') << "[" << format_card(19, structure, accessible) << "]\n";
    // 第4行：间隔显示
    std::cout << std::string(12, ' ') << "[" << format_card(17, structure, accessible) << "]"
              << std::string(12, ' ') << "[" << format_card(18, structure, accessible) << "]\n";
    // ... 依次实现下层行的布局，通过空格/制表符调整对齐
    // 显示可获取的卡牌
    std::cout << "可选取卡牌：";
    for (int pos : accessible) {
        std::cout << pos << "（" << structure.get_card(pos)->name << "）";
    }
    std::cout << std::endl;
}

// 格式化卡牌显示（背面朝上显示"???"，可获取卡牌前缀加"*"）
std::string ConsoleView::format_card(int pos, const CardStructure& s, const std::vector<int>& acc) {
    const Card* card = s.get_card(pos);
    if (!card) return "空";
    if (!card->is_face_up) return "???";
    return (std::find(acc.begin(), acc.end(), pos) != acc.end() ? "*" : "") + card->name;
}