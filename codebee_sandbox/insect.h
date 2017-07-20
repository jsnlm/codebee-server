#ifndef CODEBEE_GAME_INSECT_H
#define CODEBEE_GAME_INSECT_H

#include <memory>

#include "face.h"
#include "position.h"

class Insect {
public:
    enum InsectType {
        BEE = 0,
        WASP = 1,
        QUEEN = 2
    };

    Insect(int id, int botId, Position pos, Face face = face::UP);

    const int getId() const;
    const int getBotId() const;
    const InsectType getType() const;
    const int getCount() const;
    const int getPollen() const;
    const int getFace() const;
    const Position getPosition() const;

    virtual const int getAttack() const;
    virtual void collectPollen(int amount);

    /**
     * Updates how much damage should be inflicted to this insect. DOES NOT deal the damage.
     * Call commitDamage() to actually take away from count.
     */
    virtual void takeDamage(int damage);

    /**
     * @return the attack power needed to wipe out this insect group.
     */
    virtual const int getKillAmount() const;

    /**
     * Gives away all of this insect group's pollen.
     *
     * @return pollen of this insect group.
     */
    virtual int depositPollen();

    virtual std::shared_ptr<Insect> createInsect(int nextInsectId, Position pos, Face f, int c, int p) = 0;

    /**
     * Merge another insect group with this group.
     *
     * @param insect is the group of insects to be merged.
     */
    void merge(std::shared_ptr<Insect> insect);

    /**
     * Separate this insect group into two groups.
     *
     * @param amount is the number of insects to split off into the second group.
     * @return the second group of insects that were split off.
     */
    std::shared_ptr<Insect> splitOff(int amount, Face f, int nextInsectId, Position pos);

    void changeFace(Face f);
    void setPosition(const Position p);

    /**
     * Actually commits all the damage that was done to the insect.
     */
    void commitDamage();
protected:
    int id;
    int botId;
    Position pos;
    Face face;
    InsectType type;
    int count;
    int pollen;
    int maxCount;
    int maxPollenFactor;
    int totalDamage;
};

static const int KILL_PRIORITY[3] = { Insect::WASP, Insect::BEE, Insect::QUEEN };

#endif //CODEBEE_GAME_INSECT_H
