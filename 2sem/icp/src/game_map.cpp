// Author: Tomas Brablec
// Project: pacppman (ICP project)
// License: GPL 3.0 (https://www.gnu.org/licenses/gpl-3.0.txt)
// Module: GameMap
// Description: This module contatins implementation of the GameMap class.

#include "game_map.h"
#include "utils.h"
#include <sstream>

using std::queue, std::pair, std::vector;

/// Constructs the GameMap Object from the text representation
/// given in assignment. The method adds border of BlockType::Wall.
GameMap::GameMap(QString mapData) {
    // using stringstream because it has better API for
    // getting data out of it (operator>> and getline)
    std::stringstream file{mapData.toStdString()};
    std::string line;

    file >> y;
    file >> x;
    std::getline(file, line);

    map.clear();
    map.push_back(vector<BlockType>(x + 2, BlockType::Wall));

    for (unsigned i = 1; i < y + 1; i++) {
        map.push_back(vector<BlockType>{});
        std::getline(file, line);

        map.at(i).push_back(BlockType::Wall);
        for (char c : line) {
            if (c == '.')
                map.at(i).push_back(BlockType::Air);
            else if (c == 'T')
                map.at(i).push_back(BlockType::Target);
            else if (c == 'X')
                map.at(i).push_back(BlockType::Wall);
            else if (c == 'G')
                map.at(i).push_back(BlockType::Ghost);
            else if (c == 'S')
                map.at(i).push_back(BlockType::Start);
            else if (c == 'K')
                map.at(i).push_back(BlockType::Key);
            else
                std::cerr << "Unexpected block in map: " << c << "\n";
        }
        map.at(i).push_back(BlockType::Wall);
    }
    map.push_back(vector<BlockType>(x + 2, BlockType::Wall));

    // adding the border width
    x += 2;
    y += 2;

    std::cerr << "Loaded map with " << map.size() << " rows and "
              << map.at(0).size() << " columns.\n";
}

/// Calculates a reasonably short path from start to target,
/// and returns the first step of the path as Direction.
/// The algorithm used is breadth-first search.
Direction GameMap::bestDirection(Point start, Point target) {
    GameMap map = *this;
    if (start == target)
        return Direction::None;

    // this stores pairs (point_currently_evaluated, initial_direction)
    queue<pair<Point, Direction>> queue;

    // pushing initial valid points to evaluate
    if (map.getBlock(start.move(Direction::Left)) != BlockType::Wall)
        queue.push({start.move(Direction::Left), Direction::Left});
    if (map.getBlock(start.move(Direction::Right)) != BlockType::Wall)
        queue.push({start.move(Direction::Right), Direction::Right});
    if (map.getBlock(start.move(Direction::Up)) != BlockType::Wall)
        queue.push({start.move(Direction::Up), Direction::Up});
    if (map.getBlock(start.move(Direction::Down)) != BlockType::Wall)
        queue.push({start.move(Direction::Down), Direction::Down});

    while (!queue.empty()) {
        auto next = queue.front();
        auto nextPoint = next.first;

        // found the target.pos
        if (nextPoint == target) {
            return next.second;
        }

        if (map.getBlock(nextPoint) != BlockType::Wall) {
            map.pushValidDirs(queue, nextPoint, next.second);

            // we use Wall to indicate already "stepped-on" blocks
            map.map.at(nextPoint.y).at(nextPoint.x) = BlockType::Wall;
        }

        queue.pop();
    }

    return Direction::None;
}

/// returns the BlockType of block at position given by Point.
BlockType GameMap::getBlock(Point p) { return map.at(p.y).at(p.x); }

/// Pushes all valid (not BlockType::Wall) pairs of (new_point,
/// initial_direction) onto the queue, used in GameMap::bestDirection
void GameMap::pushValidDirs(std::queue<std::pair<Point, Direction>> &queue,
                            Point point, Direction origDir) {
    if (getBlock(point.move(Direction::Left)) != BlockType::Wall)
        queue.push({point.move(Direction::Left), origDir});
    if (getBlock(point.move(Direction::Right)) != BlockType::Wall)
        queue.push({point.move(Direction::Right), origDir});
    if (getBlock(point.move(Direction::Down)) != BlockType::Wall)
        queue.push({point.move(Direction::Down), origDir});
    if (getBlock(point.move(Direction::Up)) != BlockType::Wall)
        queue.push({point.move(Direction::Up), origDir});
}
