#include <time.h>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "bot.h"
#include "action.h"
#include "insect.h"

using namespace std;

fstream fs;

class RandomBot : public Bot {
public:
    RandomBot() : Bot("RandomBot") { }

protected:
    vector<Action> getMoves(int id, const Map& curMap) {
        vector<Action> moves;
        for (int y = 0; y < curMap.height; ++y) {
            for (int x = 0; x < curMap.width; ++x) {
                shared_ptr<Insect> insect = curMap.map[y][x]->getInsect();
                if (insect && insect->getBotId() == id) {
                    int move = rand() % 5;
                    moves.push_back(Action(x, y, move, move > 0 ? move - 1 : insect->getFace(), rand() % (insect->getCount() + 1)));
                }
            }
        }
        return moves;
    }
};

int main() {
    srand(time(0));

    fs.open("random_bot_log.txt", fstream::out | fstream::trunc);

    cout.sync_with_stdio(false);

    RandomBot randomBot = RandomBot();

    randomBot.run();

    fs.close();
}