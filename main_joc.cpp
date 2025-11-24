#include "main_joc.h"

#include <QFile>
#include <QTextStream>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>
#include <ctime>
#include <cstdlib>

// constructorul clasei main_joc
main_joc::main_joc(QWidget *parent) :
    QWidget(parent),
    wrongGuesses(0),   // numarul de greseli la inceput este 0
    maxWrong(11),      // numarul maxim de greseli permise
    score(0),          // scorul incepe de la 0
    lives(11),         // numarul de vieti la inceput
    gameOver(false)    // jocul nu e terminat la inceput
{
    // setez culoarea de fundal a ferestrei de joc
    setStyleSheet("background-color: #fafafa;");

    // label pentru desenul spanzuratorii (desen ascii)
    hangmanLabel = new QLabel(this);
    hangmanLabel->setAlignment(Qt::AlignCenter); // pun textul la centru
    hangmanLabel->setStyleSheet(
        "font-family: monospace; font-size: 22px; color: #e53935;"
        );

    // label pentru cuvantul curent (_ _ A _ etc.)
    wordLabel = new QLabel(this);
    wordLabel->setAlignment(Qt::AlignCenter);
    wordLabel->setStyleSheet(
        "font-size: 40px; font-weight: bold; color: #1e88e5;"
        );

    // label pentru literele folosite
    usedLabel = new QLabel(this);
    usedLabel->setAlignment(Qt::AlignCenter);
    usedLabel->setStyleSheet(
        "font-size: 18px; color: #ff8f00;"
        );

    // label pentru scor
    scoreLabel = new QLabel(this);
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setStyleSheet(
        "font-size: 18px; color: #43a047;"
        );

    // label pentru vieti
    livesLabel = new QLabel(this);
    livesLabel->setAlignment(Qt::AlignCenter);
    livesLabel->setStyleSheet(
        "font-size: 18px; color: #f4511e;"
        );

    // layout pentru "tastatura" cu litere
    lettersLayout = new QGridLayout();
    QWidget *lettersWidget = new QWidget(this);
    lettersWidget->setLayout(lettersLayout);

    // alfabetul pe care il folosesc pentru butoane
    QString alphabet = "abcdefghijklmnopqrstuvwxyz";
    int row = 0;
    int col = 0;

    // creez cate un buton pentru fiecare litera din alfabet
    for (QChar c : alphabet) {
        // fac un buton cu textul literei
        QPushButton *btn = new QPushButton(QString(c), this);
        // ii dau o dimensiune fixa
        btn->setFixedSize(50, 50);

        // stil pentru buton, ca sa arate mai modern
        btn->setStyleSheet(
            "QPushButton {"
            "  background-color: #ffffff;"
            "  border:2px solid #90caf9;"
            "  border-radius:8px;"
            "  font-size:20px;"
            "  color: #0d47a1;"
            "}"
            "QPushButton:disabled {"
            "  background-color: #e3f2fd;"
            "  border:2px solid #bbdefb;"
            "  color: #90a4ae;"
            "}"
            );

        // adaug butonul in grid pe linia row si coloana col
        lettersLayout->addWidget(btn, row, col);

        // cand apas pe buton, se apeleaza functia letterClicked()
        connect(btn, &QPushButton::clicked, this, &main_joc::letterClicked);

        // trec la urmatoarea coloana
        col++;
        // daca am 9 coloane, trec pe linia urmatoare
        if (col == 9) {
            col = 0;
            row++;
        }
    }

    // buton pentru restart de joc
    restartButton = new QPushButton("Restart", this);
    restartButton->setFixedHeight(45);
    restartButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #1e88e5;"
        "  color: white;"
        "  border-radius: 10px;"
        "  font-size: 18px;"
        "  padding: 6px 20px;"
        "}"
        "QPushButton:hover { background-color: #1565c0; }"
        "QPushButton:pressed { background-color: #0d47a1; }"
        );
    // cand apas pe butonul de restart, apelez functia restartGame()
    connect(restartButton, &QPushButton::clicked, this, &main_joc::restartGame);

    // buton pentru a merge inapoi la meniul principal
    menuButton = new QPushButton("Meniu principal", this);
    menuButton->setFixedHeight(45);
    // la inceput acest buton este dezactivat, il activez doar dupa ce se termina jocul
    menuButton->setEnabled(false);
    menuButton->setStyleSheet(
        "QPushButton {"
        "  background-color: #8e24aa;"
        "  color: white;"
        "  border-radius: 10px;"
        "  font-size: 18px;"
        "  padding: 6px 20px;"
        "}"
        "QPushButton:hover { background-color: #6a1b9a; }"
        "QPushButton:pressed { background-color: #4a148c; }"
        );
    // cand apas butonul de meniu, apelez functia backToMenu()
    connect(menuButton, &QPushButton::clicked, this, &main_joc::backToMenu);

    // layout-ul principal
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    // setez margini si spatiu intre elemente
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // adaug desenele si cuvantul
    mainLayout->addWidget(hangmanLabel);
    mainLayout->addWidget(wordLabel);

    // layout orizontal pentru informatii (litere folosite, scor, vieti)
    QHBoxLayout *infoLayout = new QHBoxLayout();
    infoLayout->addWidget(usedLabel);
    infoLayout->addWidget(scoreLabel);
    infoLayout->addWidget(livesLabel);
    mainLayout->addLayout(infoLayout);

    // adaug tastatura sub informatii
    mainLayout->addWidget(lettersWidget);

    // layout orizontal pentru butoanele de jos (restart si meniu)
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(restartButton);
    bottomLayout->addWidget(menuButton);
    mainLayout->addLayout(bottomLayout);

    // setez acest layout ca layout principal pentru fereastra
    setLayout(mainLayout);

    // nu las fereastra sa fie maximizata, o tin la o dimensiune fixa
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);
    setFixedSize(1000, 750);

    // incarc cuvintele din fisierul din resurse
    loadWords();
    // pornesc primul joc
    startGame();
}

