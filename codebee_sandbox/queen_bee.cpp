#include <cmath>

#include "queen_bee.h"

using namespace std;

QueenBee::QueenBee(int id, int botId, Position pos, Face face, int c, int p)
    : Insect(id, botId, pos, face) {
    type = QUEEN;
    maxPollenFactor = 9001;
    count = c;
    pollen = p;
}

const int QueenBee::getAttack() const {
    return 9001;
}

void QueenBee::takeDamage(int damage) {
    pollen -= damage * 10;
}

const int QueenBee::getKillAmount() const {
    return 0;
}

// Should never be called.
std::shared_ptr<Insect> QueenBee::createInsect(int nextInsectId, Position pos, Face f, int c, int p) {
    return make_shared<QueenBee>(nextInsectId, botId, pos, f);
}

int QueenBee::spawnBees() {
    if (pollen >= 5) {
        int bees = floor(pollen / 5);
        pollen %= 5;
        return bees;
    }
    return 0;
}

void QueenBee::takePollen(int amount) {
    pollen += amount;
}

