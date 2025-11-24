#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "main_joc.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_playButton_clicked();
    void on_IesireButton_clicked();

    void showMenuAgain();   // nou – slot pentru revenire din joc

private:
    Ui::MainWindow *ui;
    main_joc *gameWindow;
};

#endif // MAINWINDOW_H
