#ifndef PLAYER_H
#define PLAYER_H

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <set>

// 前向声明
class Card;

// 直接使用 Card.h 中的 Resource 和 Color 枚举
enum class Resource { 
    WOOD, BRICK, ORE, STONE,           // 褐色资源
    GLASS, CLOTH, PAPYRUS,             // 灰色资源
    SHIELD,                             // 军事
    SCIENCE_COMPASS, SCIENCE_GEAR, SCIENCE_TABLET,  // 科技符号
    COIN, VP                            // 金币和胜利点
};

enum class Color { BROWN, GREY, BLUE, YELLOW, RED, GREEN, PURPLE };

// 玩家类型枚举
enum class PlayerType {
    HUMAN,      // 人类玩家
    AI_RANDOM   // 随机AI（简单AI实现）
};

/**
 * Player 类：管理玩家的资源、金币、已建造卡牌等状态
 * 职责：玩家逻辑与经济系统
 * 
 * 核心功能：
 * 1. 资源管理：木头、砖块、矿石、石头、玻璃、布料、纸莎草
 * 2. 金币管理：初始7个金币，用于购买资源和卡牌
 * 3. 交易系统：购买资源费用 = 2 + 对手拥有该类褐色/灰色卡数量
 * 4. 连锁建造：拥有前置建筑可以免费建造某些卡牌
 * 5. 军事和科技：军事标记和科技符号收集
 */
class Player {
private:
    std::string name;                                   // 玩家姓名
    PlayerType type;                                    // 玩家类型（人类/AI）
    std::map<Resource, int> resources;                  // 玩家拥有的基础资源数量
    int coins;                                          // 玩家金币数量
    std::vector<std::string> builtCards;                // 已建造的卡牌名称列表
    std::map<Resource, int> resourceProducingCards;     // 按资源类型统计的产出卡数量（用于交易成本）
    std::map<Color, int> cardsByColor;                  // 按颜色统计的卡牌数量
    int militaryTokens;                                 // 军事标记（正数己方领先，负数对手领先）
    int victoryPoints;                                  // 胜利点数
    std::set<Resource> scienceSymbols;                  // 收集的科技符号

public:
    /**
     * 构造函数：初始化玩家
     * @param playerName 玩家姓名
     * @param playerType 玩家类型（人类/AI）
     */
    Player(const std::string& playerName = "Player", PlayerType playerType = PlayerType::HUMAN);

    // 玩家信息
    
    std::string getName() const { return name; }
    void setName(const std::string& playerName) { name = playerName; }
    
    PlayerType getType() const { return type; }
    void setType(PlayerType playerType) { type = playerType; }
    
    bool isHuman() const { return type == PlayerType::HUMAN; }
    bool isAI() const { return type == PlayerType::AI_RANDOM; }

    //资源管
    
    /**
     * 添加资源：给玩家增加指定数量的资源
     * @param resource 资源类型
     * @param amount 数量（正数）
     */
    void addResource(Resource resource, int amount);

    /**
     * 获取资源：返回玩家拥有的指定资源数量
     * @param resource 资源类型
     * @return 资源数量
     */
    int getResource(Resource resource) const;

    /**
     * 消耗资源：扣除指定数量的资源
     * @param resource 资源类型
     * @param amount 数量
     * @return 是否成功（资源不足则返回 false）
     */
    bool spendResource(Resource resource, int amount);

    /**
     * 检查是否拥有足够资源
     * @param resource 资源类型
     * @param amount 需要的数量
     * @return 是否足够
     */
    bool hasEnoughResource(Resource resource, int amount) const;

    //金币管理
    
    /**
     * 获取金币数量
     * @return 当前金币
     */
    int getCoins() const;

    /**
     * 消耗金币：扣除指定数量的金币
     * @param amount 金币数量
     * @return 是否成功（金币不足则返回 false）
     */
    bool spendCoins(int amount);

    /**
     * 增加金币
     * @param amount 金币数量（正数）
     */
    void addCoins(int amount);

    //卡牌管理
    
