#ifndef CODEBEE_GAME_GAME_H
#define CODEBEE_GAME_GAME_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "action.h"
#include "cell.h"
#include "position.h"
#include "queen_bee.h"
#include "replay.h"

class Game {
public:
    Game(int w = 13, int h = 13);
    ~Game();

    std::string getInitMap() const;
    std::string getInitInsects() const;
    std::string getWinner();

    std::string nextTurn(std::map<int, std::vector<Action> > botsActions, bool& gameEnded);
private:
    /**
     * Aggregate all the actions into the map. Conflicting bees on a cell will stack on top of each other.
     */
    void updateMap(std::map<int, std::vector<Action> > botsActions);

    /**
     * Spawning phase. All queen bees will spawn bees.
     */
    void spawnBees();

    /**
     * Resolve conflicts phase. All cells that have more than one bee group will be resolved into 1 group.
     */
    void resolveConflicts();

    /**
     * Attack phase. All bees will use their attacks.
     */
    void computeDamages();

    /**
     * Collect pollen phase. Bees on flowers will harvest pollen, while queen bees will take pollen from nearby bees.
     */
    void calculatePollen();

    void makeMove(Position newPos, Position currPos, Action action);
    bool didGameEnd();
    void maybeAttack(int botId, int attack, Position pos);
    void maybeHarvestPollen(std::shared_ptr<QueenBee> queenBee, Position pos);

    void computeWinners();

    const int width;
    const int height;

    Cell*** map;
    std::vector<std::shared_ptr<QueenBee> > queenBees;

    int nextInsectId;
    int turn;

    Replay replay;

    std::vector<int> winningIds;
};

#endif //CODEBEE_GAME_GAME_H
