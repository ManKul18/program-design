#include <iostream>
#include <vector>
#include <string>

using namespace std;

enum class GameStatus
{
    IN_PROGRESS,
    X_WON,
    O_WON,
    DRAW
};

enum class PlayerIndex
{
    ONE,
    TWO,
    INVALID
};

class Move
{
private:
    int m_x, m_y;
    char m_symbol;

public:
    Move(int x, int y, int piece) : m_x(x), m_y(y), m_symbol(piece) {}
    int getX() const { return m_x; }
    int getY() const { return m_y; }
    char getSymbol() const { return m_symbol; }
};

class Board
{
private:
    vector<vector<char>> m_board;
    int m_size;

public:
    Board(int sz) : m_size(sz)
    {
        m_board.resize(m_size);
        for (int i = 0; i < m_size; i++)
        {
            for (int j = 0; j < m_size; j++)
            {
                m_board[i].push_back('.');
            }
        }
    }
    int getSize() const
    {
        return m_size;
    }
    char getSymbol(int x, int y) const
    {
        return m_board[x][y];
    }
    bool setSymbol(Move &move)
    {
        int x = move.getX(), y = move.getY();
        if (m_board[x][y] == '.')
        {
            m_board[x][y] = move.getSymbol();
            return true;
        }
        return false;
    }
    bool isFull() const
    {
        for (int i = 0; i < m_size; i++)
        {
            for (int j = 0; j < m_size; j++)
            {
                if (m_board[i][j] == '.')
                    return false;
            }
        }
        return true;
    }
    virtual void display()
    {
        for (int i = 0; i < m_size; i++)
        {
            cout << "\t\t\t";
            for (int j = 0; j < m_size; j++)
            {
                cout << " " << m_board[i][j];
            }
            cout << endl;
        }
    }
};

class Player
{
protected:
    char m_symbol;
    string m_name;

public:
    Player(string name, char sym) : m_name(name), m_symbol(sym) {}
    string getName() const { return m_name; }
    char getSymbol() const { return m_symbol; }
    virtual void chooseMove(Board &board) = 0;
    virtual ~Player(){};
};

class HumanPlayer : public Player
{
public:
    HumanPlayer(string name, char symbol) : Player(name, symbol) {}

    void chooseMove(Board &brd) override
    {
        int x, y;
        do
        {
            cout << m_name << " turn, (x,y): ";
            cin >> x >> y;
        } while (!brd.setSymbol(Move(x, y, m_symbol)));
    }
};

class CompterPlayer : public Player
{
private:
public:
    CompterPlayer(string name, char symbol) : Player(name, symbol) {}
    void chooseMove(Board &brd) override
    {
        int i = 0, j = 0;
        cout << m_name << " turn, (x,y): ";
        do
        {
            i = rand() % brd.getSize();
            j = rand() % brd.getSize();
        } while (!brd.setSymbol(Move(i, j, getSymbol())));
        cout << endl;
    }
};

class Game
{
private:
    Board m_board;
    GameStatus m_gameStatus;
    vector<Player *> m_playerList;

public:
    Game(int board_size)
        : m_board(board_size), m_gameStatus(GameStatus::IN_PROGRESS)
    {
    }
    bool addPlayer(Player *player)
    {
        if (m_playerList.size() < int(PlayerIndex::INVALID))
        {
            m_playerList.push_back(player);
            return true;
        }
        return false;
    }
    void chooseMoveFor(PlayerIndex index)
    {
        m_playerList[int(index)]->chooseMove(m_board);
    }
    GameStatus getGameStatus() const
    {
        return m_gameStatus;
    }
    string getPlayerName(PlayerIndex index) const
    {
        return m_playerList[int(index)]->getName();
    }
    char getPlayerSymbol(PlayerIndex index) const
    {
        return m_playerList[int(index)]->getSymbol();
    }
    void setGameStatus(GameStatus state)
    {
        m_gameStatus = state;
    }
    Board &getBoard()
    {
        return m_board;
    }
    void displayBoard()
    {
        m_board.display();
    }

    virtual ~Game()
    {
    }
};

class GameConsole
{
private:
    Game m_game;
    HumanPlayer *m_humanPlayer;
    CompterPlayer *m_computerPlayer;
    char getWinner(Board &board)
    {
        for (int i = 0; i < board.getSize(); i++)
        {
            // all rows
            if (board.getSymbol(i, 0) != '.' && board.getSymbol(i, 0) == board.getSymbol(i, 1) &&
                board.getSymbol(i, 1) == board.getSymbol(i, 2))
                return board.getSymbol(i, 0);

            // all cols
            if (board.getSymbol(0, i) != '.' && board.getSymbol(0, i) == board.getSymbol(1, i) &&
                board.getSymbol(1, i) == board.getSymbol(2, i))
                return board.getSymbol(0, i);
        }

        // two diags
        if (board.getSymbol(0, 0) != '.' && board.getSymbol(0, 0) == board.getSymbol(1, 1) &&
            board.getSymbol(1, 1) == board.getSymbol(2, 2))
            return board.getSymbol(0, 0);

        if (board.getSymbol(0, 2) != '.' && board.getSymbol(0, 2) == board.getSymbol(1, 1) &&
            board.getSymbol(1, 1) == board.getSymbol(2, 0))
            return board.getSymbol(0, 0);

        if (board.isFull())
            return 'F';

        return '.';
    }

    GameStatus getStatus(Board &board)
    {
        GameStatus status = GameStatus::IN_PROGRESS;
        char result = getWinner(board);

        if (result == 'X')
        {
            status = GameStatus::X_WON;
        }

        if (result == 'O')
        {
            status = GameStatus::O_WON;
        }

        if (result == 'F')
            status = GameStatus::DRAW;

        return status;
    }

public:
    GameConsole(int board_size) : m_game(board_size) {}

    bool addPlayer(Player *player)
    {
        m_game.addPlayer(player);
    }

    void play()
    {
        bool firstPlayer = true;
        GameStatus result;
        string name;

        cout << "\nEnter name for human: ";
        cin >> name;
        m_humanPlayer = new HumanPlayer(name, 'X');

        cout << "\nEnter name for computer: ";
        cin >> name;
        m_computerPlayer = new CompterPlayer(name, 'O');

        m_game.addPlayer(m_humanPlayer);
        m_game.addPlayer(m_computerPlayer);

        while ((result = getStatus(m_game.getBoard())) == GameStatus::IN_PROGRESS)
        {
            if (firstPlayer)
            {
                m_game.chooseMoveFor(PlayerIndex::ONE);
            }
            else
            {
                m_game.chooseMoveFor(PlayerIndex::TWO);
            }
            m_game.displayBoard();
            firstPlayer = firstPlayer ? false : true;
        }
        m_game.setGameStatus(result);
    }
    void printResult()
    {
        cout << "\n\t\t\t\t\t\t---->| ";
        if (m_game.getGameStatus() == GameStatus::DRAW)
            cout << "No One";
        else if (m_game.getGameStatus() == GameStatus::O_WON)
        {
            cout << m_game.getPlayerName(PlayerIndex::TWO);
        }
        else
        {
            cout << m_game.getPlayerName(PlayerIndex::ONE);
        }
        cout << " WINS |<----" << endl;
    }
    ~GameConsole()
    {
        delete m_computerPlayer;
        delete m_humanPlayer;
    }
};

int main()
{
    GameConsole gc(3);
    gc.play();
    gc.printResult();
    return 0;
}