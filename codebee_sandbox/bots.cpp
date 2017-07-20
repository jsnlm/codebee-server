#include "bee.h"
#include "bots.h"
#include "face.h"
#include "json.hpp"
#include "insect.h"
#include "queen_bee.h"

#include <iostream>
#include <time.h>

using namespace std;
using namespace nlohmann;

Bot::Bot(int id, string pollenMap) : id(id) {
    srand(time(0));

    json pMap = json::parse(pollenMap);
    height = pMap.size();
    width = pMap.at(0).size();
    map = new Cell**[height];
    for (int y = 0; y < height; ++y) {
        json row = pMap.at(y);
        map[y] = new Cell*[width];
        for (int x = 0; x < width; ++x) {
            json cell = row.at(x);
            map[y][x] = new Cell(Position(x, y), cell.at("p").get<int>());
        }
    }
}

Bot::~Bot() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            delete map[y][x];
        }
        delete [] map[y];
    }
    delete [] map;
}

const int Bot::getId() const {
    return id;
}

string Bot::computeNextMove(std::string insects) {
    updateMap(insects);
    vector<Action> moves = getMoves();
    return stringifyMoves(moves);
}

void Bot::updateMap(string insects) {
    json pInsects = json::parse(insects);

    // Clear all insects.
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (map[y][x]->getInsect()) {
                map[y][x]->dequeueInsect();
                map[y][x]->resolveConflicts();  // Just sets the main insect to nullptr.
            }
        }
    }

    for (int i = 0; i < pInsects.size(); ++i) {
        json pInsect = pInsects.at(i);
        shared_ptr<Insect> insect(nullptr);
        Position pos;
        switch (pInsect.at("t").get<Insect::InsectType>()) {
            case Insect::BEE:
                insect = make_shared<Bee>(
                    pInsect.at("i").get<int>(),
                    pInsect.at("b").get<int>(),
                    Position(pInsect.at("x").get<int>(),
                             pInsect.at("y").get<int>()),
                    pInsect.at("f").get<Face>(),
                    pInsect.at("c").get<int>(),
                    pInsect.at("p").get<int>()
                );
                break;
            case Insect::WASP:
                // TODO
                insect = make_shared<Bee>(
                    pInsect.at("i").get<int>(),
                    pInsect.at("b").get<int>(),
                    Position(pInsect.at("x").get<int>(),
                             pInsect.at("y").get<int>()),
                    pInsect.at("f").get<Face>(),
                    pInsect.at("c").get<int>(),
                    pInsect.at("p").get<int>()
                );
                break;
            case Insect::QUEEN:
                insect = make_shared<QueenBee>(
                    pInsect.at("i").get<int>(),
                    pInsect.at("b").get<int>(),
                    Position(pInsect.at("x").get<int>(),
                             pInsect.at("y").get<int>()),
                    pInsect.at("f").get<Face>(),
                    pInsect.at("c").get<int>(),
                    pInsect.at("p").get<int>()
                );
                break;
        }
        pos = insect->getPosition();
        map[pos.y][pos.x]->enqueueInsect(insect);
        map[pos.y][pos.x]->resolveConflicts();      // Just sets the main insect to this one.
    }
}

string Bot::stringifyMoves(vector<Action> moves) const {
    json stringified;
    for (auto move = moves.begin(); move != moves.end(); ++move) {
        json action;
        action["x"] = move->pos.x;
        action["y"] = move->pos.y;
        action["move"] = move->move;
        action["face"] = move->face;
        action["amount"] = move->amount;
        stringified.push_back(action);
    }
    return stringified.dump();
}

Bot::Path Bot::getPath(Position from, Position to) const {
    Path path;

    int downDist = (height + to.y - from.y) % height;
    int upDist = (height + from.y - to.y) % height;
    int rightDist = (width + to.x - from.x) % width;
    int leftDist = (width + from.x - to.x) % width;

    if (to.y != from.y) {
        if (downDist > upDist) {
            path.move = move::UP;
        } else {
            path.move = move::DOWN;
        }
    } else if (to.x != from.x) {
        if (rightDist > leftDist) {
            path.move = move::LEFT;
        } else {
            path.move = move::RIGHT;
        }
    }

    path.distance = min(rightDist, leftDist) + min(downDist, upDist);

    return path;
}


RandomBot::RandomBot(int id, std::string pollenMap) : Bot(id, pollenMap) { }

