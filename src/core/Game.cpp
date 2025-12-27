#include "Game.h"
#include "Board.h"
#include "../player/Player.h"
#include "../player/CostCalculator.h"
#include "../cards/Card.h"
#include "../cards/CardStructure.h"
#include "../view/Ctrller.h"
#include <iostream>
#include <algorithm>

// 单例实例初始化
Game* Game::instance = nullptr;

Game& Game::getInstance() {
    if (instance == nullptr) {
        instance = new Game();
    }
    return *instance;
}

Game::Game() : board(std::make_unique<Board>()), 
               currentAge(1), 
               currentPlayerIdx(0), 
               isGameOver(false), 
               extraTurnTriggered(false) {}

void Game::init() {
    std::cout << "[Game] Initializing 7 Wonders Duel..." << std::endl;
    
    // 初始化玩家
    players.clear();
    players.push_back(std::make_shared<Player>("Player 1"));
    players.push_back(std::make_shared<Player>("Player 2"));

    // 初始金币：根据规则通常是7元
    for(auto& p : players) {
        p->addCoins(7); 
    }

    // 初始化第一时代卡牌布局 (Member 2 需要提供 createAllCards 逻辑)
    // 这里假设逻辑已经迁移到符合 20 张牌的金字塔结构
    auto age1_deck = createAllCards(); // 实际应按时代筛选
    // 暂时模拟洗牌取20张
    cardStructure = std::make_unique<CardStructure>(1, std::move(age1_deck));
}

// 核心动作 1：购买/建造卡牌
bool Game::takeCard(int pos, Player& player) {
    try {
            // 1. 取牌
        std::unique_ptr<Card> card = cardStructure->takeCard(pos);

        // 2. 检查建造资格 (改用 Member 3 的逻辑)
        bool isFree = player.canBuildFreeByChain(card->chainPrerequisites);
        if (!isFree) {
            // 调用 Member 3 正确的函数名
            auto result = CostCalculator::calculateBuildCost(player, *getOpponent(), card->cost);
            if (!result.canBuild) return false;
            
            player.spendCoins(result.totalCoinCost);
        }

        // 3. 记录卡牌颜色和产出
        player.addBuiltCard(card->name, card->color);
        // 如果是资源卡，要注册产出 (重要！)
        if (card->color == Color::BROWN || card->color == Color::GREY) {
            for (auto res : card->producedResources) {
                player.addResourceProducingCard(res);
            }
        }
        // 5. 切换回合
        if (!extraTurnTriggered) {
            currentPlayerIdx = (currentPlayerIdx + 1) % 2;
        } else {
            std::cout << "Extra turn! " << player.getName() << " moves again." << std::endl;
            extraTurnTriggered = false; // 重置标记
        }
        return true;
    } catch (const std::exception& e) {
        std::cout << "Action Error: " << e.what() << std::endl;
        return false;
    }
}

// 核心动作 2：弃牌换钱
void Game::discardForCoins(int pos, Player& player) {
    try {
        std::unique_ptr<Card> card = cardStructure->takeCard(pos);
        // 规则：2元 + 玩家拥有的每张黄卡额外1元
        int coins = 2 + player.countYellowCards(); 
        player.addCoins(coins);
        
        std::cout << player.getName() << " discarded " << card->name << " and earned " << coins << " coins." << std::endl;
        
        // 弃牌从不触发额外回合
        currentPlayerIdx = (currentPlayerIdx + 1) % 2;
        extraTurnTriggered = false; 
    } catch (...) {}
}

// 核心动作 3：建造奇迹
void Game::buildWonder(int wonderIdx, int pos, Player& player) {
    // TODO: 实现奇迹建造逻辑
    // 1. 检查玩家是否还有奇迹位
    // 2. 计算奇迹成本并支付
    // 3. 从 cardStructure 中移除 pos 位置卡牌（作为奇迹地基）
    // 4. 触发奇迹效果
    // 5. 换人逻辑 (注意：有些奇迹会设置 extraTurnTriggered = true)
}

void Game::run() {
    init();
    Controller controller(*this); 

    while (!isGameOver && currentAge <= 3) {
        while (!cardStructure->isEmpty() && !isGameOver) { 
            playTurn(controller);
            if (checkSupremacyVictory()) {
                isGameOver = true;
                break;
            }
        }

        if (!isGameOver) {
            endAge();
        }
    }
    // TODO: 游戏结束最后的得分计算与结果展示
}

void Game::playTurn(Controller& controller) { 
    Player* curr = getCurrentPlayer();
    std::cout << "\n--- " << curr->getName() << "'s Turn ---" << std::endl;
    
    // 委托给 Controller 处理 UI 和输入
    controller.player_turn(*curr); 
}

bool Game::checkSupremacyVictory() {
    // 1. 军事压制：棋子到达 0 或 18
    if (board->getPawnPosition() <= 0 || board->getPawnPosition() >= 18) {
        std::cout << "Military Supremacy Victory!" << std::endl;
        return true;
    }
    
    // 2. 科技压制：收集 6 种不同的科技符号
    if (getCurrentPlayer()->getUniqueScienceCount() >= 6) {
        std::cout << "Scientific Supremacy Victory!" << std::endl;
        return true;
    }
    
    return false;
}

void Game::endAge() {
    std::cout << "\n--- Age " << currentAge << " has ended ---" << std::endl;
    currentAge++;
    
    if (currentAge <= 3) {
        // TODO: 逻辑：军事落后方选择谁开始下一时代
        // 此处暂时简化为默认切换
        // cardStructure = CardStructureFactory::createAge(currentAge);
    }
}

Player* Game::getCurrentPlayer() {
    return players[currentPlayerIdx].get();
}

Player* Game::getOpponent() {
    return players[(currentPlayerIdx + 1) % 2].get();
}