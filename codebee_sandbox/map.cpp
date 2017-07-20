#include <string>

#include "map.h"
#include "json.hpp"
#include "position.h"
#include "insect.h"
#include "bee.h"
#include "queen_bee.h"
#include "face.h"
#include "position.h"

using namespace std;
using namespace nlohmann;

Map::Map(string pollenMap) {
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

Map::~Map() {
    if (map != NULL) {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                delete map[y][x];
            }
            delete[] map[y];
        }
        delete[] map;
    }
}

void Map::updateMap(std::string newInsects) {
    json pInsects = json::parse(newInsects);

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


Map::Path Map::getPath(Position from, Position to) const {
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

