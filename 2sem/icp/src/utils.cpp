// Author: Tomas Brablec
// Project: pacppman (ICP project)
// License: GPL 3.0 (https://www.gnu.org/licenses/gpl-3.0.txt)
// Module: Utils
// Description: This module contains various utility functions, mainly
// so-called boilerplate code.

#include "utils.h"

/// Converts point in map to pixel coordinates.
QPointF asQPointF(Point p) {
    return QPointF(p.x * TEXTURE_SIZE, p.y * TEXTURE_SIZE);
}

/// Animates movement of texture using starting point
/// and Direction.
void animate(QObject *texture, Point start, Direction direction) {
    QPropertyAnimation *animation = new QPropertyAnimation;
    animation->setTargetObject(texture);
    animation->setPropertyName("pos");
    animation->setDuration(TICK_TIME);
    animation->setStartValue(asQPointF(start));
    animation->setEndValue(asQPointF(start.move(direction)));
    animation->start();
}

/// Converts movement character to Qt::Key* type.
int asQKey(char c) {
    if (c == 'U')
        return Qt::Key_W;
    if (c == 'L')
        return Qt::Key_A;
    if (c == 'R')
        return Qt::Key_D;
    if (c == 'D')
        return Qt::Key_S;
    return 0;
}

bool Point::operator==(Point other) { return x == other.x && y == other.y; }

/// Returns copy of Point moved according to Direction.
Point Point::move(Direction direction) {
    Point p = *this;
    if (direction == Direction::Up)
        p.y--;
    if (direction == Direction::Down)
        p.y++;
    if (direction == Direction::Left)
        p.x--;
    if (direction == Direction::Right)
        p.x++;
    return p;
}

/// Rotates pixmap according to Direction.
void Pixmap::rotate(Direction direction) {
    setTransformOriginPoint(boundingRect().center());
    if (direction == Direction::Left)
        this->setRotation(180);
    if (direction == Direction::Right)
        this->setRotation(0);
    if (direction == Direction::Up)
        this->setRotation(270);
    if (direction == Direction::Down)
        this->setRotation(90);
}

/// Converts pixel coordinates to point in map.
Point asPoint(QPointF position) {
    return Point{static_cast<size_t>(position.x() / TEXTURE_SIZE),
                 static_cast<size_t>(position.y() / TEXTURE_SIZE)};
}

/// Callback for receiving mouse clicks.
void CustomScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    movement->type = MovementType::Mouse;
    movement->targetPoint = asPoint(event->scenePos());
}

/// Converts Direction to movement character, used for recording.
char asLetter(Direction direction) {
    if (direction == Direction::Up)
        return 'U';
    if (direction == Direction::Down)
        return 'D';
    if (direction == Direction::Left)
        return 'L';
    if (direction == Direction::Right)
        return 'R';
    return 'N';
}

/// Returns Direction opposite to input.
Direction oppositeDir(Direction direction) {
    if (direction == Direction::Down)
        return Direction::Up;
    if (direction == Direction::Up)
        return Direction::Down;
    if (direction == Direction::Left)
        return Direction::Right;
    if (direction == Direction::Right)
        return Direction::Left;
    return Direction::None;
}
