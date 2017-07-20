#include <algorithm>

#include "bee.h"

using namespace std;

Bee::Bee(int id, int botId, Position pos, Face face, int c, int p)
    : Insect(id, botId, pos, face) {
    type = BEE;
    count = c;
    pollen = p;
}

const int Bee::getAttack() const {
    return count;
}

void Bee::collectPollen(int amount) {
    pollen = min(count * 3, pollen + count * amount);
}

int Bee::depositPollen() {
    int p = pollen;
    pollen = 0;
    return p;
}

std::shared_ptr<Insect> Bee::createInsect(int nextInsectId, Position pos, Face f, int c, int p) {
    return make_shared<Bee>(nextInsectId, botId, pos, f, c, p);
}