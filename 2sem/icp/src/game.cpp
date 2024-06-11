// Author: Tomas Brablec
// Project: pacppman (ICP project)
// License: GPL 3.0 (https://www.gnu.org/licenses/gpl-3.0.txt)
// Module: Game
// Description: This module contatins implementation of the Game class.

#include "game.h"
#include "utils.h"

#include <QFile>
#include <QGraphicsItem>
#include <QGraphicsItemAnimation>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QTimer>
#include <cstddef>
#include <exception>
#include <fstream>
#include <iostream>
#include <ostream>
#include <qevent.h>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qobjectdefs.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <queue>
#include <qwidget.h>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using std::pair;

Game::Game(CustomScene *scene, QString mapFile) : map(mapFile) {
    this->graphicsScene = scene;
    recording.push_back(mapFile);
    renderMap();
    steps = 0;
    this->graphicsScene->movement = &playerMovement;
}

/// Creates the textures of all tiles from the initial map state and
/// renders them into scene, including the player's and ghosts' textures.
void Game::renderMap() {
    for (unsigned y = 0; y < map.y; y++) {
        for (unsigned x = 0; x < map.x; x++) {
            QPixmap texture{":air.png"};
            QGraphicsPixmapItem *textureItem = new QGraphicsPixmapItem{};

            BlockType block = map.getBlock({x, y});
            if (block == BlockType::Wall)
                texture = {":wall.png"};
            if (block == BlockType::Ghost)
                ghosts.push_back(Ghost{x, y});
            if (block == BlockType::Key)
                keys.push_back(Key{x, y});
            if (block == BlockType::Start)
                player.position = {x, y};
            if (block == BlockType::Target) {
                target = {x, y};
                texture = {":target.png"};
            }

            textureItem->setPixmap(texture);

            textureItem->setPos(asQPointF({x, y}));
            graphicsScene->addItem(textureItem);
        }
    }

    // the order of initializing the textures determines the
    // "height" of the texture
    for (auto &k : keys)
        initKey(k);
    initPlayer();
    for (auto &g : ghosts)
        initGhost(g);
}

/// Creates the player texture and renders it into the graphicsScene
void Game::initPlayer() {
    player.dir = Direction::None;

    QPixmap texture{":pacman.png"};
    player.texture = new Pixmap;
    player.texture->setPixmap(texture);
    player.texture->setPos(asQPointF(player.position));
    graphicsScene->addItem(player.texture);
}

/// Creates the ghost texture and renders it into the graphicsScene
void Game::initGhost(Ghost &g) {
    g.dir = Direction::None;

    QPixmap texture{":ghost.png"};
    g.texture = new Pixmap;
    g.texture->setPixmap(texture);
    g.texture->setPos(asQPointF(g.position));
    graphicsScene->addItem(g.texture);
}

/// Creates the key texture and renders it into the graphicsScene
void Game::initKey(Key &k) {

    QPixmap texture{":key.png"};
    k.texture = new Pixmap;
    k.texture->setPixmap(texture);
    k.texture->setPos(asQPointF(k.position));
    graphicsScene->addItem(k.texture);
}

/// Creates tick timer and connects it to the update() method.
void Game::play() {
    gameTick = new QTimer(this);
    connect(gameTick, SIGNAL(timeout()), this, SLOT(update()));
    gameTick->start(TICK_TIME);
}

/// Updates the player's and ghosts' positions, schedules animations
/// for their movement, and checks collisions of all relevant objects.
void Game::update() {
    step();

    // checking for target collision
    if (player.position == target && keys.empty()) {
        gameEnd(Result::Won);
        gameTick->stop();
        return;
    }

    // collision detection
    for (Ghost g : ghosts) {
        if (g.position == player.position ||
            (g.position.move(player.dir) == player.position &&
             g.dir == oppositeDir(player.dir)) // player and ghost "swap places"
        ) {
            gameEnd(Result::Lost);
            gameTick->stop();
            return;
        }
    }
}

/// Updates the positions of ghosts and player using the
/// GameMap::bestDirection() method, or from keyboard input.
void Game::step() {
    // checking for key collision
    for (unsigned i = 0; i < keys.size(); i++) {
        if (player.position == keys.at(i).position) {
            graphicsScene->removeItem(keys.at(i).texture);
            delete keys.at(i).texture;
            keys.erase(keys.begin() + i);
        }
    }

    // ghosts movement
    for (auto &g : ghosts) {
        g.dir = map.bestDirection(g.position, player.position);
        animate(dynamic_cast<QObject *>(g.texture), g.position, g.dir);
        g.position = g.position.move(g.dir);
    }

    // player movement
    setPlayerDirection();
    animate(dynamic_cast<QObject *>(player.texture), player.position,
            player.dir);
    player.position = player.position.move(player.dir);
}

/// Updates the player's direction. If the movement type is keyboard, the method
/// simply checks for wall collisions. If the movement type is mouse, it
/// calculates next step using the GameMap::bestDirection() method.
void Game::setPlayerDirection() {
    Direction nextDir;

    if (playerMovement.type == MovementType::Keyboard) {
        if (playerMovement.key == Qt::Key_W)
            nextDir = Direction::Up;
        else if (playerMovement.key == Qt::Key_S)
            nextDir = Direction::Down;
        else if (playerMovement.key == Qt::Key_A)
            nextDir = Direction::Left;
        else if (playerMovement.key == Qt::Key_D)
            nextDir = Direction::Right;
        else
            nextDir = Direction::None;
    } else {
        nextDir =
            map.bestDirection(player.position, playerMovement.targetPoint);
    }

    // wall check - we try to preserve direction if direction
    // change cannot happen
    if (map.getBlock(player.position.move(nextDir)) == BlockType::Wall)
        nextDir = player.dir;

    // second wall check - we stop if there is still no space
    // in front of us
    if (map.getBlock(player.position.move(nextDir)) == BlockType::Wall)
        nextDir = Direction::None;

    player.dir = nextDir;

    recording.push_back(asLetter(player.dir));
    if (player.dir != Direction::None) {
        player.texture->rotate(player.dir);
        steps++;
    }
}

/// Returns the recording of current game as QString.
QString Game::getRecording() { return recording; }

/// Returns the current size of game map.
std::pair<size_t, size_t> Game::mapSize() { return std::pair{map.x, map.y}; }
