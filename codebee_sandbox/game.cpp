#include <iostream>
#include <sstream>

#include "bee.h"
#include "flower.h"
#include "game.h"
#include "queen_bee.h"

using namespace std;

Game::Game(int w, int h) : width(w), height(h), nextInsectId(0), turn(0) {
    char hardcodedMap[10][10] = {
        {'F', '-', 'F', '-', '-', '-', '-', '-', '-', 'F'},
        {'-', 'W', 'W', 'W', '-', 'F', '-', 'F', '-', '-'},
        {'-', 'W', '1', 'W', '-', 'F', '-', '-', 'F', '-'},
        {'-', 'W', 'W', 'W', '-', 'F', '-', '-', '-', '-'},
        {'-', '-', '-', '-', 'P', 'P', '-', '-', '-', '-'},
        {'-', '-', '-', '-', 'P', 'P', '-', '-', '-', '-'},
        {'-', '-', '-', '-', 'F', '-', 'B', 'B', 'B', '-'},
        {'-', 'F', '-', '-', 'F', '-', 'B', '2', 'B', '-'},
        {'-', '-', 'F', '-', 'F', '-', 'B', 'B', 'B', '-'},
        {'F', '-', '-', '-', '-', '-', '-', 'F', '-', 'F'},
    };
    char hardcodedMap2[13][13] = {
        { '-', '-', '-', '-', '-', '-', '-', '-', 'F', 'F', '-', '-', '-' },
        { '-', '-', 'W', 'W', 'W', '-', '-', '-', '-', 'F', '-', '-', '-' },
        { '-', '-', 'W', '1', 'W', '-', '-', '-', '-', '-', '-', '-', '-' },
        { '-', '-', 'W', 'W', 'W', 'F', '-', 'F', '-', '-', '-', '-', '-' },
        { '-', '-', '-', '-', 'F', '-', '-', '-', 'F', '-', '-', '-', '-' },
        { '-', '-', '-', 'F', '-', 'P', 'P', 'P', '-', 'F', '-', '-', '-' },
        { '-', '-', 'F', '-', '-', 'P', '-', 'P', '-', '-', 'F', '-', '-' },
        { '-', '-', '-', 'F', '-', 'P', 'P', 'P', '-', 'F', '-', '-', '-' },
        { '-', '-', '-', '-', 'F', '-', '-', '-', 'F', '-', '-', '-', '-' },
        { '-', '-', '-', '-', '-', 'F', '-', 'F', 'B', 'B', 'B', '-', '-' },
        { '-', '-', '-', '-', '-', '-', '-', '-', 'B', '2', 'B', '-', '-' },
        { '-', '-', '-', 'F', '-', '-', '-', '-', 'B', 'B', 'B', '-', '-' },
        { '-', '-', '-', 'F', 'F', '-', '-', '-', '-', '-', '-', '-', '-' },
    };

    map = new Cell**[height];
    for (int y = 0; y < height; ++y) {
        map[y] = new Cell*[width];
        for (int x = 0; x < width; ++x) {
            Position pos(x, y);
            shared_ptr<QueenBee> queen(nullptr);
            switch(hardcodedMap2[y][x]) {
                case 'F':
                    map[y][x] = new Cell(pos, flower::REGULAR);
                    break;
                case 'P':
                    map[y][x] = new Cell(pos, flower::POTENT);
                    break;
                case 'W':
                    map[y][x] = new Cell(pos, flower::NONE, make_shared<Bee>(nextInsectId++, 0, pos, face::UP));
                    break;
                case '1':
                    queen = make_shared<QueenBee>(nextInsectId++, 0, pos, face::UP);
                    queenBees.push_back(queen);
                    map[y][x] = new Cell(pos, flower::NONE, queen);
                    break;
                case 'B':
                    map[y][x] = new Cell(pos, flower::NONE, make_shared<Bee>(nextInsectId++, 1, pos, face::UP));
                    break;
                case '2':
                    queen = make_shared<QueenBee>(nextInsectId++, 1, pos, face::UP);
                    queenBees.push_back(queen);
                    map[y][x] = new Cell(pos, flower::NONE, queen);
                    break;
                default:
                    map[y][x] = new Cell(pos, flower::NONE);
                    break;
            }
        }
    }

    replay.init(map, width, height);
}

Game::~Game() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            delete map[y][x];
        }
        delete [] map[y];
    }
    delete [] map;

    replay.write();
}

string Game::getInitMap() const {
    return replay.getInitMap();
}

string Game::getInitInsects() const {
    return replay.getInitInsects();
}

