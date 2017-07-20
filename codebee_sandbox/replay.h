/*
Replay file is in the following format
{
  // Map is a 2D array of objects which currently just have a p value for pollen.
  m: [
    [{ p: 0 }, { p: 1 }],
    [{ p: 1 }, { p: 2 }]
  ],
  // Turns is an array where each item represents a turn's data.
  t: [
    {
      // Insect data contains the final placements of each insect.
      // x = x position of insect
      // y = y position of insect
      // i = id of insect
      // b = botId of insect
      // c = insect count
      // f = insect face
      // p = insect pollen count
      // t = insect type
      i: [{ x: 4, y: 10, i: 15, b: 2, c: 15, f: 0, p: 10, t: 0 }],
      // Turn data contains the list of moves for each insect.
      // i = id of insect
      // m = move
      t: [{ i: 15, m: 1 }],
      // New insects contains the insects that split off from a group. The parameters are the same as insect data
      n: [{ x: 3, y: 3, i: 28, b: 1, c: 7, f: 0, p: 4, t: 0 }],
      // Updated insects contains the updated count and pollen for an existing insect that split into new insect groups.
      // The key is the insect ID
      // c = count
      // p = pollen
      u: { 3: { c: 2, p: 3 } }
    }
  ]
}
 */

#ifndef CODEBEE_GAME_REPLAY_H
#define CODEBEE_GAME_REPLAY_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "cell.h"
#include "insect.h"
#include "move.h"
#include "position.h"

class Replay {
public:
    Replay();

    std::string getInitMap() const;
    std::string getInitInsects() const;

    /**
     * Initialize the game map and starting insects.
     */
    void init(Cell*** map, int width, int height);

    /**
     * Push a new insect that was created on this turn.
     */
    void pushNewInsect(Position pos, const std::shared_ptr<Insect> insect);

    /**
     * Push a move that was issued on this turn.
     */
    void pushMove(int id, Move move);

    /**
     * Push updated insects that had changes in count or pollen for this turn.
     */
    void updateInsect(int id, int count, int pollen);

    /**
     * Finishes the turn and resets metadata for next turn.
     *
     * @return the final stringified statuses of all insects.
     */
    std::string endTurn(Cell*** map, int width, int height);

    /**
     * Writes replay into replay file.
     */
    void write();
private:
    struct Update {
        int count;
        int pollen;

        Update(int c = 0, int p = 0) : count(c), pollen(p) { }
    };

    std::string getInsectData(Position pos, const std::shared_ptr<Insect> insect);
    std::string getAllInsectData(Cell*** map, int width, int height);

    std::string pollenMap;
    std::string initialInsects;
    std::vector<std::string> turns;

    // Per turn related variables.
    std::vector<std::string> n;
    std::vector<std::string> t;
    std::map<int, Update> u;
};

#endif //CODEBEE_GAME_REPLAY_H
