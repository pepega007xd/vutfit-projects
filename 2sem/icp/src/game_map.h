// Author: Tomas Brablec
// Project: pacppman (ICP project)
// License: GPL 3.0 (https://www.gnu.org/licenses/gpl-3.0.txt)
// Module: GameMap
// Description: This module contatins declaration of the GameMap class.

#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "utils.h"

/// Represents the game map in the initial state, as it
/// is loaded from file. The internal data structure is a nested 2D vector
/// of BlockType.
class GameMap {
    public:
        size_t x;
        size_t y;
        std::vector<std::vector<BlockType>> map;
        GameMap(QString mapFile);
        void pushValidDirs(std::queue<std::pair<Point, Direction>> &queue,
                           Point point, Direction origDir);
        BlockType getBlock(Point p);
        Direction bestDirection(Point start, Point target);
};

#endif // GAME_MAP_H
