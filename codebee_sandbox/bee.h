#ifndef CODEBEE_GAME_BEE_H
#define CODEBEE_GAME_BEE_H

#include "face.h"
#include "insect.h"
#include "position.h"

class Bee : public Insect {
public:
    Bee(int id, int botId, Position pos, Face face, int c = 1, int p = 0);

    const int getAttack() const override;
    void collectPollen(int amount) override;
    int depositPollen() override;
    std::shared_ptr<Insect> createInsect(int nextInsectId, Position pos, Face f, int c, int p) override;
};

#endif //CODEBEE_GAME_BEE_H
