#include <cmath>
#include <algorithm>

#include "insect.h"

using namespace std;

Insect::Insect(int id, int botId, Position pos, Face face) :
        id(id), botId(botId), pos(pos), face(face), type(BEE),
        count(1), pollen(0), maxCount(100), maxPollenFactor(3), totalDamage(0) { }

const int Insect::getId() const {
    return id;
}

const int Insect::getBotId() const {
    return botId;
}

const Insect::InsectType Insect::getType() const {
    return type;
}

const int Insect::getCount() const {
    return count;
}

const int Insect::getPollen() const {
    return pollen;
}

const int Insect::getFace() const {
    return face;
}

const Position Insect::getPosition() const {
    return pos;
}

const int Insect::getAttack() const {
    return 0;
}

void Insect::collectPollen(int amount) {
    // Default method is do nothing.
}

void Insect::takeDamage(int damage) {
    totalDamage += damage;
}

const int Insect::getKillAmount() const {
    return count;
}

int Insect::depositPollen() {
    return 0;
}

void Insect::merge(shared_ptr<Insect> insect) {
    count = min(maxCount, count + insect->getCount());
    pollen = min(count * maxPollenFactor, pollen + insect->getPollen());
}

shared_ptr<Insect> Insect::splitOff(int amount, Face f, int nextInsectId, Position pos) {
    int pollenToGive = round(pollen * float(amount) / count);
    count -= amount;
    pollen -= pollenToGive;
    return createInsect(nextInsectId, pos, f, amount, pollenToGive);
}

void Insect::changeFace(Face f) {
    face = f;
}

void Insect::setPosition(Position p) {
    pos = p;
}

void Insect::commitDamage() {
    count -= totalDamage;
    pollen = min(count * maxPollenFactor, pollen);
    totalDamage = 0;
}

