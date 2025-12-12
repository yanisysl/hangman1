#ifndef MAIN_JOC_H
#define MAIN_JOC_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QSet>
#include <QStringList>

class main_joc : public QWidget
{
    Q_OBJECT

public:
    explicit main_joc(QWidget *parent = nullptr);
    ~main_joc();

signals:
    void backToMenuRequested();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void letterClicked();
    void restartGame();
    void backToMenu();

private:
    // UI general
    QLabel *wordLabel;
    QLabel *usedLabel;
    QLabel *scoreLabel;
    QLabel *livesLabel;
    QPushButton *restartButton;
    QPushButton *menuButton;
    QGridLayout *lettersLayout;

    // Spanzuratoare
    QVBoxLayout *hangmanLayout;   // layout vertical pentru spânzurătoare
    QLabel *poleLabel;            // stâlp vertical
    QLabel *topLabel;             // bara orizontală
    QLabel *ropeLabel;            // sfoară
    QLabel *headLabel;            // cap
    QLabel *bodyLabel;            // corp
    QLabel *leftArmLabel;         // braț stâng
    QLabel *rightArmLabel;        // braț drept
    QLabel *leftLegLabel;         // picior stâng
    QLabel *rightLegLabel;        // picior drept
    QLabel *baseLabel;            // baza platformei (optional)
    // Adaugă în secțiunea private UI:
    QList<QLabel*> hangmanLines;  // lista de QLabel pentru fiecare linie a spânzurătorii
       // layout-ul vertical care conține liniile


    // logica jocului
    QStringList wordList;
    QString secretWord;
    QString currentDisplay;
    QSet<QChar> usedLetters;
    int wrongGuesses;
    int maxWrong;
    int score;
    int lives;
    bool gameOver;

    // metode
    void loadWords();
    void startGame();
    void updateHangmanDisplay(); // actualizează vizual spânzurătoarea
    void updateDisplay();        // actualizează tot ecranul (cuvânt, scor, vieți)
    void handleGuess(QChar letter);
};

#endif // MAIN_JOC_H
