// Author: Tomas Brablec
// Project: pacppman (ICP project)
// License: GPL 3.0 (https://www.gnu.org/licenses/gpl-3.0.txt)
// Module: MainWindow
// Description: This module contains the implementation of the MainWindow class.

#include "mainwindow.h"
#include "game.h"

#include <QApplication>
#include <QFileDialog>
#include <QKeySequence>
#include <QMenuBar>
#include <QMouseEvent>
#include <QTextStream>
#include <QTimer>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <qnamespace.h>
#include <string>
#include <thread>

#include "ui_mainwindow.h"
#include "utils.h"

/// Main function of the program, instantiates the MainWindow object,
/// and launches the Qt runtime.
int main(int argc, char **argv) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.show();

    app.exec();

    return 0;
}

/// Constructs the MainWindow object, sets the default map (in QRC).
MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    currentStep = 0;
    mode = gameMode::None;
    filename = ":default.map";
    loadMap();
}

MainWindow::~MainWindow() { delete ui; }

/// Sets the game mode to gameMode::Playing and starts the game.
/// Registers callbacks for updating the UI elements and for updating
/// result.
void MainWindow::startGame() {
    if (mode != gameMode::None)
        return;

    mode = gameMode::Playing;
    game->play();
    connect(game->gameTick, SIGNAL(timeout()), this, SLOT(updateUiElements()));
    connect(game, SIGNAL(gameEnd(Result)), this, SLOT(gameEnd(Result)));
    startTime = std::chrono::high_resolution_clock::now();
}

/// Reloads the map from file, and starts the game again.
void MainWindow::restartGame() {
    if (mode == gameMode::Replaying)
        return;

    delete game;
    delete graphicsScene;
    loadMap();
    startGame();
}

/// Invokes file picker, and loads map from selected file.
void MainWindow::openMap() {
    mode = gameMode::None;

    filename = QFileDialog::getOpenFileName(this, "Open Map");

    delete game;
    delete graphicsScene;
    loadMap();
}

/// Invokes file picker, and loads game recording from selected file.
void MainWindow::openRecording() {
    mode = gameMode::None;

    filename = QFileDialog::getOpenFileName(this, "Open Recording");
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        recording = in.readAll();
        file.close();
    }

    int lastLineIdx = recording.lastIndexOf('\n');
    if (lastLineIdx < 0) {
        std::cerr << "Invalid recording file\n";
        return;
    }
    directions = recording.mid(lastLineIdx + 1);

    delete game;
    delete graphicsScene;
    loadMap();
    mode = gameMode::Replaying;
    startTime = std::chrono::high_resolution_clock::now();
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(stepRecording()));
}

/// Invokes file savind dialog, and saves game recording to the specified file.
void MainWindow::saveRecording() {
    if (mode != gameMode::None)
        return;

    filename = QFileDialog::getSaveFileName(this, "Save File");
    QFile file(filename);

    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << game->getRecording();
        file.close();
    } else {
        return;
    }
}

/// Performs a single step in the recording, by calling the Game::update()
/// method manually.
void MainWindow::stepRecording() {
    if (mode != gameMode::Replaying)
        return;

    if (currentStep >= directions.size()) {
        return;
    }
    game->playerMovement.type = MovementType::Keyboard;
    game->playerMovement.key = asQKey(directions.at(currentStep).toLatin1());
    currentStep++;
    game->step();
    updateUiElements();
}

/// Moves recording one step back. This is done by reloading the recording
/// from file, and calling stepRecording() (step - 1) times.
void MainWindow::stepBackRecording() {
    if (mode != gameMode::Replaying)
        return;

    if (currentStep == 0)
        return;

    unsigned prevStep = currentStep - 1;
    currentStep = 0;

    delete game;
    delete graphicsScene;
    mode = gameMode::None;
    loadMap();
    mode = gameMode::Replaying;

    for (unsigned i = 0; i < prevStep; i++) {
        stepRecording();
    }
}

/// Plays recording at its native speed by enabling timer with callback set
/// to stepRecording().
void MainWindow::playRecording() {
    if (mode != gameMode::Replaying)
        return;

    timer->start(TICK_TIME);
}

/// Pauses playing recording by stopping the timer.
void MainWindow::pauseRecording() {
    if (mode != gameMode::Replaying)
        return;

    timer->stop();
}

/// Loads map from file specified in the finename member variable.
/// The method also instantiates the graphicsScene and game objects.
void MainWindow::loadMap() {
    if (mode != gameMode::None)
        return;

    QString mapData;
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        mapData = in.readAll();
        file.close();
    } else {
        return;
    }

    graphicsScene = new CustomScene();
    ui->graphicsView->setScene(graphicsScene);
    auto callback = [&](QGraphicsSceneMouseEvent *event) {};

    game = new Game(graphicsScene, mapData);
    auto x = game->mapSize();
    ui->graphicsView->resize(x.first * TEXTURE_SIZE, x.second * TEXTURE_SIZE);
    resize(x.first * TEXTURE_SIZE + 60, x.second * TEXTURE_SIZE + 120);
}

/// Sends received key presses to game.
void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (mode != gameMode::Playing)
        return;

    game->playerMovement.type = MovementType::Keyboard;
    game->playerMovement.key = event->key();
}

/// Redraws the interactive UI elements.
void MainWindow::updateUiElements() {
    QString labelText = "<b style=\" color : orange;\"> %1 </b> steps taken";
    ui->stepsLabel->setText(labelText.arg(game->steps));

    if (mode == gameMode::Playing) {
        labelText = "Status: <b style=\"color: orange\"> PLAYING </b>";
    } else if (mode == gameMode::Replaying) {
        labelText = "status: <b style=\"color: cyan\"> REPLAY </b>";
    } else if (result == Result::Lost) {
        labelText = "Status: <b style=\"color: red\"> YOU LOST </b>";
    } else {
        labelText = "Status: <b style=\"color: lime\"> YOU WON </b>";
    }
    ui->statusLabel->setText(labelText);

    labelText = "%1:%2";
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - startTime);
    unsigned minutes = elapsed.count() / 60;
    unsigned seconds = elapsed.count() % 60;
    ui->timeLabel->setText(
        labelText.arg(minutes).arg(seconds, 2, 10, QLatin1Char('0')));

    labelText = "<b style=\" color : orange;\"> %1 </b> keys left";
    ui->keysLabel->setText(labelText.arg(game->keys.size()));
}

/// Callback for setting the mode when the game ends.
void MainWindow::gameEnd(Result result) {
    mode = gameMode::None;
    this->result = result;
    updateUiElements();
}

/// Exits the application.
void MainWindow::quit() { exit(0); }
