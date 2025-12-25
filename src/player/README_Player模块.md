# Player 模块使用说明

## 成员3职责：玩家逻辑与经济系统

本模块实现了 7 Wonders Duel 游戏中的玩家逻辑和经济系统，包括：
- 资源管理
- 金币管理
- 交易系统
- 连锁建造
- 军事和科技系统


## 核心功能说明

### 1. 玩家初始化
```cpp
// 创建人类玩家
Player player1("Alice", PlayerType::HUMAN);

// 创建AI玩家
Player player2("Bot", PlayerType::AI_RANDOM);
```

### 2. 资源管理
```cpp
// 添加资源（建造褐色/灰色卡时调用）
player.addResource(Resource::WOOD, 1);
player.addResourceProducingCard(Resource::WOOD);  // 用于交易成本计算

// 检查资源
int woodCount = player.getResource(Resource::WOOD);
bool hasEnough = player.hasEnoughResource(Resource::WOOD, 2);
```

### 3. 金币管理
```cpp
// 获取金币
int coins = player.getCoins();

// 增加金币
player.addCoins(3);

// 消耗金币
if (player.spendCoins(5)) {
    // 成功消耗
}
```

### 4. 建造卡牌

#### 4.1 检查连锁建造
```cpp
std::vector<std::string> chainPrereqs = {"Lumber Yard", "Quarry"};
if (player.canBuildFreeByChain(chainPrereqs)) {
    // 可以免费建造
    player.addBuiltCard("Sawmill", Color::BROWN);
}
```

#### 4.2 普通建造（使用 CostCalculator）
```cpp
std::map<Resource, int> cardCost = {
    {Resource::WOOD, 2},
    {Resource::COIN, 1}
};

// 方法1：检查是否能建造（考虑交易）
if (CostCalculator::canAffordWithTrade(player, opponent, cardCost)) {
    // 方法2：执行建造（自动计算交易成本并扣除资源/金币）
    if (CostCalculator::executeBuild(player, opponent, cardCost)) {
        player.addBuiltCard("Forum", Color::YELLOW);
        std::cout << "建造成功！" << std::endl;
    }
}
```

#### 4.3 详细成本计算
```cpp
auto result = CostCalculator::calculateBuildCost(player, opponent, cardCost);

if (result.canBuild) {
    std::cout << "总金币成本: " << result.totalCoinCost << std::endl;
    
    for (const auto& [resource, amount] : result.resourcesToBuy) {
        std::cout << "需要购买: " << amount << " 个资源" << std::endl;
    }
}
```

### 5. 交易系统

#### 交易成本计算规则
购买资源费用 = **2 + 对手拥有该类褐色/灰色卡数量**

```cpp
// 示例：玩家1想从玩家2购买木头
// 玩家2有 3 张木头产出卡
opponent.addResourceProducingCard(Resource::WOOD);
opponent.addResourceProducingCard(Resource::WOOD);
opponent.addResourceProducingCard(Resource::WOOD);

// 计算交易成本
int cost = CostCalculator::calculateTradeCost(player, opponent, Resource::WOOD);
// cost = 2 + 3 = 5 金币
```

### 6. 军事系统
```cpp
// 添加军事标记（己方前进）
player.addMilitaryTokens(2);

// 检查军事胜利
if (player.hasMilitaryVictory()) {
    std::cout << player.getName() << " 军事获胜！" << std::endl;
}
```

### 7. 科技系统
```cpp
// 添加科技符号
player.addScienceSymbol(Resource::SCIENCE_COMPASS);
player.addScienceSymbol(Resource::SCIENCE_GEAR);

// 检查科技胜利
if (player.hasScienceVictory()) {
    std::cout << player.getName() << " 科技获胜！" << std::endl;
}
```

### 8. 胜利点数
```cpp
// 添加胜利点数（蓝卡、紫卡等）
player.addVictoryPoints(5);

// 计算最终得分（包括金币）
int finalScore = player.calculateFinalScore();
// finalScore = victoryPoints + coins/3
```

## 完整建造流程示例

```cpp
void buildCardExample(Player& player, Player& opponent, const Card& card) {
    // 1. 检查连锁建造
    if (player.canBuildFreeByChain(card.chain_prerequisites)) {
        std::cout << "连锁建造，免费！" << std::endl;
        player.addBuiltCard(card.name, card.color);
        
        // 注册资源产出卡（如果是褐色/灰色卡）
        if (card.color == Color::BROWN || card.color == Color::GREY) {
            // 假设卡牌产出木头
            player.addResourceProducingCard(Resource::WOOD);
        }
        
        return;
    }
    
    // 2. 检查是否能建造（考虑交易）
    if (!CostCalculator::canAffordWithTrade(player, opponent, card.cost)) {
        std::cout << "资源/金币不足，无法建造！" << std::endl;
        return;
    }
    
    // 3. 执行建造
    if (CostCalculator::executeBuild(player, opponent, card.cost)) {
        std::cout << "建造成功：" << card.name << std::endl;
        
        // 4. 添加已建造卡牌
        player.addBuiltCard(card.name, card.color);
        
        // 5. 注册资源产出卡
        if (card.color == Color::BROWN || card.color == Color::GREY) {
            // 根据卡牌效果添加资源
            player.addResourceProducingCard(Resource::WOOD);
        }
        
        // 6. 应用卡牌效果
        if (card.effect) {
            card.effect(player, game);
        }
    }
}
```

