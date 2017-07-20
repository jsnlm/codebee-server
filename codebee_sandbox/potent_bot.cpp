#include <time.h>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "bot.h"
#include "action.h"
#include "cell.h"
#include "flower.h"
#include "insect.h"
#include "position.h"
#include "map.h"

using namespace std;

fstream fs;

class PotentBot : public Bot {
public:
    PotentBot() : Bot("PotentBot") { }

protected:
    vector<Action> getMoves(int id, const Map& curMap) {
        vector<Action> moves;
        vector<Cell*> flowerCells;
        vector<Cell*> beeCells;
        Cell* queenBeeCell;

        for (int y = 0; y < curMap.height; ++y) {
            for (int x = 0; x < curMap.width; ++x) {
                if (curMap.map[y][x]->getPotency() == flower::POTENT) {
                    flowerCells.push_back(curMap.map[y][x]);
                }

                shared_ptr<Insect> insect = curMap.map[y][x]->getInsect();
                if (insect && insect->getBotId() == id) {
                    if (insect->getType() == Insect::QUEEN) {
                        queenBeeCell = curMap.map[y][x];
                    } else {
                        beeCells.push_back(curMap.map[y][x]);
                    }
                }
            }
        }

        for (auto beeCell = beeCells.begin(); beeCell != beeCells.end(); ++beeCell) {
            shared_ptr<Insect> insect = (*beeCell)->getInsect();
            Position pos = (*beeCell)->getPosition();
            if (insect->getPollen() >= insect->getCount()) {
                Map::Path path = curMap.getPath(pos, queenBeeCell->getPosition());
                moves.push_back(
                        Action(pos.x, pos.y, path.move, path.move - 1 /* Faces move direction. */, insect->getCount()));
            } else {
                Cell* firstFlower = flowerCells[0];
                Map::Path minPath = curMap.getPath(pos, firstFlower->getPosition());
                for (auto flowerCell = flowerCells.begin() + 1; flowerCell != flowerCells.end(); ++flowerCell) {
                    Map::Path path = curMap.getPath(pos, (*flowerCell)->getPosition());
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
};

int main() {
    srand(time(0));

    fs.open("potent_bot_log.txt", fstream::out | fstream::trunc);

    cout.sync_with_stdio(false);

    PotentBot randomBot = PotentBot();

    randomBot.run();

    fs.close();
}
