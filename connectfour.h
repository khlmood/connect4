#ifndef CONNECTFOURGAME_H
#define CONNECTFOURGAME_H

#include <vector>
#include <string>
#include <sqlite3.h>

class ConnectFourGame {
  public:
    ConnectFourGame();
    void setDatabase(sqlite3* dbConnection);
    void startMultiplayerGame(const std::string& player1, const std::string& player2);
    void startSingleplayerGame(const std::string& player);
    void initShow(bool* show, bool* state);
    void display();
    void mouse(int button, int state, int x, int y);
    void keyboardFunc(unsigned char key, int x, int y);
    int chooseBotMove();
    void makeMoveML(int col, int player);
    void undoMoveML(int col);
    int minimaxML(int depth, bool isMaximizingPlayer, int alpha, int beta);
    bool isMoveLegalML(int col);
    int evaluateBoardML();
    bool checkWinML(int col, int row, int player);
    void makeMove(int column);
    void incrementWinnerScore(const std::string& player);
    void displayMenu();  
    void restartGame();
    void exitGame();


  private:
    std::vector<std::vector<int>> board;
    int currentPlayer;
    int winner;
    std::string player1Name;
    std::string player2Name;
    bool singlePlayer;
    sqlite3* db;
    bool* showScreen;
    bool* gameState;

    void checkForWin();
};

#endif // CONNECTFOURGAME_H
