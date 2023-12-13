// WORD GAME
// GRUPO 2 - EDA - L.BIO
// Ana Francisca Barros
// Catarina Pais
// Ema Beira
//================================================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <ios>
#include <limits>
#include <ctime>
#include <cstdlib>
#include <random>
#include <set>
#include <iterator>
//--------------------------------------------------------------------------------
using namespace std;
//================================================================================
// COLOR CODES / ANSI ESCAPE SEQUENCES
//================================================================================
// TEXT COLOR
#define NO_COLOR "\033[0m"
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define BROWN "\033[0;33m"
#define BLUE "\033[0;34m"
#define MAGENTA "\033[0;35m"
#define CYAN "\033[0;36m"
#define LIGHTGRAY "\033[0;37m"
#define DARKGRAY "\033[1;30m"
#define LIGHTRED "\033[1;31m"
#define LIGHTGREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define LIGHTBLUE "\033[1;34m"
#define LIGHTMAGENTA "\033[1;35m"
#define LIGHTCYAN "\033[1;36m"
#define WHITE "\033[1;37m"
// BACKGROUND COLOR
#define BLACK_B "\033[0;40m"
#define RED_B "\033[0;41m"
#define GREEN_B "\033[0;42m"
#define YELLOW_B "\033[0;43m"
#define BLUE_B "\033[0;44m"
#define MAGENTA_B "\033[0;45m"
#define CYAN_B "\033[0;46m"
#define WHITE_B "\033[1;47m"
//================================================================================
// USER DEFINED TYPES
//================================================================================

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
void toupperStr(string &s) {
    for (auto &c: s) c = toupper(c);
}

//--------------------------------------------------------------------------------
typedef vector<string> WordsOnInitialBoard; //vetor que guarda todas as palavras do board inicial

typedef struct {
    char lin;
    char col;
    char dir;
} WordPosition;
typedef struct {
    char lin;
    char col;
} LetterPosition;
typedef struct {
    char letter;
    int playerId; //'1','2','3','4' - played by that player / '0' - letter hasn't been played
    //LetterPosition pos;
} Letters;
typedef struct {
    WordPosition pos;
    string word;
    //vector<Letters> word;
} WordOnBoard;

class Board //board inicial do text file previamente criado
{
public:
    int getlines() const;

    int getcolumns() const;

    bool canPlayAt(char line, char column, char letter) const;

    bool stillOnPlay();

    bool insert(char line, char column, char letter, int nplayer);

    Board(); //default constructor
    Board(int numLins, int numCols);  // constructor1
    Board(const string &filename);    // constructor2 - reads a file with the board
    void show() const;

    int whoFinished(WordOnBoard word);

    vector<vector<Letters>> getLetters();
    // string getLetters(); // usar para criar o board!!
private:
    vector<vector<Letters>> gameBoard;
    int numLins_;
    int numCols_;
    vector<WordOnBoard> wordsOnBoard;
    //int numWords;
    string filename;
};

Board::Board() : numLins_(7), numCols_(7), gameBoard(
        vector<vector<Letters>>(7, vector<Letters>(7, {'.', 0}))) {}

Board::Board(int numLins, int numCols) : numLins_(numLins), numCols_(numCols), gameBoard(
        vector<vector<Letters>>(numLins, vector<Letters>(numCols, {'.', 0}))) {}

Board::Board(const string &filename) { // constructor2 definition
    ifstream f(filename);
    if (!(f.is_open())) {
        cerr << "File not found!\n";
        exit(1);
    }
    string dummy, boardSize, lines, pos, word;
    //getline(f,dummy);getline(f,dummy); // tf o que faz esta linha aqui
    getline(f, boardSize);
    istringstream iss(boardSize);
    iss >> dummy >> dummy >> dummy >> numLins_ >> dummy >> numCols_;
    getline(f, dummy);
    getline(f, dummy);
    getline(f, dummy);
    getline(f, dummy);
    gameBoard = vector<vector<Letters>>(numLins_, vector<Letters>(numCols_, {'.', 0}));

    for (int i = 0; i < numLins_; i++) {
        getline(f, lines);
        istringstream iss(lines);
        iss >> dummy; //remove capslock letter regarding the number of lines
        for (int j = 0; j < numCols_; j++) {
            iss >> gameBoard.at(i).at(j).letter;
            gameBoard.at(i).at(j).playerId = 0;
        }
    }
    getline(f, dummy); //ignore "WORDS : "
    getline(f, dummy);
    getline(f, dummy);
    while (getline(f, lines)) {
        istringstream iss(lines);
        iss >> pos >> word;
        WordPosition position;

        position.lin = pos[0];
        position.col = pos[1];
        position.dir = pos[2];
        //if (position.dir == 'H') {
        //    for (int i = 0; i < word.size(); i++) {
        //        //wordsOnBoard.push_back()
        //    }
        //}
        wordsOnBoard.push_back({position, word});
    }
}

