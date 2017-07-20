#ifndef CODEBEE_GAME_NETWORKING_H
#define CODEBEE_GAME_NETWORKING_H

#include <string>
#include <vector>

class Messaging {
public:
    void startAndConnectBot(std::string command);
    int handleInitialMessaging(int id, std::string initialMap);
    int handleTurnMessaging(int id, std::string currInsects, std::string* actions);
    void killPlayer(int id);

private:
    const int timeoutMilliseconds = 1000;
    struct UnixConnection {
        int read, write;
    };
    std::vector<UnixConnection> connections;
    std::vector<int> processes;

    void sendString(int id, std::string sendString);
    std::string getString(int id);

    bool isProcessDead(int id);
};

#endif //CODEBEE_GAME_NETWORKING_H
