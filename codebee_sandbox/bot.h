#ifndef CODEBEE_GAME_BOT_H
#define CODEBEE_GAME_BOT_H

#include <string>
#include <vector>

#include "map.h"
#include "action.h"

class Bot {
public:
    Bot(std::string name);
    ~Bot();
    void run();

protected:
    std::string getString();
    void sendString(std::string send);

    void getInit(int &id, Map* &initialMap);
    void sendInit(std::string name);
    void getTurn(Map& curMap);
    void sendTurn(std::vector<Action> moves);

    virtual std::vector<Action> getMoves(int id, const Map& curMap) = 0;

    int id;
    std::string name;
    Map* curMap;

private:
    std::string serializeMoves(std::vector<Action> moves);
};

#endif //CODEBEE_GAME_BOT_H
