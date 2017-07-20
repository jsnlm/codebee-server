#include <string>
#include <iostream>
#include <vector>
#include <fstream>

#include "bot.h"
#include "json.hpp"
#include "action.h"

using namespace std;
using namespace nlohmann;

extern fstream fs;

Bot::Bot(string name) : name(name) {
    getInit(id, curMap);
    sendInit(name);
}

Bot::~Bot() {
    delete curMap;
}

void Bot::run() {
    vector<Action> moves;

    int turn = 1;
    while(true) {
        moves.clear();

        fs << "Start of turn " << turn << endl;

        getTurn(*curMap);

        fs << "Map retrieved for turn" << endl;

        moves = getMoves(id, *curMap);

        fs << "Moves calculated for turn" << endl;

        sendTurn(moves);

        fs << "End of turn " << turn << endl;

        turn++;
    }
}

void Bot::getInit(int &id, Map* &initialMap) {
    id = std::stoi(getString());
    const std::string map = getString();
    fs << "Id " << id << std::endl;
    fs << "Map:" << std::endl << map << std::endl;
    initialMap = new Map(map);
}

void Bot::sendInit(std::string name) {
    sendString(name);
}

void Bot::getTurn(Map& curMap) {
    string curInsects = getString();
    fs << "CurrInsects:" << endl << curInsects << endl;
    curMap.updateMap(curInsects);
}

void Bot::sendTurn(std::vector<Action> moves) {
    string strMoves = serializeMoves(moves);
    fs << "Moves:" << endl << strMoves << endl;
    sendString(strMoves);
}

void Bot::sendString(string send) {
    cout << send << endl; // Flushes stream
}

string Bot::getString() {
    string response;
    getline(cin, response);
    return response;
}

std::string Bot::serializeMoves(std::vector<Action> moves) {
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