// destructor, aici nu trebuie sa fac nimic special
main_joc::~main_joc() {}

// functie care citeste cuvintele din fisierul de resurse
void main_joc::loadWords() {
    // incerc sa deschid fisierul cu cuvinte
    QFile file(":/cuvinte/cuvinte.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        // daca nu se poate deschide, ies din functie
        return;
    }

    QTextStream in(&file); // flux text pentru citire
    while (!in.atEnd()) {
        // citesc o linie
        QString line = in.readLine().toLower().trimmed();
        // daca linia nu e goala, o adaug in lista de cuvinte
        if (!line.isEmpty())
            wordList.append(line);
    }
}

// functie care porneste sau reporneste jocul
void main_joc::startGame() {
    // daca nu avem cuvinte, pun un cuvant de rezerva
    if (wordList.isEmpty()) {
        secretWord = "eroare";
    } else {
        // aleg un cuvant random din vector
        srand(static_cast<unsigned int>(time(nullptr)));
        int index = rand() % wordList.size();
        secretWord = wordList[index];
    }

    // fac afisarea cu _ _ _ pentru fiecare litera din cuvant
    currentDisplay = QString(secretWord.size(), '_');

    // curat literele folosite si resetez variabile
    usedLetters.clear();
    wrongGuesses = 0;
    lives = maxWrong;
    score = 0;
    gameOver = false;

    // activez toate butoanele de litere
    for (int i = 0; i < lettersLayout->count(); ++i) {
        QPushButton *btn = qobject_cast<QPushButton*>(lettersLayout->itemAt(i)->widget());
        if (btn)
            btn->setEnabled(true);
    }

    // dezactivez butonul de meniu cat timp jocul este in desfasurare
    menuButton->setEnabled(false);

    // actualizez afisarea pe ecran
    updateDisplay();
}

// functie chemata cand apas butonul de restart
void main_joc::restartGame() {
    startGame();
}

