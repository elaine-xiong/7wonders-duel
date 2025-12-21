#include "Player.h"

Player::Player() : coins(7) {}

void Player::addResource(ResourceType resource, int amount) {
    resources[resource] += amount;
}

int Player::getResource(ResourceType resource) const {
    auto it = resources.find(resource);
    return it != resources.end() ? it->second : 0;
}

int Player::getCoins() const {
    return coins;
}

bool Player::spendCoins(int amount) {
    if (coins < amount) return false;
    coins -= amount;
    return true;
}

void Player::addCoins(int amount) {
    coins += amount;
}