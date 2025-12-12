#include "main_joc.h"
#include <QFile>
#include <QTextStream>
#include <QKeyEvent>
#include <QFontDatabase>
#include <ctime>

main_joc::main_joc(QWidget *parent)
    : QWidget(parent), wrongGuesses(0), maxWrong(6), score(0), lives(6), gameOver(false)
{
    setFixedSize(800, 720);
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);
    setWindowFlag(Qt::CustomizeWindowHint);

    // FONT MONOSPACE
    QFont mono("Consolas");
    mono.setStyleHint(QFont::Monospace);
    mono.setFixedPitch(true);
    mono.setPointSize(18);

    if (!QFontDatabase::isFixedPitch(mono.family())) {
        mono = QFontDatabase::systemFont(QFontDatabase::FixedFont);
        mono.setPointSize(16);
    }

    // Spanzuratoare: fiecare linie un QLabel
    hangmanLayout = new QVBoxLayout();
    hangmanLayout->setAlignment(Qt::AlignLeft);
    for (int i = 0; i < 7; ++i) {
        QLabel *line = new QLabel(this);
        line->setFont(mono);
        line->setText(" "); // gol initial
        line->setAlignment(Qt::AlignLeft);
        hangmanLines.append(line);
        hangmanLayout->addWidget(line);
    }

    // Info
    wordLabel  = new QLabel(this);
    wordLabel->setFont(mono);
    wordLabel->setTextFormat(Qt::PlainText);
    wordLabel->setAlignment(Qt::AlignCenter);

    usedLabel  = new QLabel(this);
    scoreLabel = new QLabel(this);
    livesLabel = new QLabel(this);

    // Tastatura
    lettersLayout = new QGridLayout();
    QWidget *keyboardWidget = new QWidget(this);
    keyboardWidget->setLayout(lettersLayout);

    QString alphabet = "abcdefghijklmnopqrstuvwxyz";
    int row = 0, col = 0;
    for (QChar letter : alphabet) {
        QPushButton *button = new QPushButton(QString(letter), this);
        button->setFixedSize(40, 40);
        lettersLayout->addWidget(button, row, col);
        connect(button, &QPushButton::clicked, this, &main_joc::letterClicked);
        col++;
        if (col == 9) { col = 0; row++; }
    }

    // Butoane
    restartButton = new QPushButton("Restart", this);
    connect(restartButton, &QPushButton::clicked, this, &main_joc::restartGame);

    menuButton = new QPushButton("Meniu principal", this);
    menuButton->setEnabled(false);
    connect(menuButton, &QPushButton::clicked, this, &main_joc::backToMenu);

    // Layout principal
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(hangmanLayout);
    mainLayout->addWidget(wordLabel);

    QHBoxLayout *infoLayout = new QHBoxLayout();
    infoLayout->addWidget(usedLabel);
    infoLayout->addWidget(scoreLabel);
    infoLayout->addWidget(livesLabel);
    mainLayout->addLayout(infoLayout);

    mainLayout->addWidget(keyboardWidget);

    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(restartButton);
    bottomLayout->addWidget(menuButton);
    mainLayout->addLayout(bottomLayout);

    loadWords();
    startGame();
}

main_joc::~main_joc() {}

void main_joc::loadWords() {
    QFile file(":/cuvinte/cuvinte.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString word = in.readLine().trimmed().toLower();
        if(!word.isEmpty())
            wordList.append(word);
    }
}

void main_joc::startGame() {
    srand(static_cast<unsigned int>(time(nullptr)));
    secretWord = wordList.isEmpty() ? "eroare" : wordList[rand() % wordList.size()];
    currentDisplay = QString(secretWord.size(), '_');
    usedLetters.clear();
    wrongGuesses = 0;
    lives = maxWrong;
    score = 0;
    gameOver = false;

    for(int i = 0; i < lettersLayout->count(); i++) {
        QWidget *w = lettersLayout->itemAt(i)->widget();
        if(w) w->setEnabled(true);
    }

    menuButton->setEnabled(false);
    updateDisplay();
}

void main_joc::updateHangmanDisplay() {
    QString states[7][7] = {
        { "  -----", "  |   |", "      |", "      |", "      |", "      |", "---------" }, // 0
        { "  -----", "  |   |", "  O   |", "      |", "      |", "      |", "---------" }, // 1
        { "  -----", "  |   |", "  O   |", "  |   |", "      |", "      |", "---------" }, // 2
        { "  -----", "  |   |", "  O   |", " /|   |", "      |", "      |", "---------" }, // 3
        { "  -----", "  |   |", "  O   |", " /|\\  |", "      |", "      |", "---------" }, // 4
        { "  -----", "  |   |", "  O   |", " /|\\  |", " /    |", "      |", "---------" }, // 5
        { "  -----", "  |   |", "  O   |", " /|\\  |", " / \\  |", "      |", "---------" }  // 6
    };

    for (int i = 0; i < hangmanLines.size(); ++i) {
        hangmanLines[i]->setText(states[wrongGuesses][i]);

    }
}

void main_joc::updateDisplay() {
    updateHangmanDisplay();

    QString displayWord;
    for (const QChar &c : currentDisplay)
        displayWord += QString(c) + " ";
    wordLabel->setText(displayWord.trimmed());

    QString used;
    for (const QChar &c : usedLetters)
        used += QString(c) + " ";
    usedLabel->setText("Folosite: " + used.trimmed());

    scoreLabel->setText("Scor: " + QString::number(score));
    livesLabel->setText("Vieti: " + QString::number(lives));

    if(gameOver) {
        wordLabel->setText(secretWord.toUpper());
        menuButton->setEnabled(true);
    }
}

void main_joc::handleGuess(QChar letter) {
    if(gameOver || !letter.isLetter()) return;

    letter = letter.toLower();
    if(usedLetters.contains(letter)) return;

    usedLetters.insert(letter);

    bool correctGuess = false;
    for(int i = 0; i < secretWord.size(); i++) {
        if(secretWord[i] == letter) {
            currentDisplay[i] = letter;
            correctGuess = true;
        }
    }

    if(correctGuess)
        score += 10;
    else {
        wrongGuesses++;
        lives--;
    }

    if(currentDisplay == secretWord || wrongGuesses >= maxWrong || lives <= 0) {
        gameOver = true;
        currentDisplay = secretWord;
    }

    updateDisplay();

    if(gameOver) {
        for(int i = 0; i < lettersLayout->count(); i++) {
            QWidget *w = lettersLayout->itemAt(i)->widget();
            if(w) w->setEnabled(false);
        }
    }
}

void main_joc::keyPressEvent(QKeyEvent *event) {
    if(event && !event->text().isEmpty())
        handleGuess(event->text().at(0));
}

void main_joc::letterClicked() {
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    if(!button) return;
    button->setEnabled(false);
    handleGuess(button->text().at(0));
}

void main_joc::backToMenu() {
    emit backToMenuRequested();
}

void main_joc::restartGame() {
    startGame();
}