// functie care deseneaza spanzuratoarea si afiseaza toate informatiile pe ecran
void main_joc::updateDisplay() {
    // siruri cu desene ascii pentru fiecare numar de greseli
    QString states[12] = {
        "_________\n|       |\n|\n|\n|\n|\n",
        "_________\n|       |\n|       O\n|\n|\n|\n",
        "_________\n|       |\n|       O\n|       |\n|\n|\n",
        "_________\n|       |\n|       O\n|      /|\n|\n|\n",
        "_________\n|       |\n|       O\n|      /|\\\n|\n|\n",
        "_________\n|       |\n|       O\n|      /|\\\n|      /\n|\n",
        "_________\n|       |\n|       O\n|      /|\\\n|      / \\\n|\n",
        "_________\n|       |\n|      \\O\n|      /|\\\n|      / \\\n|\n",
        "_________\n|       |\n|      \\O/\n|      /|\\\n|      / \\\n|\n",
        "_________\n|       |\n|      \\O/\n|      /|\\\n|      / \\\n|     RIP\n",
        "_________\n|       |\n|      X_X\n|      /|\\\n|      / \\\n|     RIP\n",
        "_________\n|       |\n|      DEAD\n|      /|\\\n|      / \\\n|     RIP\n"
    };

    // ma asigur ca indexul este in interval
    int idx = wrongGuesses;
    if (idx < 0) idx = 0;
    if (idx > 11) idx = 11;

    // afisez spanzuratoarea corespunzatoare numarului de greseli
    hangmanLabel->setText(states[idx]);

    // fac afisarea cu spatii intre literele cuvantului curent
    QString spaced;
    for (QChar c : std::as_const(currentDisplay))
        spaced += QString(c) + " ";
    wordLabel->setText(spaced);

    // construiesc sirul cu litere folosite
    QString usedStr;
    for (QChar c : std::as_const(usedLetters))
        usedStr += QString(c) + " ";
    usedLabel->setText("Litere folosite: " + usedStr);

    // afisez scorul si vietile
    scoreLabel->setText("Scor: " + QString::number(score));
    livesLabel->setText("Vieti: " + QString::number(lives));

    // daca jocul s-a terminat (castigat sau pierdut)
    if (gameOver) {
        // afisez cuvantul complet cu litere mari
        wordLabel->setText(secretWord.toUpper());
        // schimb culoarea sa fie verde (cuvant final)
        wordLabel->setStyleSheet("font-size: 40px; font-weight: bold; color: #2e7d32;");
        // activez butonul de meniu, ca sa pot reveni la meniul principal
        menuButton->setEnabled(true);
    } else {
        // daca jocul nu s-a terminat, culoarea ramane albastra
        wordLabel->setStyleSheet("font-size: 40px; font-weight: bold; color: #1e88e5;");
    }
}

// functie care trateaza o incercare (o litera ghicita)
void main_joc::handleGuess(QChar key) {
    // daca jocul este deja terminat, nu mai fac nimic
    if (gameOver) return;
    // daca nu este litera, nu fac nimic
    if (!key.isLetter()) return;

    // transform litera in litera mica
    key = key.toLower();

    // daca litera a fost deja folosita, ies
    if (usedLetters.contains(key)) return;

    // adaug litera in setul de litere folosite
    usedLetters.insert(key);

    bool correct = false;

    // verific fiecare pozitie din cuvantul secret
    for (int i = 0; i < secretWord.size(); ++i) {
        if (secretWord[i] == key) {
            // daca litera se potriveste, o pun in currentDisplay
            currentDisplay[i] = key;
            correct = true;
        }
    }

    // daca litera a fost corecta, cresc scorul
    if (correct) {
        score += 10;
    } else {
        // daca litera e gresita, cresc numarul de greseli si scad viata
        wrongGuesses++;
        lives--;
    }

    // daca am ghicit tot cuvantul
    if (currentDisplay == secretWord) {
        gameOver = true;
        // nu mai schimb currentDisplay, deja contine cuvantul ghicit
    } else if (wrongGuesses >= maxWrong || lives <= 0) {
        // daca am depasit numarul maxim de greseli sau am ramas fara vieti
        gameOver = true;
        // fortam afisarea cuvantului corect
        currentDisplay = secretWord;
    }

    // actualizez tot ce se vede pe ecran
    updateDisplay();

    // daca jocul s-a terminat, dezactivez toate butoanele de litere
    if (gameOver) {
        for (int i = 0; i < lettersLayout->count(); ++i) {
            QPushButton *btn = qobject_cast<QPushButton*>(lettersLayout->itemAt(i)->widget());
            if (btn)
                btn->setEnabled(false);
        }
    }
}

// functie apelata cand apas o tasta de la tastatura fizica
void main_joc::keyPressEvent(QKeyEvent *event) {
    if (!event) return;
    if (event->text().isEmpty()) return;

    // preiau primul caracter din textul evenimentului
    QChar key = event->text().at(0);
    // trimit litera la handleGuess
    handleGuess(key);
}

// functie apelata cand apas pe un buton de litera din interfata
void main_joc::letterClicked() {
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    // litera este textul de pe buton
    QChar key = btn->text().at(0);

    // dezactivez butonul ca sa nu il mai pot apasa iar
    btn->setEnabled(false);

    // trimit litera la handleGuess
    handleGuess(key);
}

// functie care emite semnalul ca vrem sa ne intoarcem la meniul principal
void main_joc::backToMenu() {
    emit backToMenuRequested();
}