string Game::getWinner() {
    if (winningIds.empty()) {
        computeWinners();
    }

    stringstream ss;
    ss << "The game has ended after " << turn << " turns." << endl;

    if (winningIds.size() == 1) {
        int winner = winningIds.front();
        if (winner == -1) {
            // TODO find out who ties.
            ss << "Tie game!";
        } else {
            ss << "Bot " << winner << " wins!";
        }
    } else {
        ss << "Bots ";
        for (auto winner = winningIds.begin(); winner != winningIds.end(); ++winner) {
            if (winner != winningIds.begin()) {
                ss << ",";
            }
            ss << *winner;
        }
        ss << " all tie for first!";
    }

    return ss.str();
}

string Game::nextTurn(std::map<int, vector<Action> > botsActions, bool& gameEnded) {
    updateMap(botsActions);
    spawnBees();
    resolveConflicts();
    computeDamages();
    calculatePollen();
    gameEnded = didGameEnd();

    turn++;
    return replay.endTurn(map, width, height);
}

void Game::updateMap(std::map<int, vector<Action> > botsActions) {
    for (auto bot = botsActions.begin(); bot != botsActions.end(); ++bot) {
        for (auto action = bot->second.begin(); action != bot->second.end(); ++action) {
            Position pos = action->pos;
            if (pos.y < height && pos.x < width) {
                shared_ptr<Insect> insect = map[pos.y][pos.x]->getInsect();
                if (insect) {
                    Position insectPos = insect->getPosition();
                    // Validate that the insect belongs to this bot, and also it has not been moved yet.
                    if (insect->getBotId() == bot->first && pos == insectPos) {
                        switch (action->move) {
                            case move::UP:
                                makeMove(Position(pos.x, (height + pos.y - 1) % height), pos, *action);
                                break;
                            case move::RIGHT:
                                makeMove(Position((pos.x + 1) % width, pos.y), pos, *action);
                                break;
                            case move::DOWN:
                                makeMove(Position(pos.x, (pos.y + 1) % height), pos, *action);
                                break;
                            case move::LEFT:
                                makeMove(Position((width + pos.x - 1) % width, pos.y), pos, *action);
                                break;
                            default:
                                insect->changeFace(action->face);
                                break;
                        }
                    } // Else invalid action.
                }
            }
        }
    }
}

void Game::spawnBees() {
    for (auto queenBee = queenBees.begin(); queenBee != queenBees.end(); ++queenBee) {
        int bees = (*queenBee)->spawnBees();
        if (bees > 0) {
            Position pos = (*queenBee)->getPosition();
            switch ((*queenBee)->getFace()) {
                case face::UP:
                    pos.y = (pos.y + 1) % height;
                    break;
                case face::RIGHT:
                    pos.x = (width + pos.x - 1) % width;
                    break;
                case face::DOWN:
                    pos.y = (height + pos.y - 1) % height;
                    break;
                case face::LEFT:
                    pos.x = (pos.x + 1) % width;
                    break;
            }
            map[pos.y][pos.x]->enqueueInsect(
                    make_shared<Bee>(nextInsectId++, (*queenBee)->getBotId(), pos, (*queenBee)->getFace(), bees));
        }
    }
}

void Game::resolveConflicts() {
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            map[y][x]->resolveConflicts();
        }
    }
}

void Game::computeDamages() {
    // Compute total damage for each insect.
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            shared_ptr<Insect> insect = map[y][x]->getInsect();
            if (insect && insect->getType() != Insect::QUEEN) {
                switch (insect->getFace()) {
                    case face::UP:
                        maybeAttack(insect->getBotId(), insect->getAttack(), Position(x - 1, y - 1));
                        maybeAttack(insect->getBotId(), insect->getAttack(), Position(x, y - 1));
                        maybeAttack(insect->getBotId(), insect->getAttack(), Position(x + 1, y - 1));
                        break;
                    case face::RIGHT:
                        maybeAttack(insect->getBotId(), insect->getAttack(), Position(x + 1, y - 1));
                        maybeAttack(insect->getBotId(), insect->getAttack(), Position(x + 1, y));
                        maybeAttack(insect->getBotId(), insect->getAttack(), Position(x + 1, y + 1));
                        break;
                    case face::DOWN:
                        maybeAttack(insect->getBotId(), insect->getAttack(), Position(x - 1, y + 1));
                        maybeAttack(insect->getBotId(), insect->getAttack(), Position(x, y + 1));
                        maybeAttack(insect->getBotId(), insect->getAttack(), Position(x + 1, y + 1));
                        break;
                    case face::LEFT:
                        maybeAttack(insect->getBotId(), insect->getAttack(), Position(x - 1, y - 1));
                        maybeAttack(insect->getBotId(), insect->getAttack(), Position(x - 1, y));
                        maybeAttack(insect->getBotId(), insect->getAttack(), Position(x - 1, y + 1));
                        break;
                }
            }
        }
    }

    // Commit damages, and remove the dead.
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            shared_ptr<Insect> insect = map[y][x]->getInsect();
            if (insect) {
                insect->commitDamage();
                if (insect->getCount() <= 0) {
                    map[y][x]->dequeueInsect();
                    map[y][x]->resolveConflicts();
                }
            }
        }
    }
}

