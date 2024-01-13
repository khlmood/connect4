#include "multiplayer.h"
#include "connectfour.h"
#include <GL/freeglut_std.h>
#include <GL/glut.h>
#include <iostream>
#include <sqlite3.h>

static void renderText(const std::string& text, float x, float y) {
  glRasterPos2f(x, y);
  for (char c : text) {
    glutBitmapCharacter(GLUT_BITMAP_9_BY_15, c);
  }
}

static int callback(void* data, int argc, char** argv, char** azColName) {
  Multiplayer* obj = static_cast<Multiplayer*>(data);
  if (argc > 0) {
    int score = std::stoi(argv[0]);
    if (obj->currentPlayerName == obj->player1Name) {
      obj->player1Score = score;
    } else if (obj->currentPlayerName == obj->player2Name) {
      obj->player2Score = score;
    }
    obj->playerExists = true; 
  }
  return 0;
}

Multiplayer::Multiplayer() : gameStarted(false) { 
  openDatabase(); 
  game.setDatabase(db); 
}

Multiplayer::~Multiplayer() {
  closeDatabase();
}

void Multiplayer::openDatabase() {
  int rc = sqlite3_open("leaderboard.db", &db);
  if (rc) {
    std::cerr << "Error opening SQLite3 database: " << sqlite3_errmsg(db) << std::endl;
    sqlite3_close(db);
  }
}

void Multiplayer::closeDatabase() {
  sqlite3_close(db);
}

void Multiplayer::initShow(bool* show) {
  showScreen = show;
}

void Multiplayer::display() {
  if (gameStarted) {
    game.display();
  } else {
    glClear(GL_COLOR_BUFFER_BIT);
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    float inputBoxWidth = 200.0f;
    float inputBoxHeight = 30.0f;
    float startX = (windowWidth - inputBoxWidth) / 2.0f;

    float startY = (windowHeight / 3.0f) - inputBoxHeight;

    // Calculate positions for labels
    float labelYOffset = 20.0f; 
    float player1LabelY = startY - labelYOffset;
    float player2LabelY = startY + 40 + inputBoxHeight;

    // Render labels and input boxes
    glColor3f(1.0f, 1.0f, 1.0f); 
    renderText("Player 1:", startX, player1LabelY);
    renderInputBox(inputPlayer1, startX, startY, inputBoxWidth, inputBoxHeight, isPlayer1Active);

    glColor3f(1.0f, 1.0f, 1.0f); 
    renderText("Player 2:", startX, player2LabelY);
    renderInputBox(inputPlayer2, startX, startY + 60 + inputBoxHeight, inputBoxWidth, inputBoxHeight, isPlayer2Active);

    // Render the start / back button
    float buttonWidth = 100.0f;
    float buttonHeight = 30.0f;
    float buttonSpacing = 30.0f; 
    renderButton("Start", startX, startY + 100 + 2 * inputBoxHeight, buttonWidth, buttonHeight);
    renderButton("Back", startX, startY + 100 + 2 * inputBoxHeight + buttonHeight + buttonSpacing, buttonWidth, buttonHeight);
    glutSwapBuffers();
  }
}

void Multiplayer::renderInputBox(const std::string& text, float x, float y, float width, float height, bool isActive) {
  if (isActive) {
    glColor3f(1.0f, 1.0f, 1.0f);  
  } else {
    glColor3f(0.7f, 0.7f, 0.7f); 
  }

  // Draw the box
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + width, y);
  glVertex2f(x + width, y + height);
  glVertex2f(x, y + height);
  glEnd();

  // Set text color
  glColor3f(0.0f, 0.0f, 0.0f); 

  float textX = x + 5; // 5 pixels padding from the left
  float textY = y + (height / 2) + 5; // Adjust textY based on your font size
  renderText(text, textX, textY);
}

