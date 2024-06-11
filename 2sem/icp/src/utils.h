// Author: Tomas Brablec
// Project: pacppman (ICP project)
// License: GPL 3.0 (https://www.gnu.org/licenses/gpl-3.0.txt)
// Module: Utils
// Description: This module contains declaration of utility functions and types.

#ifndef UTILS_H
#define UTILS_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <QPropertyAnimation>
#include <cstddef>
#include <iostream>
#include <qgraphicsitem.h>
#include <queue>
#include <qwidget.h>
#include <string>
#include <utility>
#include <vector>

#define TEXTURE_SIZE 25
#define TICK_TIME 300

enum class gameMode { None, Playing, Replaying };
enum class BlockType { Air, Wall, Ghost, Player, Key, Target, Start };
enum class Direction { None, Left, Right, Up, Down };
enum class Result { Won, Lost };

/// Struct representing a tile position in the GameMap. Point corresponds to
/// the characters in map file, not the pixel coordinates on screen.
struct Point {
        std::size_t x, y;
        Point move(Direction d);
        bool operator==(Point other);
};

/// Custom class extending QGraphicsPixmapItem that can be animated (inherits
/// necessary code from QObject). It is used to display tiles in
/// GameState::scene.
class Pixmap : public QObject, public QGraphicsPixmapItem {
        Q_OBJECT
        Q_PROPERTY(QPointF pos READ pos WRITE setPos);

    public:
        void rotate(Direction dir);
};

/// Class representing the player (pacman) in GameState.
struct Player {
        Point position;
        Pixmap *texture;
        Direction dir;
};

/// Class representing a single ghost in GameState.
struct Ghost {
        Point position;
        QGraphicsPixmapItem *texture;
        Direction dir;
};

/// Class representing a single key in GameState.
struct Key {
        Point position;
        QGraphicsPixmapItem *texture;
};

/// Represents the type of movenent input currently used.
enum class MovementType { Keyboard, Mouse };

/// Struct that represents current movement command from
/// user, has two variants (keyboard and mouse control) given by MovementType.
struct PlayerMovement {
        MovementType type;
        int key;
        Point targetPoint;
};

/// Custom class that inherits from QGraphicsScene, which allows for
/// intercepting mouse clicks with their exact position.
struct CustomScene : public QGraphicsScene {
        PlayerMovement *movement;
        CustomScene(QObject *parent = nullptr) : QGraphicsScene(parent) {}
        void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

QPointF asQPointF(Point p);
Point asPoint(QPointF position);
int asQKey(char c);
char asLetter(Direction direction);

void animate(QObject *texture, Point position, Direction direction);

Direction oppositeDir(Direction direction);

#endif // UTILS_H
