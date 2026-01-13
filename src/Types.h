#pragma once

enum class Color { NONE, BROWN, GREY, BLUE, YELLOW, RED, GREEN, PURPLE };

enum class Resource { 
    NONE, WOOD, CLAY, STONE, GLASS, PAPYRUS, COIN, VP,
    ARMILLARY,  // 天文仪 (看起来像地球仪)
    SUNDIAL,    // 日象仪 (有刻度的圆盘)
    WHEEL,    // 齿轮
    MORTAR,   // 研钵
    QUILL,    // 羽毛笔
    ARCH,     // 拱门
    LAW       // 法律 (仅通过进步板块)
};


enum class LinkSymbol {
    NONE,
    // --- 军事类 ---
    SWORD,      // 剑 (Garrison -> Barracks)
    TOWER,      // 塔 (Guard Tower -> Training Ground -> Circus)
    HORSESHOE,  // 马蹄铁 (Stable -> Horse Breeders) - 重点检查这一行！
    TARGET,     // 靶子 (Workshop -> Archery Range -> Siege Workshop)
    HELMET,     // 头盔 (Barracks -> Parade Ground -> Circus)

    // --- 科技类 ---
    BOOK,       // 书 (Scriptorium -> Library -> Senate)
    GEAR,       // 齿轮 (Workshop -> Laboratory -> Observatory)
    LAMP,       // 灯 (Apothecary -> Dispensary -> University)
    HARP,       // 竖琴

    // --- 市政类 ---
    MASK,       // 面具 (Theater -> Statue -> Gardens)
    MOON,       // 月亮 (Altar -> Temple -> Pantheon)
    SUN,        // 太阳 (Temple -> Palace)
    DROP,       // 水滴 (Baths -> Aqueduct)
    COLUMN,     // 柱子 (Courthouse -> 奇迹)
    TEMPLE,     // 罗马殿堂/神庙
    
    // --- 商业类 ---
    BARREL,     // 木桶 (Forum -> Haven)
    VASE,     // 花瓶 (Caravansery -> Lighthouse)
    
};

enum class PlayerType { HUMAN, AI_RANDOM };
enum class ProgressToken { AGRICULTURE, ARCHITECTURE, ECONOMY, LAW, MASONRY, MATHEMATICS, PHILOSOPHY, STRATEGY, THEOLOGY, URBANISM };

