#ifndef MAIN_JOC_H
#define MAIN_JOC_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QSet>
#include <QStringList>

class main_joc : public QWidget
{
    Q_OBJECT

public:
    explicit main_joc(QWidget *parent = nullptr);
    ~main_joc();

signals:
    void backToMenuRequested();   // <-- AICI e signal-ul pentru MainWindow

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void letterClicked();
    void restartGame();
    void backToMenu();            // <-- slot pentru butonul „Meniu principal”

private:
    // UI
    QLabel *hangmanLabel;
    QLabel *wordLabel;
    QLabel *usedLabel;
    QLabel *scoreLabel;
    QLabel *livesLabel;
    QPushButton *restartButton;
    QPushButton *menuButton;
    QGridLayout *lettersLayout;

    // Logica jocului
    QStringList wordList;
    QString secretWord;
    QString currentDisplay;
    QSet<QChar> usedLetters;
    int wrongGuesses;
    int maxWrong;
    int score;
    int lives;
    bool gameOver;

    void loadWords();
    void startGame();
    void updateDisplay();
    void handleGuess(QChar key);
};

#endif // MAIN_JOC_H
