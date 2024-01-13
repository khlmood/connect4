#ifndef LEADERBOARD_H
#define LEADERBOARD_H

#include <sqlite3.h>
#include <vector>
#include <string>

class Leaderboard {
public:
    Leaderboard();
    ~Leaderboard();
    void initShow(bool* show);
    std::vector<std::pair<std::string, int>> getTopScores(int topN);
    int getScore(const std::string& name);
    void display();
    void backToMenu();
    void mouse(int button, int state, int x, int y);

private:
    sqlite3* db;
    bool* showScreen;
};

#endif // LEADERBOARD_H
