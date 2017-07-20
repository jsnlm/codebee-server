#ifndef CODEBEE_GAME_MAP_H
#define CODEBEE_GAME_MAP_H

#include <string>

#include "cell.h"
#include "position.h"
#include "move.h"

class Map {
public:
    // TODO: move path to its own file/class
    struct Path {
        int distance;
        Move move;

        Path(int d = 0, Move m = move::UP) : distance(d), move(m) { }
    };


    Map();
    Map(std::string pollenMap);
    ~Map();
    void updateMap(std::string newInsects);
    Path getPath(Position a, Position b) const;

    int height;
    int width;
    Cell*** map;
};

#endif //CODEBEE_GAME_MAP_H
