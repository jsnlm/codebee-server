#include "cell.h"
#include "queen_bee.h"

#include <iostream>

using namespace std;

Cell::Cell(Position pos, int p, shared_ptr<Insect> i) : pos(pos), potency(p), mainInsect(i) {
    if (i) {
        insects.push_back(i);
    }
}

shared_ptr<Insect> Cell::getInsect() const {
    return mainInsect;
}

const int Cell::getPotency() const {
    return potency;
}

const Position Cell::getPosition() const {
    return pos;
}

void Cell::enqueueInsect(shared_ptr<Insect> insect) {
    insects.insert(insects.begin(), insect);
}

shared_ptr<Insect> Cell::dequeueInsect() {
    shared_ptr<Insect> insect = insects.back();
    insects.pop_back();
    return insect;
}

void Cell::resolveConflicts() {
    if (insects.size() > 1) {
        map<int, InsectTeam> teams = separateInsects();
        computeDamages(teams);
        computeLastManStanding(teams);
    } else if (insects.size() == 1) {
        mainInsect = insects[0];
    } else {
        mainInsect = shared_ptr<Insect>(nullptr);
    }
}

map<int, Cell::InsectTeam> Cell::separateInsects() {
    map<int, InsectTeam> teams;
    while (insects.size() > 0) {
        // Taking from front will make last insect's face take precedence.
        shared_ptr<Insect> insect = insects.front();
        insects.erase(insects.begin());
        int botId = insect->getBotId();

        if (teams.find(botId) == teams.end()) {
            teams[botId] = InsectTeam();
        }

        teams[botId].attackPower += insect->getAttack();
        teams[botId].types[insect->getType()].push_back(insect);
    }
    return teams;
}

void Cell::computeDamages(map<int, InsectTeam> teams) {
    for (auto team = teams.begin(); team != teams.end(); ++team) {
        int maxAttackPower = 0;
        // Get the maximum damage this team will take.
        for (auto team2 = teams.begin(); team2 != teams.end(); ++team2) {
            if (team2 != team && team2->second.attackPower > maxAttackPower) {
                maxAttackPower = team2->second.attackPower;
            }
        }

        // Take the damage in order of kill priorities.
        for (int i = 0; i < 3; ++i) {
            int killType = KILL_PRIORITY[i];
            vector<shared_ptr<Insect> > insects = team->second.types[killType];
            for (auto insect = insects.begin(); insect != insects.end(); ++insect) {
                if (maxAttackPower == 0) {
                    break;
                }

                if (killType == Insect::QUEEN) {
                    (*insect)->takeDamage(maxAttackPower);
                    maxAttackPower = 0;
                } else {
                    int killAmount = (*insect)->getKillAmount();
                    if (killAmount < maxAttackPower) {
                        (*insect)->takeDamage(killAmount);
                        maxAttackPower -= killAmount;
                    } else {
                        (*insect)->takeDamage(maxAttackPower);
                        maxAttackPower = 0;
                    }
                }
                (*insect)->commitDamage();
            }
        }
    }
}

void Cell::computeLastManStanding(map<int, InsectTeam> teams) {
    int survivingTeams = 0;
    for (auto team = teams.begin(); team != teams.end(); ++team) {
        shared_ptr<Insect> finalInsect(nullptr);
        for (int i = 0; i < 3; ++i) {
            int killType = KILL_PRIORITY[i];
            vector<shared_ptr<Insect> > insects = team->second.types[killType];
            if (insects.size() > 0) {
                shared_ptr<Insect> primeInsect = insects.back();
                insects.pop_back();
                for (; insects.size() > 0;) {
                    shared_ptr<Insect> mergeInsect = insects.back();
                    insects.pop_back();
                    primeInsect->merge(mergeInsect);
                }

                if (primeInsect->getCount() > 0) {
                    // If other bees went on queen bee, just turn the bee into pollen.
                    if (finalInsect && primeInsect->getType() == Insect::QUEEN) {
                        std::dynamic_pointer_cast<QueenBee>(primeInsect)->takePollen(finalInsect->getCount() * 5);
                    }
                    finalInsect = primeInsect;
                }
            }
        }

        if (finalInsect && finalInsect->getCount() > 0) {
            mainInsect = finalInsect;
            ++survivingTeams;
        }
    }

    // Only time this condition is satisfied is when 2 queen bees move to the same cell. Kill them all then.
    if (survivingTeams > 1 || survivingTeams == 0) {
        mainInsect = shared_ptr<Insect>(nullptr);
    } else {
        insects.insert(insects.begin(), mainInsect);
    }
}