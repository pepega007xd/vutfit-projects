// Author: Tomas Brablec
// Project: pacppman (ICP project)
// License: GPL 3.0 (https://www.gnu.org/licenses/gpl-3.0.txt)
// Module: MainWindow
// Description: This module contains delaration of the MainWindow class.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "game.h"
#include <QGraphicsScene>
#include <QMainWindow>
#include <string>

class QAction;
class QFileDialog;

namespace Ui {
/// Automatically generated UI class.
class MainWindow;
} // namespace Ui

/// Class of the main game window, handles UI interactions and initialization
/// of the Game object. The class stores its current mode in the `mode` member,
/// which determines whether many of its UI callbacks are functional.
class MainWindow : public QMainWindow {
        Q_OBJECT
    public:
        MainWindow(QWidget *parent = nullptr,
                   Qt::WindowFlags flags = Qt::WindowFlags());
        MainWindow(MainWindow const &) = delete;

        ~MainWindow() override;
        void keyPressEvent(QKeyEvent *event) override;

    private slots:
        void openMap();
        void openRecording();
        void saveRecording();
        void stepRecording();
        void stepBackRecording();
        void playRecording();
        void pauseRecording();
        void updateUiElements();
        void startGame();
        void restartGame();
        void quit();

        // Internal state.
    private:
        CustomScene *graphicsScene;
        Game *game;
        void loadMap();
        /// Valid modes are "None", "Playing", and "Replaying"
        gameMode mode;
        /// Directions to feed into game when replaying.
        QString directions;
        /// Stores the current step of replay.
        unsigned currentStep;
        /// Used for continuous replaying of recodings.
        QTimer *timer;
        /// Content of the loaded recording file.
        QString recording;
        /// Used for storing file name of currently loaded game/recording.
        QString filename;
        /// For showing elapsed time in a game.
        std::chrono::system_clock::time_point startTime;
        /// Stores result of last game, valid results are "Won" and "Lost"
        Result result;
        /// Pointer to the generated UI class.
        Ui::MainWindow *ui;

    public slots:
        void gameEnd(Result r);
};

#endif // MAINWINDOW_H
