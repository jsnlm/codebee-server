#include <fstream>
#include <sstream>

#include "replay.h"

using namespace std;

Replay::Replay() { }

string Replay::getInitMap() const {
    return pollenMap;
}

string Replay::getInitInsects() const {
    return initialInsects;
}

void Replay::init(Cell*** map, int width, int height) {
    // Initialize the map.
    stringstream ss;
    ss << "[";
    for (int y = 0; y < height; ++y) {
        if (y != 0) {
            ss << ",";
        }

        ss << "[";
        for (int x = 0; x < width; ++x) {
            if (x != 0) {
                ss << ",";
            }

            ss << "{"
               << "\"p\":" << map[y][x]->getPotency()
               << "}";
        }
        ss << "]";
    }
    ss << "]";
    pollenMap = ss.str();

    // Initialize the starting turn.
    initialInsects = endTurn(map, width, height);
}

void Replay::pushNewInsect(Position pos, const shared_ptr<Insect> insect) {
    n.push_back(getInsectData(pos, insect));
}

void Replay::pushMove(int id, Move move) {
    stringstream ss;
    ss << "{"
       << "\"i\":" << id
       << ",\"m\":" << move
       << "}";
    t.push_back(ss.str());
}

void Replay::updateInsect(int id, int count, int pollen) {
    u[id] = Update(count, pollen);
}

string Replay::endTurn(Cell*** map, int width, int height) {
    stringstream ss;
    ss << "{";

    // Moves.
    ss << "\"t\":[";
    for (int i = 0; i < t.size(); ++i) {
        if (i != 0) {
            ss << ",";
        }

        ss << t[i];
    }
    ss << "]";

    // New insects.
    ss << ",\"n\":[";
    for (auto it = n.begin(); it != n.end(); ++it) {
        if (it != n.begin()) {
            ss << ",";
        }

        ss << *it;
    }
    ss << "]";

    // Updated insects.
    ss << ",\"u\":{";
    for (auto it = u.begin(); it != u.end(); ++it) {
        if (it != u.begin()) {
            ss << ",";
        }
        ss << "\"" << it->first << "\":" << "{"
           << "\"c\":" << it->second.count
           << ",\"p\":" << it->second.pollen
           << "}";
    }
    ss << "}";

    // Push final insect statuses after this turn has been computed.
    string finalInsectData = getAllInsectData(map, width, height);
    ss << ",\"i\":" << finalInsectData;

    ss << "}";

    turns.push_back(ss.str());

    // Reset the turn variables.
    n.clear();
    t.clear();
    u.clear();

    return finalInsectData;
}

void Replay::write() {
    ofstream replay;
    replay.open("replay.cb");
    replay << "{";

    // Map.
    replay << "\"m\":" << pollenMap;

    // Turns.
    replay << ",\"t\":[";
    for (auto it = turns.begin(); it != turns.end(); ++it) {
        if (it != turns.begin()) {
            replay << ",";
        }

        replay << *it;
    }
    replay << "]";

    // Initial insects.
    replay << ",\"i\":" << initialInsects;

    replay << "}";
    replay.close();
}

string Replay::getInsectData(Position pos, const shared_ptr<Insect> insect) {
    stringstream ss;
    ss << "{"
       << "\"x\":" << pos.x
       << ",\"y\":" << pos.y
       << ",\"i\":" << insect->getId()
       << ",\"b\":" << insect->getBotId()
       << ",\"c\":" << insect->getCount()
       << ",\"f\":" << insect->getFace()
       << ",\"p\":" << insect->getPollen()
       << ",\"t\":" << insect->getType()
       << "}";
    return ss.str();
}

string Replay::getAllInsectData(Cell*** map, int width, int height) {
    stringstream ss;
    ss << "[";
    bool first = true;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            shared_ptr<Insect> insect = map[y][x]->getInsect();
            if (insect) {
                if (!first) {
                    ss << ",";
                } else {
                    first = false;
                }
                ss << getInsectData(Position(x, y), insect);
            }
        }
    }
    ss << "]";
    return ss.str();
}
