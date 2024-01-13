#ifndef MULTIPLAYER_H
#define MULTIPLAYER_H

#include <string>
#include <sqlite3.h>
#include "connectfour.h"

static int callback(void* data, int argc, char** argv, char** azColName);

class Multiplayer {
  public:
    Multiplayer();
    ~Multiplayer();
    void display();
    void initShow(bool* show);
    void mouse(int button, int state, int x, int y);
    void startGame(const std::string& player1, const std::string& player2);
    void checkDatabaseForPlayer(const std::string& playerName); 
    void keyboardFunc(unsigned char key, int x, int y);

  private:
    ConnectFourGame game;
    bool* showScreen;
    std::string player1Name;
    std::string player2Name;
    std::string inputPlayer1;
    std::string inputPlayer2;
    std::string currentPlayerName;
    int player1Score;
    int player2Score;
    bool isPlayer1Active;
    bool isPlayer2Active;
    bool playerExists;
    sqlite3* db;
    bool gameStarted;

    void openDatabase();
    void closeDatabase();
    void renderInputBox(const std::string& text, float x, float y, float width, float height, bool isActive);
    void renderButton(const std::string& text, float x, float y, float width, float height);
    friend int callback(void* data, int argc, char** argv, char** azColName);
};

#endif // MULTIPLAYER_H
