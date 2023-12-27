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

//================================================================================
class Board //board inicial do text file previamente criado
{
public:
    int getlines() const;

    int getcolumns() const;

    size_t getNumWords() const;

    vector<WordOnBoard> getWords() const;

    bool canPlayAt(char line, char column, char letter) const;

    bool stillOnPlay();

    bool insert(char line, char column, char letter, int nplayer);

    // constructors:
    Board();

    Board(int numLins, int numCols);  // constructor1
    Board(const string &filename);    // constructor2 - reads a file with the board

    void show() const;

    int whoFinished(WordOnBoard word);

    vector<vector<Letters>> getLetters();

private:
    vector<vector<Letters>> gameBoard;
    int numLins_;
    int numCols_;
    vector<WordOnBoard> wordsOnBoard;
    //int numWords;
    string filename_;
};

Board::Board() : numLins_(7), numCols_(7), gameBoard(
        vector<vector<Letters>>(7, vector<Letters>(7, {'.', 0}))) {}

Board::Board(int numLins, int numCols) : numLins_(numLins), numCols_(numCols), gameBoard(
        vector<vector<Letters>>(numLins, vector<Letters>(numCols, {'.', 0}))) {}

Board::Board(const string &filename) { // constructor2 definition
    this->filename_ = filename;
    ifstream f(filename_);
    if (!(f.is_open())) {
        cerr << "File not found!\n";
        return;
    }
    string dummy, boardSize, lines, pos, word;
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
    bool plays = 0;
    int countPlays;
    for (int a = 0; a < wordsOnBoard.size(); a++) { // checking every word on the board
        if (wordsOnBoard.at(a).pos.dir == 'V') { // if the word is set vertically (lines change)
            for (int b = 0; b < wordsOnBoard.at(a).word.size(); b++) { // check if letters correspond
                if (wordsOnBoard.at(a).word.at(b) == letter && col == wordsOnBoard.at(a).pos.col - 'a' &&
                    lin ==
                    wordsOnBoard.at(a).pos.lin + b - 'A') { // has found the letter to insert and in which word it is
                    // condition for checking if past letters have been put on board
                    countPlays = 0;
                    for (int c = b - 1; c >= 0; c--) { // check letters before the one we want to insert
                        if (gameBoard.at((int) wordsOnBoard.at(a).pos.lin + c - 'A').at(
                                (int) wordsOnBoard.at(a).pos.col - 'a').playerId > 0) {
                            // letter before has been played
                            countPlays++; // letter can't be played in the associated vertical word
                        }
                    }
                    if (countPlays == b) plays = 1;
                }
            }
        } else if (wordsOnBoard.at(a).pos.dir == 'H') { // if the word is set horizontally (columns change)
            for (int b = 0; b < wordsOnBoard.at(a).word.size(); b++) { // check if letters correspond

                if (wordsOnBoard.at(a).word.at(b) == letter && col == wordsOnBoard.at(a).pos.col + b - 'a' &&
                    lin == wordsOnBoard.at(a).pos.lin - 'A') { // has found the letter to insert and in which word it is

                    // condition for checking if past letters have been put on board
                    countPlays = 0;
                    for (int c = b - 1; c >= 0; c--) { // check letters before the one we want to insert
                        if (gameBoard.at((int) wordsOnBoard.at(a).pos.lin - 'A').at(
                                (int) wordsOnBoard.at(a).pos.col + c - 'a').playerId > 0) {
                            // letter before hasn't been played
                            countPlays++;
                        }
                    }
                    if (countPlays == b) plays = 1;
                }

            }
        }
    }

    if (gameBoard.at(lin).at(col).letter != letter) { // the letter is different from the position on the board
        plays = 0;
    } else if (gameBoard.at(lin).at(col).playerId > 0) { // a player has been played in that position
        plays = 0;
    }
    return plays;
}

bool Board::insert(char line, char column, char letter, int nplayer) {
    int lin = (int) line - 'A';
    int col = (int) column - 'a';
    bool canPlay = canPlayAt(line, column, letter);

    if (!canPlay) cout << RED << "Letter can't be placed there ! \n" << NO_COLOR;
    else gameBoard.at(lin).at(col).playerId = nplayer;

    return canPlay; // returns true if it has successfully inserted a letter on the board
}