## 与其他模块的接口

### 与 Card 模块交互
```cpp
// Card 模块提供卡牌定义
Card card;
card.name = "Lumber Yard";
card.color = Color::BROWN;
card.cost = {}; // 免费
card.chain_provides = "WOOD_SYMBOL";

// Player 模块使用卡牌信息建造
player.addBuiltCard(card.name, card.color);
```

### 与 Game 模块交互
```cpp
// Game 模块管理两名玩家和回合流程
class Game {
    Player player1;
    Player player2;
    
    void playerTurn(Player& currentPlayer, Player& opponent) {
        // 玩家选择操作
        // 使用 CostCalculator 计算成本
        // 调用 Player 的方法执行操作
    }
};
```

### 与 View/Controller 模块交互
```cpp
// View 模块显示玩家状态
void displayPlayerStatus(const Player& player) {
    std::cout << "玩家: " << player.getName() << std::endl;
    std::cout << "金币: " << player.getCoins() << std::endl;
    std::cout << "胜利点数: " << player.getVictoryPoints() << std::endl;
    std::cout << "已建造卡牌数: " << player.getBuiltCards().size() << std::endl;
}

// Controller 模块处理用户输入
void handleBuildAction(Player& player, Player& opponent, const Card& card) {
    if (CostCalculator::canAffordWithTrade(player, opponent, card.cost)) {
        CostCalculator::executeBuild(player, opponent, card.cost);
        player.addBuiltCard(card.name, card.color);
    } else {
        std::cout << "无法建造此卡牌！" << std::endl;
    }
}
```

## 关键规则实现

### ✅ 已实现的规则
1. **初始金币7个** - `Player()` 构造函数
2. **交易成本计算** - `CostCalculator::calculateTradeCost()`
3. **连锁建造** - `Player::canBuildFreeByChain()`
4. **资源管理** - `addResource()`, `spendResource()`, `getResource()`
5. **军事胜利** - `hasMilitaryVictory()`
6. **科技胜利** - `hasScienceVictory()`
7. **最终计分** - `calculateFinalScore()` (VP + 金币/3)

### 📝 需要其他模块配合的规则
- 卡牌金字塔结构（成员2负责）
- 游戏主循环和回合管理（成员1负责）
- 用户界面和输入处理（成员4负责）

## 测试建议

### 单元测试示例
```cpp
void testTradeCost() {
    Player buyer("Alice");
    Player seller("Bob");
    
    // Bob 有 3 张木头卡
    seller.addResourceProducingCard(Resource::WOOD);
    seller.addResourceProducingCard(Resource::WOOD);
    seller.addResourceProducingCard(Resource::WOOD);
    
    // 计算交易成本
    int cost = CostCalculator::calculateTradeCost(buyer, seller, Resource::WOOD);
    
    assert(cost == 5);  // 2 + 3 = 5
    std::cout << "交易成本测试通过！" << std::endl;
}

void testChainBuilding() {
    Player player("Alice");
    
    // 建造前置建筑
    player.addBuiltCard("Lumber Yard", Color::BROWN);
    
    // 检查连锁建造
    std::vector<std::string> prereqs = {"Lumber Yard", "Quarry"};
    assert(player.canBuildFreeByChain(prereqs) == true);
    
    std::cout << "连锁建造测试通过！" << std::endl;
}
```

## 常见问题

### Q: 为什么有 Player_new.h 和 Player.h 两个文件？
A: `Player_new.h` 是完整重写的版本，包含所有必需功能。你需要将其重命名为 `Player.h` 替换旧文件。

### Q: ResourceType 和 Resource 有什么区别？
A: 旧代码使用 `ResourceType`，新代码统一使用 `Resource`（与 Card.h 一致）。请使用新代码。

### Q: 如何处理黄卡的特殊资源产出？
A: 在建造黄卡时，根据卡牌效果调用 `addResource()` 或 `addResourceProducingCard()`。

### Q: 科技胜利的"3对相同符号"如何实现？
A: 当前简化为收集6个不同符号。完整实现需要维护每种符号的数量，可以在 Player 类中添加 `std::map<Resource, int> scienceSymbolCounts`。

## 后续优化建议

1. **科技符号详细统计**：维护每种科技符号的数量，支持"3对相同符号"胜利条件
2. **奇迹建造**：添加 Wonder 相关的资源管理
3. **特殊卡牌效果**：扩展资源产出逻辑，支持黄卡的多种资源选择
4. **日志系统**：添加操作日志，方便调试和显示
5. **存档/读档**：支持游戏状态序列化

## 联系与协作

本模块（成员3）需要与以下模块协作：
- **成员1**：提供 Game 类接口，调用 Player 和 CostCalculator
- **成员2**：提供 Card 类定义，确保 Resource 枚举一致
- **成员4**：提供 View 和 Controller，展示玩家状态和处理输入

如有问题，请及时沟通！
