//
// Created by William on 7/3/2017.
//

#ifndef CODEBEE_GAME_BOTS_H
#define CODEBEE_GAME_BOTS_H

#include <string>
#include <vector>

#include "action.h"
#include "cell.h"
#include "move.h"
#include "position.h"

class Bot {
public:
    Bot(int id, std::string pollenMap);
    ~Bot();

    const int getId() const;

    std::string computeNextMove(std::string insects);
protected:
    struct Path {
        int distance;
        Move move;

        Path(int d = 0, Move m = move::UP) : distance(d), move(m) { }
    };

    virtual std::vector<Action> getMoves() = 0;

    void updateMap(std::string insects);
    std::string stringifyMoves(std::vector<Action> moves) const;

    Path getPath(Position from, Position to) const;

    int id;
    Cell*** map;
    int width;
    int height;
};

class RandomBot : public Bot {
public:
    RandomBot(int id, std::string pollenMap);

protected:
    std::vector<Action> getMoves() override;
};

class HarvesterBot : public Bot {
public:
    HarvesterBot(int id, std::string pollenMap, int minPollen = 1);

protected:
    std::vector<Action> getMoves() override;

private:
    int minPollen;
};

class PotentBot : public Bot {
public:
    PotentBot(int id, std::string pollenMap, int minPollen = 1);

protected:
    std::vector<Action> getMoves() override;

private:
    int minPollen;
};

#endif //CODEBEE_GAME_BOTS_H