int Board::getlines() const {
    return numLins_;
}

int Board::getcolumns() const {
    return numCols_;
}

size_t Board::getNumWords() const {
    return wordsOnBoard.size();
}

vector<vector<Letters>> Board::getLetters() {
    return gameBoard;
}

vector<WordOnBoard> Board::getWords() const {
    return wordsOnBoard;
}


int Board::whoFinished(WordOnBoard word) { // returns the id of who finished that specific word
    int lin = word.pos.lin - 'A';
    int col = word.pos.col - 'a';
    int id;
    if (word.pos.dir == 'V') { // vertical word: lines change
        id = gameBoard.at(lin + word.word.size() - 1).at(col).playerId;
    } else { // horizontal word: columns change
        id = gameBoard.at(lin).at(col + word.word.size() - 1).playerId;
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

    int size();

private:
    Board gameBoard_;
    int numLets_;
    vector<char> lets;
};

Bag::Bag() { // default constructor
    this->numLets_ = 0;
    lets = vector<char>();
}

Bag::Bag(Board &gameBoard) { // reads the whole board and saves all positions different from '.'
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
    shuffle();
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
    // make a copy of the removed element (the first occurrence, in case there's more than one)
    char removedElements{lets.front()};

    // erase the first element from the vector
    lets.erase(lets.begin());

    // Return the vector containing the removed element
    return removedElements;
}

void Bag::exchange(char bylet) { //troca as letras do Bag
    insert(bylet); //insere a letra da hand do jogador
    remove(); //retirar do bag as letras
}

void Bag::shuffle() { // verificar se faz algo de jeito
    std::random_device rd; // random number generator (non-deterministic random numbers)
    std::mt19937 g(rd()); // mersenne twister engine
    std::shuffle(lets.begin(), lets.end(), g); // bag is shuffled
}

int Bag::size() {
    return lets.size();
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

    bool isOnHand(char letter);

    void insert(char letter);

    int size();

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
    bool isIt = isOnHand(letter);
    if (isIt) // check if it has found the letter
    {
        auto it = hand_.find(letter);
        hand_.erase(it);
        char removedLetter = *it;
    }
}

void Hand::show() const {
    auto it = hand_.begin(); // iterator that points to the 1st position of the multiset
    for (int i = 0; i < hand_.size(); i++) {
        cout << NO_COLOR << *it << " ";
        it++;
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

bool Hand::isOnHand(char letter) {
    hand_.count(letter) > 0;
    auto it = hand_.find(letter);
    if (it != hand_.end()) { // letter was found in the hand_ multiset
        return 1;
    } else { // letter wasn't found in the hand_ multiset
        cout << RED << "That letter is not on your hand ! \n" << NO_COLOR;
        return 0;
    }
}

void Hand::insert(char letter) {
    hand_.insert(letter);
}

multiset<char> Hand::getLetters() const {
    return hand_;
}

int Hand::size() {
    return hand_.size();
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

    Hand &getHand();

    void addPoints(int points);

    void insertHand(const string &letters);

    string removeHand(const string &letters);

    void quitPlayer();


private:
    static int playerCount;  //mantém track da contagem dos players, como é static  is shared among all instances of the class, and its value persists across different objects.
    int playerId_;        //  number assigned to each player
    string name_;
    int points_;
    Hand hand_; // each player has a hand with letters
};

int Player::playerCount = 0; // static member variables of a class must be defined outside the class.This is because the static member variable exists independently of any specific instance of the class, and its memory must be allocated separately. (basicamente para tds os players a variavel player count vai ser a mm)

Player::Player(const string &name, int &numLets) {
    // will attribute to each player (instance) a name, an id, a hand)
    this->name_ = name;
    playerCount++; // each time the constructor is called, this variable increments (it's static)
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

Hand &Player::getHand() {
    return hand_;
}

void Player::addPoints(int points) {
    this->points_ += points;
}

void bubbleSortPoints(vector<Player> &v) {
    size_t size = v.size();
    int i = (int) size - 1;
    while (i >= 0) {
        for (int j = 0; j < i; j++) {
            if (v.at(j).getPoints() < v.at(j + 1).getPoints()) {
                Player spare = v.at(j);
                v.at(j) = v.at(j + 1);
                v.at(j + 1) = spare;
            }
        }
        i--;
    }
}


void Player::quitPlayer() {
    name_ = "\0";
    playerId_ = -1;
    hand_.getLetters().clear();
}

int main() {
    int numPlayers, numLetters, p = 1, i = 0;
    string filename, name;
    vector<string> names;

    cout << LIGHTMAGENTA << "~ ~ ~ WORDS GAME ~ ~ ~\n\n" << NO_COLOR;

    bool validNumberP = 0;
    while (!validNumberP) {
        cout << LIGHTCYAN << "How many players (2-4) ? " << NO_COLOR;
        cin >> numPlayers;
        if (cin.fail()) {
            cin.clear();
            cout << RED << "The input failed.\n" << NO_COLOR;
        } else if (numPlayers >= 2 && numPlayers <= 4) {
            validNumberP = 1;
        } else {
            cout << RED << "Invalid number of players. Please enter a number between 2 and 4." << NO_COLOR << endl;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    while (p <= numPlayers) {
        cout << LIGHTCYAN << "Name of Player " << p << " ? " << NO_COLOR;
        cin >> name;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        names.push_back(name);
        p++;
    }

    cout << LIGHTCYAN << "\nBoard filename ? " << NO_COLOR;
    cin >> filename;
    Board board(filename);

    bool validInput = false;
    while (!validInput) {
        if (cin.fail()) {
            cin.clear();  // clear the error state
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // clean input buffer
            cout << RED << "Invalid filename. Please try again.\n" << NO_COLOR;
        } else if (board.getNumWords() > 0) {
            validInput = true;
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // clean input buffer
        } else {
            cout << RED << "Invalid filename. Please try again.\n" << NO_COLOR;
        }

        if (!validInput) {
            cout << LIGHTCYAN << "\nBoard filename ? " << NO_COLOR;
            cin >> filename;
            board = Board(filename);  // Update the existing board object
        }
    }

    board.show();
    Bag bag(board);
    vector<Player> players;

    bool validLetters = 0;
    while (!validLetters) {
        cout << LIGHTCYAN << "How many letters do you want to play with ? " << NO_COLOR;
        cin >> numLetters;
        if (cin.fail()) {
            cin.clear();
            cout << RED << "The input failed. \n" << NO_COLOR;
        } else if (numLetters < bag.get().size() / numPlayers) {
            validLetters = 1;
        } else {
            cout << RED << "There are not enough letters to be distributed by all players ! \n" << NO_COLOR;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    for (int j = 0; j < numPlayers; j++) {
        Player jog(names.at(j), numLetters);
        players.push_back(jog);
        for (int i = 0; i < numLetters; i++) {
            char remlet;
            remlet = bag.remove();
            players.at(j).getHand().insert(remlet);
        }
        cout << LIGHTCYAN << "Player " << j + 1 << ": " << NO_COLOR;
        players.at(j).getHand().show();
        cout << endl;
    }
    int letPerRound;
    bool validLetPerRound = 0;
    cout << LIGHTCYAN << "How many letters can each player play each round (1-2) ? " << NO_COLOR;
    cin >> letPerRound;
    while (!validLetPerRound) {
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "The input failed.\n" << NO_COLOR;
            cout << LIGHTCYAN << "How many letters per round ? " << NO_COLOR;
            cin >> letPerRound;
        } else if (letPerRound < 1 || letPerRound > 2) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "You can only play 1 or 2 ! \n" << NO_COLOR;
            cout << LIGHTCYAN << "How many letters per round ? " << NO_COLOR;
            cin >> letPerRound;
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            validLetPerRound = 1;
        }
    }

    int ema = 0;
    while (board.stillOnPlay()) { // when there are still letters to be played
        for (int j = 0; j < numPlayers; j++) { // indexing players
            if (players.at(j).getId() == -1) continue; // whenever a player has quit the game
            else if (players.at(j).getHand().isEmpty()) continue; // whenever a player no longer has letters on hand

            cout << endl << players.at(j).getName() << ", it's your turn ! \n\n";

            // Check if the player's hand is empty and only show it when it's not
            if (!players.at(j).getHand().isEmpty()) {
                cout << LIGHTCYAN << "Hand : " << NO_COLOR;
                players.at(j).getHand().show();
                cout << endl;
            } else {
                cout << "You have no more letters in the hand!";
            }

            ema = 0;

            // cycle that checks if any letter on hand can be played or not. if not, letters can be exchanged
            bool canPlay = 0; // checking if any of the letters on hand can be played
            for (char it: players.at(j).getHand().getLetters()) {
                // checking all letters on hand
                for (int m = 0; m < board.getlines(); m++) { // checking all lines
                    for (int t = 0; t < board.getcolumns(); t++) { // checking all columns
                        if (board.canPlayAt(char(m + 65), char(t + 97),
                                            it)) { // if a letter can be placed in that position
                            canPlay = 1;
                            break;
                        }
                    }
                }
            }


            // if canPlay = 1, there's at least one letter that can be played, so the player won't be able to exchange any letter
            if (!canPlay && bag.size() > 0) { // exchange of letters (when there are still letters on the bag!)
                int excLetters;
                if (players.at(j).getHand().size() == 1) { // only one letter on hand
                    cout << RED << "You can't play that letter !" << NO_COLOR;
                    excLetters = 0;
                    char willExchange;
                    bool validExcLet = 0;
                    while (!validExcLet) {
                        cout << LIGHTCYAN << "\nWould you like to exchange your letter ? (Y/N) " << NO_COLOR;
                        cin >> willExchange;
                        if (cin.fail()) {
                            cin.clear();
                            cout << RED << "The input failed.\n" << NO_COLOR;
                        } else if (willExchange != 'Y' && willExchange != 'N') { // wrong input
                            cout << RED << "Please only write Y or N ! " << NO_COLOR << endl;
                        } else {
                            validExcLet = 1;
                            if (willExchange == 'Y') excLetters = 1;
                        }
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    } // will only leave this loop when the input is either 'Y' or 'N'
                } else {
                    cout << RED << "You can't play any letters !\n" << NO_COLOR;
                    bool validExcLet = 0;
                    while (!validExcLet) {
                        cout << LIGHTCYAN << "Would you like to exchange 1 or 2 letters ? " << NO_COLOR;
                        cin >> excLetters;
                        if (cin.fail()) {
                            cin.clear();
                            cout << RED << "The input failed.\n" << NO_COLOR;
                        } else if (excLetters < 1 || excLetters > 2) {
                            cout << RED << "You can only exchange 1 or 2 letters ! " << NO_COLOR << endl;
                        } else {
                            validExcLet = 1;
                        }
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    } // will only leave this loop when the number of letters is within the limits or a valid input
                }
                for (int exc = 0; exc < excLetters; exc++) {
                    // removing a letter from hand chosen by the player
                    char letter;
                    bool canRemove = 0;
                    while (!canRemove) {
                        cout << LIGHTCYAN << "Letter to remove ? " << NO_COLOR;
                        cin >> letter;
                        if (cin.fail()) {
                            cin.clear();
                            cout << RED << "The input failed. \n" << NO_COLOR;
                        } else if (players.at(j).getHand().isOnHand(letter)) { // if that letter is on the hand
                            //proceeding to remove the letter from the hand
                            players.at(j).getHand().remove(letter); // removing letter from hand
                            bag.insert(letter); // inserting that same letter on the bag
                            bag.shuffle(); // shuffling the bag after inserting a new letter
                            // inserting a new letter on hand (while removing it from the bag)
                            players.at(j).getHand().insert(bag.remove());
                            cout << LIGHTCYAN << "Letter " << letter << " removed from your hand !" << NO_COLOR << endl;
                            canRemove = 1;
                        }
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    }
                }
                cout << LIGHTCYAN << "Hand : " << NO_COLOR;
                players.at(j).getHand().show();
                cout << endl;
                board.show();
            }

            while (ema < letPerRound) { // for each player to play either 1 or 2 letters per round
                if (ema > 0) {
                    cout << LIGHTCYAN << "Hand : " << NO_COLOR;
                    players.at(j).getHand().show();
                    cout << endl;
                }
                string positions;
                char letter;
                bool rightPos = 0;
                while (!rightPos) {
                    cout << LIGHTCYAN << "Position (Lc / PASS / QUIT) ? " << NO_COLOR;
                    cin >> positions;
                    if (cin.fail()) {
                        cin.clear();
                        cout << RED << "The input failed.\n" << NO_COLOR;
                    } else if (positions == "PASS") {
                        rightPos = 1;
                        ema = 2;
                        continue;
                    } else if (positions == "QUIT") {
                        rightPos = 1;
                        //Insert the letters in the players' hand into the bag
                        for (char letter_hand: players.at(j).getHand().getLetters()) {
                            bag.insert(letter_hand);
                        }
                        // change the players name to "", Player Id to -1 and empties the hand
                        players.at(j).quitPlayer();
                        ema = 2;
                        break;
                    } else if (positions.at(0) < board.getlines() + 'A' && positions.at(0) >= 'A' &&
                               positions.at(1) < board.getcolumns() + 'a' &&
                               positions.at(1) >= 'a') { // position within the range
                        rightPos = 1;
                        bool rightLet = 0;
                        while (!rightLet) {
                            cout << LIGHTCYAN << "Letter ? " << NO_COLOR; // necessário por isto aqui?
                            cin >> letter;
                            if (cin.fail()) {
                                cin.clear();
                                cout << RED << "The input failed.\n" << NO_COLOR;
                            } else if (!board.canPlayAt(positions.at(0), positions.at(1),
                                                        letter)) { // if letter cannot be inserted
                                cout << RED << "Letter can't be placed there ! \n" << NO_COLOR;
                                break;
                            } else if (!players.at(j).getHand().isOnHand(letter)) { // letter is not on the hand
                                break;
                            } else { // letter can be inserted
                                // inserting the letter on the board
                                board.insert(positions.at(0), positions.at(1), letter, j + 1);
                                board.show();
                                // removing letter from the hand when it has been successfully played
                                players.at(j).getHand().remove(letter);

                                // updating the hand (if there are still letters on the bag)
                                if (bag.size() > 0) {
                                    players.at(j).getHand().insert(bag.remove());
                                }
                                rightLet = 1;
                                ema++;
                                break;
                            }
                            cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        }
                    } else { // out of range
                        cout << RED << "Invalid position !\n" << NO_COLOR;
                    }
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
        }
    }

    // checking who won the game
    for (int n = 0; n < board.getNumWords(); n++) {
        players.at(board.whoFinished(board.getWords().at(n)) - 1).addPoints(1);
    }

    vector<Player> playersOnGame;
    // at the end of the game, sorting the vector of players' points (players who haven't quit)
    for (int j = 0; j < players.size(); j++) {
        if (players.at(j).getId() != -1) { // players who haven't quit the game
            playersOnGame.push_back(players.at(j));
        }
    }
    bubbleSortPoints(playersOnGame);

    cout << LIGHTCYAN << "\nThe game has ended ! Let's find out the podium ! \n\n" << NO_COLOR;

    // podium for the players
    if (playersOnGame.size() == 1) {
        cout << LIGHTMAGENTA << "1st place: " << playersOnGame.at(0).getName() << " ("
             << playersOnGame.at(0).getPoints()
             << " points)" << " ! \n";
    }
    if (playersOnGame.size() == 2) {
        if (players.at(0).getPoints() > players.at(1).getPoints()) {
            cout << LIGHTMAGENTA << "1st place: " << playersOnGame.at(0).getName() << " ("
                 << playersOnGame.at(0).getPoints()
                 << " points)" << " ! \n";
            cout << LIGHTMAGENTA << "2nd place: " << playersOnGame.at(1).getName() << " ("
                 << playersOnGame.at(1).getPoints()
                 << " points)"
                 << " ! \n" << NO_COLOR;
        }
    } else {
        cout << LIGHTMAGENTA << "1st place: " << NO_COLOR << playersOnGame.at(0).getName() << " ("
             << playersOnGame.at(0).getPoints()
             << " points)" << " ! \n";
        cout << LIGHTMAGENTA << "2nd place: " << NO_COLOR << playersOnGame.at(1).getName() << " ("
             << playersOnGame.at(1).getPoints() << " points)"
             << " ! \n";
        cout << LIGHTMAGENTA << "3rd place: " << NO_COLOR << playersOnGame.at(2).getName() << " ("
             << playersOnGame.at(2).getPoints() << " points)"
             << " ! \n";
    }
    cout << LIGHTMAGENTA << "Until next time ! \n\n" << NO_COLOR;

    return 0;
}