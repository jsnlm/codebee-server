#ifndef CODEBEE_GAME_ACTION_H
#define CODEBEE_GAME_ACTION_H

#include "face.h"
#include "move.h"
#include "position.h"

struct Action {
    const Position pos;
    const Move move;
    const Face face;
    const int amount;

    Action(int x = 0, int y = 0, Move move = move::STAY, Face face = face::UP, int amount = 0) :
        pos(Position(x, y)), move(move), face(face), amount(amount) { }
};

#endif //CODEBEE_GAME_ACTION_H