vector<Action> RandomBot::getMoves() {
    vector<Action> moves;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            shared_ptr<Insect> insect = map[y][x]->getInsect();
            if (insect && insect->getBotId() == id) {
                int move = rand() % 5;
                moves.push_back(Action(x, y, move, move > 0 ? move - 1 : insect->getFace(), rand() % (insect->getCount() + 1)));
            }
        }
    }
    return moves;
}


HarvesterBot::HarvesterBot(int id, std::string pollenMap, int minPollen) : Bot(id, pollenMap), minPollen(minPollen) { }

vector<Action> HarvesterBot::getMoves() {
    vector<Action> moves;
    vector<Cell*> flowerCells;
    vector<Cell*> beeCells;
    Cell* queenBeeCell;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (map[y][x]->getPotency() > 0) {
                flowerCells.push_back(map[y][x]);
            }

            shared_ptr<Insect> insect = map[y][x]->getInsect();
            if (insect && insect->getBotId() == id) {
                if (insect->getType() == Insect::QUEEN) {
                    queenBeeCell = map[y][x];
                } else {
                    beeCells.push_back(map[y][x]);
                }
            }
        }
    }

    for (auto beeCell = beeCells.begin(); beeCell != beeCells.end(); ++beeCell) {
        shared_ptr<Insect> insect = (*beeCell)->getInsect();
        Position pos = (*beeCell)->getPosition();
        if (insect->getPollen() >= minPollen * insect->getCount()) {
            Path path = getPath(pos, queenBeeCell->getPosition());
            moves.push_back(
                Action(pos.x, pos.y, path.move, path.move - 1 /* Faces move direction. */, insect->getCount()));
        } else {
            Cell* firstFlower = flowerCells[0];
            Path minPath = getPath(pos, firstFlower->getPosition());
            for (auto flowerCell = flowerCells.begin() + 1; flowerCell != flowerCells.end(); ++flowerCell) {
                Path path = getPath(pos, (*flowerCell)->getPosition());
                if (path.distance < minPath.distance) {
                    minPath = path;
                }
            }
            moves.push_back(
               Action(pos.x, pos.y, minPath.move, minPath.move - 1 /* Faces move direction. */, insect->getCount()));
        }
    }

    Position queenPos = queenBeeCell->getPosition();
    moves.push_back(Action(queenPos.x, queenPos.y, move::STAY, rand() % 4, 1));

    return moves;
}


PotentBot::PotentBot(int id, std::string pollenMap, int minPollen) : Bot(id, pollenMap), minPollen(minPollen) { }

vector<Action> PotentBot::getMoves() {
    vector<Action> moves;
    vector<Cell*> flowerCells;
    vector<Cell*> beeCells;
    Cell* queenBeeCell;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (map[y][x]->getPotency() == flower::POTENT) {
                flowerCells.push_back(map[y][x]);
            }

            shared_ptr<Insect> insect = map[y][x]->getInsect();
            if (insect && insect->getBotId() == id) {
                if (insect->getType() == Insect::QUEEN) {
                    queenBeeCell = map[y][x];
                } else {
                    beeCells.push_back(map[y][x]);
                }
            }
        }
    }

    for (auto beeCell = beeCells.begin(); beeCell != beeCells.end(); ++beeCell) {
        shared_ptr<Insect> insect = (*beeCell)->getInsect();
        Position pos = (*beeCell)->getPosition();
        if (insect->getPollen() >= minPollen * insect->getCount()) {
            Path path = getPath(pos, queenBeeCell->getPosition());
            moves.push_back(
                    Action(pos.x, pos.y, path.move, path.move - 1 /* Faces move direction. */, insect->getCount()));
        } else {
            Cell* firstFlower = flowerCells[0];
            Path minPath = getPath(pos, firstFlower->getPosition());
            for (auto flowerCell = flowerCells.begin() + 1; flowerCell != flowerCells.end(); ++flowerCell) {
                Path path = getPath(pos, (*flowerCell)->getPosition());
                if (path.distance < minPath.distance) {
                    minPath = path;
                }
            }
            moves.push_back(
                    Action(pos.x, pos.y, minPath.move, minPath.move - 1 /* Faces move direction. */, insect->getCount()));
        }
    }

    Position queenPos = queenBeeCell->getPosition();
    moves.push_back(Action(queenPos.x, queenPos.y, move::STAY, face::LEFT, 1));

    return moves;
}