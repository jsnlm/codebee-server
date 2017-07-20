#ifndef CODEBEE_GAME_QUEEN_BEE_H
#define CODEBEE_GAME_QUEEN_BEE_H

#include "face.h"
#include "insect.h"
#include "position.h"

class QueenBee : public Insect {
public:
    QueenBee(int id, int botId, Position pos, Face face, int c = 1, int p = 0);

    const int getAttack() const override;
    void takeDamage(int damage) override;
    const int getKillAmount() const override;
    std::shared_ptr<Insect> createInsect(int nextInsectId, Position pos, Face f, int c, int p) override;

    int spawnBees();
    void takePollen(int amount);
};

#endif //CODEBEE_GAME_QUEEN_BEE_H