    /**
     * 添加已建造的卡牌：玩家建造卡牌后调用
     * @param cardName 卡牌名称
     * @param cardColor 卡牌颜色
     */
    void addBuiltCard(const std::string& cardName, Color cardColor);

    /**
     * 检查是否拥有指定建筑：用于连锁建造判断
     * @param cardName 卡牌名称
     * @return 是否拥有该卡牌
     */
    bool hasBuilding(const std::string& cardName) const;

    /**
     * 获取已建造的卡牌列表
     * @return 卡牌名称列表
     */
    const std::vector<std::string>& getBuiltCards() const;

    /**
     * 获取某颜色卡牌的数量
     * @param color 卡牌颜色
     * @return 卡牌数量
     */
    int getCardCountByColor(Color color) const;

    //资源产出卡统计（用于交易成本计算
    
    /**
     * 注册资源产出卡：当玩家建造褐色/灰色卡时调用
     * @param resource 资源类型
     */
    void addResourceProducingCard(Resource resource);

    /**
     * 获取某类资源的产出卡数量：用于对手计算交易成本
     * @param resource 资源类型
     * @return 产出卡数量
     */
    int getResourceProducingCardCount(Resource resource) const;

    //建造逻辑
    
    /**
     * 检查是否能建造卡牌（不考虑交易）
     * @param cost 卡牌成本
     * @return 是否能建造
     */
    bool canAffordCard(const std::map<Resource, int>& cost) const;

    /**
     * 检查通过连锁建造是否免费
     * @param chainPrerequisites 前置建筑列表
     * @return 是否满足连锁条件
     */
    bool canBuildFreeByChain(const std::vector<std::string>& chainPrerequisites) const;

    /**
     * 建造卡牌：扣除资源和金币
     * @param cost 卡牌成本
     * @return 是否成功
     */
    bool buildCard(const std::map<Resource, int>& cost);

    //军事 
    
    /**
     * 添加军事标记
     * @param amount 数量（正数表示己方前进，负数表示对手前进）
     */
    void addMilitaryTokens(int amount);

    /**
     * 获取军事标记数量
     * @return 军事标记
     */
    int getMilitaryTokens() const;

    /**
     * 检查是否军事获胜（军事标记达到阈值）
     * @return 是否军事胜利
     */
    bool hasMilitaryVictory() const;

    //科技 
    
    /**
     * 添加科技符号
     * @param symbol 科技符号类型
     */
    void addScienceSymbol(Resource symbol);

    /**
     * 获取收集的科技符号集合
     * @return 科技符号集合
     */
    const std::set<Resource>& getScienceSymbols() const;

    /**
     * 检查是否科技获胜（收集6个不同科技符号或3对相同科技符号）
     * @return 是否科技胜利
     */
    bool hasScienceVictory() const;

    //  胜利点数 
    
    /**
     * 添加胜利点数
     * @param amount 点数
     */
    void addVictoryPoints(int amount);

    /**
     * 获取胜利点数
     * @return 胜利点数
     */
    int getVictoryPoints() const;

    /**
     * 计算最终得分（包括金币）
     * @return 最终得分
     */
    int calculateFinalScore() const;


    
    // --- 新增：获取唯一科技符号数量 ---
    int getUniqueScienceCount() const { 
        return scienceSymbols.size(); 
    }

    // --- 新增：统计黄色卡牌数量（用于弃牌金币计算） ---
    int countYellowCards() const {
        return getCardCountByColor(Color::YELLOW);
    }
    
    // --- 新增：强制扣钱（用于奇迹效果或军事惩罚） ---
    void loseCoins(int amount) {
        coins = (coins > amount) ? (coins - amount) : 0;
    }

    // --- 新增：处理二选一资源（如：林场产出的 木头/石头） ---
    // 这通常需要一个专门的结构来存储，或者简化为注册两种产出
    void addWildcardResource(const std::vector<Resource>& options) {
        // 简单实现：将可选资源都加入产出列表，或者记录到专门的 wildcard 集合中
        for(auto r : options) addResourceProducingCard(r);
    }
};

#endif // PLAYER_H