void Multiplayer::renderButton(const std::string& text, float x, float y, float width, float height) {
  glColor3f(0.3f, 0.3f, 0.3f); 
  glBegin(GL_QUADS);
  glVertex2f(x, y);
  glVertex2f(x + width, y);
  glVertex2f(x + width, y + height);
  glVertex2f(x, y + height);
  glEnd();
  glColor3f(1.0f, 1.0f, 1.0f);

  float textWidth = text.size() * 9; 
  float textX = x + (width - textWidth) / 2;
  float textY = y + (height / 2) + 5; 

  renderText(text, textX, textY);
}

void Multiplayer::mouse(int button, int state, int x, int y) {
  if(gameStarted) {
    game.mouse(button, state, x, y);
  } else {
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      float inputBoxWidth = 200.0f;
      float inputBoxHeight = 30.0f;
      float startX = (windowWidth - inputBoxWidth) / 2.0f;
      float startY = (windowHeight / 3.0f) - inputBoxHeight;

      float inputBox2Y = startY + 60 + inputBoxHeight;
      float startButtonY = startY + 100 + 2 * inputBoxHeight;
      float backButtonY = startButtonY + 30 + inputBoxHeight; // 30 pixels space + height of the start button
      float buttonWidth = 100.0f;
      float buttonHeight = 30.0f;

      // Check for input box 1
      if (x >= startX && x <= startX + inputBoxWidth &&
          y >= startY && y <= startY + inputBoxHeight) {
        isPlayer1Active = true;
        isPlayer2Active = false;
      }
      // Check for input box 2
      else if (x >= startX && x <= startX + inputBoxWidth &&
          y >= inputBox2Y && y <= inputBox2Y + inputBoxHeight) {
        isPlayer1Active = false;
        isPlayer2Active = true;
      }
      // Check for start button
      else if (x >= startX && x <= startX + buttonWidth &&
          y >= startButtonY && y <= startButtonY + buttonHeight) {
        // Start the game
        if (!(inputPlayer1 == "") && !(inputPlayer2 == "")) {
          startGame(inputPlayer1, inputPlayer2);
        }
      }
      // Check for back button
      else if (x >= startX && x <= startX + buttonWidth &&
          y >= backButtonY && y <= backButtonY + buttonHeight) {
        *(showScreen) = false;
      }
      else {
        isPlayer1Active = false;
        isPlayer2Active = false;
      }
    }
  }
}

void Multiplayer::keyboardFunc(unsigned char key, int x, int y) {
  if(gameStarted) {
    game.keyboardFunc(key, x, y);
  } else {
    std::string* activeInput = nullptr;
    if (isPlayer1Active) {
      activeInput = &inputPlayer1;
    } else if (isPlayer2Active) {
      activeInput = &inputPlayer2;
    }

    if (activeInput) {
      if (key == 8 || key == 127) { // Backspace or Delete key
        if (!activeInput->empty()) {
          activeInput->pop_back();
        }
      } else if (key >= 32 && key <= 126) { // Standard ASCII characters
        activeInput->push_back(key);
      }
    }
  }
}

void Multiplayer::checkDatabaseForPlayer(const std::string& playerName) {
  currentPlayerName = playerName;
  std::string selectSql = "SELECT score FROM leaderboard WHERE name = '" + playerName + "'";
  char* errMsg = nullptr;
  playerExists = false; 

  // Try to select the player
  if (sqlite3_exec(db, selectSql.c_str(), callback, this, &errMsg) != SQLITE_OK) {
    std::cerr << "SQLite error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
  }

  // If player does not exist, insert them
  if (!playerExists) {
    std::string insertSql = "INSERT INTO leaderboard (name, score) VALUES ('" + playerName + "', 0)";
    if (sqlite3_exec(db, insertSql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
      std::cerr << "SQLite error: " << errMsg << std::endl;
      sqlite3_free(errMsg);
    }
  }
}

void Multiplayer::startGame(const std::string& player1, const std::string& player2) {
  player1Name = player1;
  player2Name = player2;

  checkDatabaseForPlayer(player1Name);
  checkDatabaseForPlayer(player2Name);

  // Initialize Connect Four game logic
  game.startMultiplayerGame(player1Name, player2Name);
  game.initShow(showScreen, &gameStarted);
  gameStarted = true;
}
