// Author: Tomas Brablec
// Project: pacppman (ICP project)
// License: GPL 3.0 (https://www.gnu.org/licenses/gpl-3.0.txt)
// Module: Game
// Description: This module contatins declaration of the Game class.
#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>
#include <cstddef>
#include <iostream>
#include <qgraphicsitem.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <queue>
#include <qwidget.h>
#include <string>
#include <utility>
#include <vector>

#include "game_map.h"
#include "utils.h"

using std::vector;

/// Main class representing a single game.
class Game : public QObject {
        Q_OBJECT

        GameMap map;
        CustomScene *graphicsScene;
        class Player player;
        vector<class Ghost> ghosts;
        Point target;
        void renderMap();
        void setPlayerDirection();
        void initPlayer();
        void initGhost(Ghost &g);
        void initKey(Key &k);
        QString recording;

    private slots:
        void update();

    public:
        QTimer *gameTick;
        Game(CustomScene *scene, QString map);
        void play();
        QString getRecording();
        std::pair<size_t, size_t> mapSize();
        vector<Key> keys;
        unsigned steps;
        PlayerMovement playerMovement;

    public slots:
        void step();

    signals:
        void gameEnd(Result result);
};

#endif // GAME_STATE_H
