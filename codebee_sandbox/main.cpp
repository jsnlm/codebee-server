#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "bots.h"
#include "face.h"
#include "game.h"
#include "json.hpp"
#include "move.h"

using namespace std;
using namespace nlohmann;

vector<Action> parseActions(string actions) {
    json pActions = json::parse(actions);
    vector<Action> parsedActions;
    for (int i = 0; i < pActions.size(); ++i) {
        json pAction = pActions.at(i);
        parsedActions.push_back(Action(
                pAction.at("x").get<int>(),
                pAction.at("y").get<int>(),
                pAction.at("move").get<Move>(),
                pAction.at("face").get<Face>(),
                pAction.at("amount").get<int>()
        ));
    }
    return parsedActions;
}

int main() {
    Game game;

    string initMap = game.getInitMap();

    vector<Bot*> bots;
//    bots.push_back(new PotentBot(0, initMap));
//    bots.push_back(new HarvesterBot(1, initMap));
    bots.push_back(new RandomBot(0, initMap));
    bots.push_back(new RandomBot(1, initMap));

    int turn = 0;
    bool gameEnded = false;
    string currInsects = game.getInitInsects();
    for (; turn < 100 && !gameEnded; ++turn) {
        // Parse bots actions.
        map<int, vector<Action> > botsActions;

        for (auto bot = bots.begin(); bot != bots.end(); ++bot) {
            string actions = (*bot)->computeNextMove(currInsects);
            botsActions[(*bot)->getId()] = parseActions(actions);
        }

        currInsects = game.nextTurn(botsActions, gameEnded);
    }

    cout << game.getWinner() << endl;
    return 0;
}
