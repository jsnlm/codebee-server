#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <future>

#include "game.h"
#include "messaging.h"
#include "json.hpp"

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

int main(int argc, char ** argv) {
    Game game;
    Messaging messaging;

    string initMap = game.getInitMap();
    const int numBots = argc - 1;

    cout << "Number of bots " << to_string(numBots) << endl;

    // TODO: better way to get bot executables
    for (int id = 0; id < numBots; id++) {
        string command = argv[id + 1];

        cout << "Bot #" << to_string(id) << ", Command: " << command << endl;

        messaging.startAndConnectBot(command);
    }

    vector<future<int>> initBotThreads(numBots);
    for (int i = 0; i < numBots; i++) {
        initBotThreads[i] = async(&Messaging::handleInitialMessaging, &messaging, i, initMap);
    }

    for (int id = 0; id < numBots; id++) {
        if (initBotThreads[id].get() == -1) {
            messaging.killPlayer(id);
        }
    }

    int turn = 0;
    bool gameEnded = false;
    string currInsects = game.getInitInsects();
    for (; turn < 100 && !gameEnded; ++turn) {
        // Parse bots actions.
        vector<string> actions(numBots);
        map<int, vector<Action>> botsActions;

        vector<future<int>> turnThreads(numBots);
        for (int i = 0; i < numBots; i++) {
            turnThreads[i] = async(&Messaging::handleTurnMessaging, &messaging, i, currInsects, &actions[i]);
        }

        for (int id = 0; id < numBots; id++) {
            if (turnThreads[id].get() == -1) {
                messaging.killPlayer(id);
            }
            botsActions[id] = parseActions(actions[id]);
        }

        cout << "Turn " << (turn + 1) << " completed" << endl;

        currInsects = game.nextTurn(botsActions, gameEnded);
    }

    return 0;
}