// Shows the board ‘b’ on the screen
// The line and column identifying letters are colored in red
void Board::show() const {
    cout << endl;
    cout << NO_COLOR << "  ";
    for (size_t j = 0; j < gameBoard.at(0).size(); j++)
        cout << (char) ('a' + j) << ' ';
    cout << endl;
    cout << NO_COLOR;
    for (size_t i = 0; i < gameBoard.size(); i++) {
        cout << NO_COLOR << (char) ('A' + i) << ' ' << WHITE_B;
        for (size_t j = 0; j < gameBoard.at(i).size(); j++) {
            if (gameBoard.at(i).at(j).letter == '.') {
                cout << BLACK << WHITE_B << "  ";
            } else if (gameBoard.at(i).at(j).playerId == 0) { // letter can still be played
                cout << BLACK << WHITE_B << gameBoard.at(i).at(j).letter << ' ';
            } else if (gameBoard.at(i).at(j).playerId > 0) { // letter has been played
                cout << RED << WHITE_B << gameBoard.at(i).at(j).letter << ' ';
            }
        }
        cout << NO_COLOR << endl;
    }
    cout << endl;
}

bool Board::canPlayAt(char line, char column, char letter) const {
    int lin = (int) line - 'A';
    int col = (int) column - 'a';
    bool plays = 1;
    for (int a = 0; a < wordsOnBoard.size(); a++) { // checking every word on the board
        if (wordsOnBoard.at(a).pos.dir == 'V') { // if the word is set vertically (lines change)

            for (int b = 0; b < wordsOnBoard.at(a).word.size(); b++) { // check if letters correspond
                if (wordsOnBoard.at(a).word.at(b) == letter && col == wordsOnBoard.at(a).pos.col - 'a' &&
                    lin ==
                    wordsOnBoard.at(a).pos.lin + b - 'A') { // has found the letter to insert and in which word it is

                    // condition for checking if past letters have been put on board
                    for (int c = b - 1; c >= 0; c--) { // check letters before the one we want to insert
                        if (gameBoard.at((int) wordsOnBoard.at(a).pos.lin + c - 'A').at(
                                (int) wordsOnBoard.at(a).pos.col - 'a').playerId == 0) {
                            // letter before hasn't been played
                            plays = 0;
                        }
                    }
                }

            }
            if (!plays) {
                break;
            }
        } else if (wordsOnBoard.at(a).pos.dir == 'H') { // if the word is set horizontally (columns change)

            for (int b = 0; b < wordsOnBoard.at(a).word.size(); b++) { // check if letters correspond

                if (wordsOnBoard.at(a).word.at(b) == letter && col == wordsOnBoard.at(a).pos.col + b - 'a' &&
                    lin == wordsOnBoard.at(a).pos.lin - 'A') { // has found the letter to insert and in which word it is
                    // condition for checking if past letters have been put on board
                    for (int c = b - 1; c >= 0; c--) { // check letters before the one we want to insert
                        if (gameBoard.at((int) wordsOnBoard.at(a).pos.lin - 'A').at(
                                (int) wordsOnBoard.at(a).pos.col + c - 'a').playerId == 0) {
                            // letter before hasn't been played
                            plays = 0;
                        }
                    }
                }

            }
            if (!plays) {
                break;
            }
        }
    }

    if (gameBoard.at(lin).at(col).letter != letter) { // the letter is different from the posi
        plays = 0;
    } else if (gameBoard.at(lin).at(col).playerId > 0) { // a player has played in that position
        plays = 0;
    }
    return plays;
}

bool Board::insert(char line, char column, char letter, int nplayer) {
    int lin = (int) line - 'A';
    int col = (int) column - 'a';
    bool canPlay = canPlayAt(line, column, letter);
    if (!canPlay) {
        cout << RED << "Letter can't be placed there ! \n" << NO_COLOR;
    } else
        gameBoard.at(lin).at(col).playerId = nplayer;

    return canPlay; // returns true if it has successfully inserted a letter on the board
}

