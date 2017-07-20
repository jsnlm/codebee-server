//
// Created by William on 7/2/2017.
//

#ifndef CODEBEE_GAME_CELL_H
#define CODEBEE_GAME_CELL_H

#include <map>
#include <memory>
#include <vector>

#include "flower.h"
#include "insect.h"
#include "position.h"

class Cell {
public:
    Cell(Position pos, Flower p, std::shared_ptr<Insect> i = std::shared_ptr<Insect>(nullptr));

    std::shared_ptr<Insect> getInsect() const;
    const int getPotency() const;
    const Position getPosition() const;

    /**
     * Adds an insect group to this cell. This function behaves like a queue.
     *
     * @param insect group to be added.
     */
    void enqueueInsect(std::shared_ptr<Insect> insect);

    /**
     * Removes an insect group to this cell. This function behaves like a queue.
     *
     * @return the insect group that was removed.
     */
    std::shared_ptr<Insect> dequeueInsect();

    /**
     * All insect groups in this cell will merge or fight until there is one group remaining.
     */
    void resolveConflicts();
private:
    struct InsectTeam {
        int attackPower;                    // Aggregation of total attack for this team.
        std::vector<std::shared_ptr<Insect>> types[3];       // Each index contains all insects of that type.

        InsectTeam() : attackPower(0) { }
    };

    /**
     * Sorts the insects into their teams and also separate into their insect types.
     *
     * @return a map of botId to InsectTeam.
     */
    std::map<int, InsectTeam> separateInsects();

    /**
     * Take damage. There should be only one team left in the end, unless a queen bee is involved.
     *
     * @param teams the separated InsectTeams.
     */
    void computeDamages(std::map<int, InsectTeam> teams);

    /**
     * Remove the dead and squash remaining insects to last group standing, which will become the new main insect group.
     *
     * @param teams the separated InsectTeams.
     */
    void computeLastManStanding(std::map<int, InsectTeam> teams);

    const Position pos;
    const int potency;
    std::vector<std::shared_ptr<Insect> > insects;
    std::shared_ptr<Insect> mainInsect;
};


#endif //CODEBEE_GAME_CELL_H
