#include "leaderboard.h"
#include <GL/freeglut_std.h>
#include <sqlite3.h>
#include <string>
#include <GL/glut.h>
#include <vector>

void Leaderboard::initShow(bool* show) {
  showScreen = show;
}

static void renderText(const std::string& text, float x, float y) {
  glRasterPos2f(x, y);
  for (char c : text) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
  }
}

Leaderboard::Leaderboard() {
  sqlite3_open("leaderboard.db", &db);

  const char* sql = 
    "CREATE TABLE IF NOT EXISTS leaderboard ("
    "name TEXT PRIMARY KEY,"
    "score INTEGER);";
  sqlite3_exec(db, sql, nullptr, nullptr, nullptr);
}

Leaderboard::~Leaderboard() {
  sqlite3_close(db);
}

std::vector<std::pair<std::string, int>> Leaderboard::getTopScores(int topN) {
  std::vector<std::pair<std::string, int>> scores;
  const char* sql = "SELECT name, score FROM leaderboard ORDER BY score DESC;";
  char* errMsg;
  auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
    if (argc == 2) {
      auto* vec = static_cast<std::vector<std::pair<std::string, int>>*>(data);
      vec->emplace_back(argv[0], std::stoi(argv[1]));
    }
    return 0;
  };

  sqlite3_exec(db, sql, callback, &scores, &errMsg);

  if (scores.size() > topN) {
    scores.resize(topN);  // Keep only the top N scores
  }

  return scores;
}

//return -1 if not found
int Leaderboard::getScore(const std::string& name) {
  std::string sql = "SELECT score FROM leaderboard WHERE name = '" + name + "';";
  char* errMsg;
  int score = -1; 
  auto callback = [](void* data, int argc, char** argv, char** azColName) -> int {
    if (argc > 0) {
      *static_cast<int*>(data) = std::stoi(argv[0]);
    }
    return 0;
  };

  sqlite3_exec(db, sql.c_str(), callback, &score, &errMsg);

  return score;
}

void renderButton(const std::string& text, int x, int y, int width, int height) {
  glColor3f(0.1, 0.1, 0.1);
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + width, y);
  glVertex2f(x + width, y + height);
  glVertex2f(x, y + height);
  glEnd();
  int textWidth = text.size() * 9;
  glColor3f(1.0, 1.0, 1.0);
  renderText(text, (x + (width - textWidth) / 2.0) - 5, y + (50 / 2.0) - 4);
}

void Leaderboard::display() {
  int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
  int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
  int buttonWidth = 100;
  int buttonHeight = 30;
  int buttonX = (windowWidth - buttonWidth) / 2;
  int buttonY = windowHeight - buttonHeight - 40;

  glClear(GL_COLOR_BUFFER_BIT);


  auto topScores = getTopScores(10);  
  int rank = 1;
  int yPos = 50;
  int xPos = (windowWidth / 2) - 50;
  for (const auto& [name, score] : topScores) {
    std::string scoreText = std::to_string(rank) + ". " + name + ": " + std::to_string(score);
    renderText(scoreText, xPos, yPos); 
    yPos += 50;
    rank++;
  }


  renderButton("Back", buttonX, buttonY, buttonWidth, buttonHeight);

  glutSwapBuffers();
  glutPostRedisplay();
}

void Leaderboard::backToMenu() {
  *(showScreen) = false;
}

void Leaderboard::mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    int buttonWidth = 100;
    int buttonHeight = 30;
    int buttonX = (windowWidth - buttonWidth) / 2;
    int buttonY = windowHeight - buttonHeight - 40; // 40 pixels from bottum

    if (x >= buttonX && x <= (buttonX + buttonWidth) &&
        y >= buttonY && y <= (buttonY + buttonHeight)) {
      backToMenu();
    }
  }
}
