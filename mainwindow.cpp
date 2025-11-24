#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "main_joc.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , gameWindow(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_playButton_clicked()
{
    if (!gameWindow) {
        gameWindow = new main_joc();
        // conectam semnalul din joc la slotul din MainWindow
        connect(gameWindow, &main_joc::backToMenuRequested,
                this, &MainWindow::showMenuAgain);
    }

    gameWindow->show();
    this->hide();
}

void MainWindow::on_IesireButton_clicked()
{
    close();
}

void MainWindow::showMenuAgain()
{
    // reapare meniul, inchidem fereastra de joc
    this->show();
    if (gameWindow) {
        gameWindow->hide();    // sau gameWindow->close();
        // daca vrei sa refaci fereastra la fiecare joc, poti chiar s-o stergi:
        // gameWindow->deleteLater();
        // gameWindow = nullptr;
    }
}