void Game::calculatePollen() {
    for (auto queenBee = queenBees.begin(); queenBee != queenBees.end(); ++queenBee) {
        Position pos = (*queenBee)->getPosition();
        maybeHarvestPollen(*queenBee, Position(pos.x - 1, pos.y + 1));
        maybeHarvestPollen(*queenBee, Position(pos.x, pos.y + 1));
        maybeHarvestPollen(*queenBee, Position(pos.x + 1, pos.y + 1));
        maybeHarvestPollen(*queenBee, Position(pos.x - 1, pos.y - 1));
        maybeHarvestPollen(*queenBee, Position(pos.x, pos.y - 1));
        maybeHarvestPollen(*queenBee, Position(pos.x + 1, pos.y - 1));
        maybeHarvestPollen(*queenBee, Position(pos.x - 1, pos.y));
        maybeHarvestPollen(*queenBee, Position(pos.x + 1, pos.y));
    }

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            shared_ptr<Insect> insect = map[y][x]->getInsect();
            if (insect) {
                insect->collectPollen(map[y][x]->getPotency());
            }
        }
    }
}

void Game::makeMove(Position newPos, Position currPos, Action action) {
    if (action.amount > 0) {
        shared_ptr<Insect> insect = map[currPos.y][currPos.x]->getInsect();
        // Case when user doesn't move all bees off the square.
        if (action.amount < insect->getCount()) {
            shared_ptr<Insect> newInsect = insect->splitOff(action.amount, action.face, nextInsectId++, newPos);
            map[newPos.y][newPos.x]->enqueueInsect(newInsect);
            replay.pushNewInsect(currPos, newInsect);
            replay.updateInsect(insect->getId(), insect->getCount(), insect->getPollen());
            replay.pushMove(newInsect->getId(), action.move);
        } else {
            shared_ptr<Insect> insectToMove = map[currPos.y][currPos.x]->dequeueInsect();
            insectToMove->changeFace(action.face);
            insectToMove->setPosition(newPos);
            map[newPos.y][newPos.x]->enqueueInsect(insectToMove);
            replay.pushMove(insectToMove->getId(), action.move);
        }
    }
}

bool Game::didGameEnd() {
    int winningId = -1;
    bool isWinPossible = true;
    for (int y = 0; y < height && isWinPossible; ++y) {
        for (int x = 0; x < width && isWinPossible; ++x) {
            shared_ptr<Insect> insect = map[y][x]->getInsect();
            if (insect && insect->getType() != Insect::QUEEN) {
                if (winningId == -1) {
                    winningId = insect->getBotId();
                    winningIds.push_back(winningId);
                } else if (winningId != insect->getBotId()) {
                    winningIds.clear();
                    isWinPossible = false;
                }
            }
        }
    }
    return isWinPossible;
}

void Game::maybeAttack(int botId, int attack, Position pos) {
    pos.y = (pos.y + height) % height;
    pos.x = (pos.x + width) % width;
    shared_ptr<Insect> insect = map[pos.y][pos.x]->getInsect();
    if (insect && insect->getBotId() != botId) {
        insect->takeDamage(attack);
    }
}

void Game::maybeHarvestPollen(shared_ptr<QueenBee> queenBee, Position pos) {
    pos.y = (pos.y + height) % height;
    pos.x = (pos.x + width) % width;
    shared_ptr<Insect> insect = map[pos.y][pos.x]->getInsect();
    if (insect && insect->getBotId() == queenBee->getBotId()) {
        queenBee->takePollen(insect->depositPollen());
    }
}

void Game::computeWinners() {
    std::map<int, float> botPoints;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            shared_ptr<Insect> insect = map[y][x]->getInsect();
            if (insect) {
                botPoints[insect->getBotId()] += insect->getCount();
            }
        }
    }

    float maxPoints = 0;
    for (auto botPoint = botPoints.begin(); botPoint != botPoints.end(); ++botPoint) {
        botPoint->second += queenBees[botPoint->first]->getPollen() / 5.0f;
        cout << botPoint->first << " scored " << botPoint->second << " points!" << endl;
        if (maxPoints < botPoint->second) {
            winningIds.clear();
            winningIds.push_back(botPoint->first);
            maxPoints = botPoint->second;
        } else if (maxPoints == botPoint->second) {
            winningIds.push_back(botPoint->first);
        }
    }
}