int Board::getlines() const {
    return numLins_;
}

int Board::getcolumns() const {
    return numCols_;
}

vector<vector<Letters>> Board::getLetters() {
    return gameBoard;
}

int Board::whoFinished(WordOnBoard word) { // returns the id of who finished that specific word
    int lin = word.pos.lin - 'A';
    int col = word.pos.col - 'a';
    int id;
    if (word.pos.dir == 'V') { // vertical word: lines change
        id = gameBoard.at(lin + word.word.size()).at(col).playerId;
    } else { // horizontal word: columns change
        id = gameBoard.at(lin).at(col + word.word.size()).playerId;
    }
    return id;
}

bool Board::stillOnPlay() { // check if there is still any letter to be played at the board
    bool a = 0;
    for (int i = 0; i < numLins_; i++) {
        for (int j = 0; j < numCols_; j++) {
            if (gameBoard.at(i).at(j).letter != '.' && gameBoard.at(i).at(j).playerId == 0) {
                a = 1;
            }
        }
    }
    return a;
}

class Bag //todas as letras
{
public:
    Bag();

    Bag(Board &gameboard);

    vector<char> get();

    bool isEmpty();

    void insert(const char &letter);

    char remove();

    void exchange(char bylet);

    void shuffle();

private:
    Board gameBoard_;
    int numLets_;
    vector<char> lets;
};

Bag::Bag() { // default constructor
    this->numLets_ = 0;
    lets = vector<char>();
}

Bag::Bag(Board &gameBoard) { // le todo o board e guarda as letras que nao são pontos
    this->gameBoard_ = gameBoard;
    for (int i = 0; i < gameBoard_.getlines(); i++) {
        for (int j = 0; j < gameBoard_.getcolumns(); j++) {
            char letter = gameBoard_.getLetters().at(i).at(j).letter;
            if (letter != '.') {
                lets.push_back(letter);
            }
        }
    }
    this->numLets_ = lets.size();
}

vector<char> Bag::get() {
    return lets;
}

bool Bag::isEmpty() {
    if (lets.empty())
        return (1); // vector of letters is empty
    else
        return (0); // vector of letters is yet not empty
}

void Bag::insert(const char &letter) {
    lets.push_back(letter); // inserts on the bag the letter that the user has to exchange
}

char Bag::remove() {
    // erase the first element from the vector
    lets.erase(lets.begin());

    // make a copy of the removed element (the first occurrence, in case there's more than one)
    char removedElements{lets.front()};
    // Return the vector containing the removed element
    return removedElements;
}

void Bag::exchange(char bylet) { //troca as letras do Bag
    insert(bylet); //insere a letra da hand do jogador
    remove(); //retirar do bag as letras
}

void Bag::shuffle() { // verificar se faz algo de jeito
    std::random_device rd;
    std::mt19937 g(rd());
    ::shuffle(lets.begin(), lets.end(), g); // bag is shuffled
}

class Hand // letters that each player has on the hand
{
public:
    Hand(); //default constructor
    Hand(int &numLets); //constructor1 - inicializa a mao do jogador
    void remove(char letter);

    void update();

    void show() const;

    multiset<char> getLetters() const;

    void exchange(char returnlet, char bylet);

    bool isEmpty();

private:
    int numLets_;
    multiset<char> hand_;
};

Hand::Hand() {// default constructor
    this->numLets_ = 5; // default number of letters when not specified
}

Hand::Hand(int &numLets) { // constructor1 definition
    this->numLets_ = numLets;
}

void Hand::remove(char letter) { // when a letter is inserted on the board, it has to be removed from the hand
    if (hand_.count(letter) > 0) //verifica se encontrou a letra
    {
        auto it = hand_.find(letter);
        hand_.erase(it);
        char removedLetter = *it;
    } else
        cout << "That letter doesn't exist in the hand";
};

void Hand::show() const { // VERIFICAR SE FUNCIONA
    auto it = hand_.begin(); // iterator that points to the 1st position of the multiset
    for (int i = 0; i < numLets_; i++) {
        advance(it, i);
        cout << *it << " ";
    }
}

void Hand::exchange(char returnlet, char bylet) {
    hand_.insert(returnlet);    //insere na hand do jogador a letra retirada do board
    remove(bylet);           //retirar da hand as letras que o jogador não quer
}

bool Hand::isEmpty() {
    if (hand_.empty())
        return (1); // vector of letters on hand is empty
    else
        return (0); // vector of letters on hand is yet not empty
}

multiset<char> Hand::getLetters() const { // é necessário?
    return hand_;
}

class Player //cada um dos jogadores
{
public:
    Player();

    Player(const string &name, int &numLets);

    // remover os objetos bag e board daqui; deixar hand
    Board gameBoard;
    Bag bag;

    int getId() const;

    string getName() const;

    int getPoints() const;

    Hand getHand() const;

    void addPoints(int points);

    void insertHand(const string &letters);

    string removeHand(const string &letters);

private:
    static int playerCount;  //mantém track da contagem dos players, como é static  is shared among all instances of the class, and its value persists across different objects.
    int playerId_;        //  number assigned to each player
    string name_;
    int points_;
    Hand hand_; // each player has a hand with letters
};

int Player::playerCount = 0; // static member variables of a class must be defined outside of the class.This is because the static member variable exists independently of any specific instance of the class, and its memory must be allocated separately. (basicamente para tds os players a variavel player count vai ser a mm)

Player::Player(const string &name, int &numLets) {
    //a cada player vou atribuir um nome e um numero (para o caso de haver nomes iguais)
    this->name_ = name;
    playerCount++; // cada vez que o construtor é chamado esta variável incrementa
    this->playerId_ = playerCount;
    this->hand_ = Hand(numLets);
    this->points_ = 0;
}

int Player::getId() const {
    return playerId_;
}

string Player::getName() const {
    return name_;
}

int Player::getPoints() const {
    return points_;
}

Hand Player::getHand() const {
    return hand_;
}

void Player::addPoints(int points) {
    this->points_ += points;
}

int main() {
    int numPlayers, numLetters, p = 1, i = 0;
    string filename, name;
    vector<string> names;

    cout << "How many players ? ";
    cin >> numPlayers;
    // if clause para verificar numero de players entre 2 e 4
    bool validNumberP = 0;
    while (!validNumberP) {
        if (numPlayers >= 2 && numPlayers <= 4) {
            validNumberP = 1;
        } else {
            cout << "Invalid number of players. Please enter a number between 2 and 4." << endl;
            cout << "How many players? ";
            cin >> numPlayers;
        }

    }
    while (p <= numPlayers) {
        cout << "Name of Player " << p << " ? ";
        cin >> name;
        names.push_back(name);
        p++;
    }

    cout << "Board filename ? ";
    cin >> filename;
    // if clause

    Board board(filename);
    board.show();
    Bag bag(board);
    vector<Player> players;

    cout << "How many letters do you want to play with ? ";
    cin >> numLetters;
    bool validLetters = 0;
    while (!validLetters) {
        if (numLetters < bag.get().size() / numPlayers) {
            validLetters = 1;
        } else {
            cout << "There are not enough letters to be distributed by all players ! " << endl;
            cout << "How many letters for each player ? ";
            cin >> numLetters;
        }

    }

    for (int j = 0; j < numPlayers; j++) {
        Player jog(names.at(j), numLetters);
        players.push_back(jog);
        for (int i = 0; i < numLetters; i++) {
            char remlet;
            remlet = bag.remove();
            players.at(j).getHand().getLetters().insert(remlet);
        }
        cout << "Player " << j + 1 << ": ";
        players.at(j).getHand().show(); // DEVE ESTAR MAL!!
    }
    int letPerRound;
    bool validLetPerRound = 0;
    cout << "Do you want to play 1 or 2 letters per round?" << endl;
    cin >> letPerRound;
    while (!validLetPerRound) {
        if (letPerRound < 1 || letPerRound > 2) {
            validLetPerRound = 1;
        } else {
            cout << "You can only play 1 or 2 ! " << endl;
            cout << "How many letters per round ? ";
            cin >> letPerRound;
        }
    }
    int ema = 0;
    while (board.stillOnPlay()) { // when there are still letters to be played
        for (int j = 0; j < numPlayers; j++) {
            ema = 0;
            while (ema<letPerRound) {

                // codigo para insertion/deletion of letters
                string positions;
                char letter;
                bool canPlay = 1;
                cout << "Position (Lc) ? ";
                cin >> positions;
                cout << "Letter ? ";
                cin >> letter;
                while(!board.insert(positions.at(0),positions.at(1),letter,j+1)) {
                    cout << "Position (Lc) ? ";
                    cin >> positions;
                    cout << "Letter ? ";
                    cin >> letter;
                }

                ema++;
            }
            //update
        }

    }



    return 0;
}