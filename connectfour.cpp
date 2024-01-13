#include "connectfour.h"
#include <GL/freeglut_std.h>
#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <sqlite3.h>

#define DEPTH 10

constexpr int COLUMNS = 7;
constexpr int ROWS = 6;

static void renderText(const std::string& text, float x, float y) {
  glRasterPos2f(x, y);
  for (char c : text) {
    glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
  }
}

void drawCell(float x, float y, float width, float height) {
  const int segments = 30;  // Number of segments to approximate a circle
  float radius = std::min(width, height) / 2.0f; 
  float centerX = x + width / 2.0f;  
  float centerY = y + height / 2.0f; 

  // Begin drawing a polygon to approximate a circle
  glBegin(GL_POLYGON);
  for (int i = 0; i < segments; i++) {
    float theta = 2.0f * 3.1415926f * float(i) / float(segments); // Angle for this segment

    float dx = radius * cosf(theta); // Calculate the x component
    float dy = radius * sinf(theta); // Calculate the y component

    glVertex2f(centerX + dx, centerY + dy); // Output vertex
  }
  glEnd();
}

ConnectFourGame::ConnectFourGame() {
  // Initialize the game board to be empty
  board = std::vector<std::vector<int>>(COLUMNS, std::vector<int>(ROWS, 0));

  currentPlayer = 1;
  winner = 0;
}

void ConnectFourGame::setDatabase(sqlite3* dbConnection) {
  db = dbConnection;
}

void ConnectFourGame::startMultiplayerGame(const std::string& player1, const std::string& player2) {
  // Reset the game board to be empty
  for (auto &column : board) {
    std::fill(column.begin(), column.end(), 0);
  }
  currentPlayer = 1;
  winner = 0;
  player1Name = player1;
  player2Name = player2;
  singlePlayer = false;
}

void ConnectFourGame::startSingleplayerGame(const std::string& player) {
  // Reset the game board to be empty
  for (auto &column : board) {
    std::fill(column.begin(), column.end(), 0);
  }
  currentPlayer = 1;
  winner = 0;
  player1Name = player;
  player2Name = "Bot";
  singlePlayer = true;
}

void ConnectFourGame::initShow(bool* show, bool* state) {
  showScreen = show;
  gameState = state;
}

void ConnectFourGame::display() {
  if(winner == 0) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.2, 0.3, 0.3, 1.0);

    const float player1Color[3] = {1.0f, 0.0f, 0.0f}; // Red for player 1
    const float player2Color[3] = {1.0f, 1.0f, 0.0f}; // Yellow for player 2
    const float emptyColor[3] = {0.8f, 0.8f, 0.8f};   // Gray for empty slots
    const float boardColor[3] = {0.0f, 0.0f, 1.0f};   // Blue for the board

    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // Make the board slightly smaller than the window
    float boardWidth = windowWidth * 0.8f;
    float boardHeight = windowHeight * 0.8f;
    float cellWidth = boardWidth / board.size();
    float cellHeight = boardHeight / board[0].size();

    // Calculate the starting position of the board
    float boardStartX = (windowWidth - boardWidth) / 2.0f;
    float boardStartY = (windowHeight - boardHeight) / 2.0f;

    // Render the blue board background
    glColor3fv(boardColor);
    glBegin(GL_QUADS);
    glVertex2f(boardStartX, boardStartY);
    glVertex2f(boardStartX + boardWidth, boardStartY);
    glVertex2f(boardStartX + boardWidth, boardStartY + boardHeight);
    glVertex2f(boardStartX, boardStartY + boardHeight);
    glEnd();

    // Display player names with color indication for whose turn it is
    glColor3fv(currentPlayer == 1 ? player1Color : emptyColor);
    renderText(player1Name, 50, 30); // Top left

    glColor3fv(currentPlayer == 2 ? player2Color : emptyColor);
    renderText(player2Name, windowWidth - 110, 30); // Top right

    // Display the game board
    for (int col = 0; col < board.size(); ++col) {
      for (int row = 0; row < board[col].size(); ++row) {
        float x = boardStartX + col * cellWidth;
        float y = boardStartY + row * cellHeight;

        // Choose color based on cell state
        const float* color = emptyColor;
        if (board[col][row] == 1) {
          color = player1Color;
        } else if (board[col][row] == 2) {
          color = player2Color;
        }

        // Draw the cell
        glColor3fv(color);
        drawCell(x, y, cellWidth, cellHeight);
      }
    }
  } else {
    displayMenu();
  }
  glutSwapBuffers();
}

void ConnectFourGame::mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && winner == 0) {
    int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
    int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);

    // Board dimensions and position
    float boardWidth = windowWidth * 0.8f;
    float boardHeight = windowHeight * 0.8f;
    float boardStartX = (windowWidth - boardWidth) / 2.0f;
    float boardStartY = (windowHeight - boardHeight) / 2.0f;

    // Check if the click is within the board's bounds
    if (x >= boardStartX && x <= (boardStartX + boardWidth) &&
        y >= boardStartY && y <= (boardStartY + boardHeight)) {

      float columnWidth = boardWidth / static_cast<float>(board.size());
      // Calculate which column was clicked, adjusting for the board's starting position
      int columnClicked = static_cast<int>((x - boardStartX) / columnWidth);

      if (columnClicked >= 0 && columnClicked < board.size()) {
        makeMove(columnClicked);

        if (singlePlayer && currentPlayer == 2) {
          int botColumn = chooseBotMove();
          makeMove(botColumn);
        }
      }
    }

    if(winner == 1) {
      incrementWinnerScore(player1Name);
    } else if (winner == 2 && !singlePlayer) {
      incrementWinnerScore(player2Name);
    }
  }
}

void ConnectFourGame::keyboardFunc(unsigned char key, int x, int y) {
  if (winner == 0)
    return;
  switch (key) {
    case 'r':
    case 'R':
      // Restart the game
      restartGame();
      break;
    case 'e':
    case 'E':
      // Exit the game
      exitGame();
      break;
    default:
      break;
  }
}

bool ConnectFourGame::checkWinML(int col, int row, int player) {
  // Check horizontally
  if (col + 3 < COLUMNS &&
      board[col][row] == player && board[col+1][row] == player &&
      board[col+2][row] == player && board[col+3][row] == player) {
    return true;
  }
  // Check vertically
  if (row + 3 < ROWS &&
      board[col][row] == player && board[col][row+1] == player &&
      board[col][row+2] == player && board[col][row+3] == player) {
    return true;
  }
  // Check diagonal (down-right)
  if (row + 3 < ROWS && col + 3 < COLUMNS &&
      board[col][row] == player && board[col+1][row+1] == player &&
      board[col+2][row+2] == player && board[col+3][row+3] == player) {
    return true;
  }
  // Check diagonal (up-right)
  if (row - 3 >= 0 && col + 3 < COLUMNS &&
      board[col][row] == player && board[col+1][row-1] == player &&
      board[col+2][row-2] == player && board[col+3][row-3] == player) {
    return true;
  }

  return false;
}

bool ConnectFourGame::isMoveLegalML(int column) {
  return board[column][0] == 0;
}

void ConnectFourGame::makeMoveML(int column, int player) {
  // Start from the bottom of the column and find the first empty cell
  for (int row = ROWS - 1; row >= 0; --row) {
    if (board[column][row] == 0) {
      board[column][row] = player;
      break;
    }
  }
}

void ConnectFourGame::undoMoveML(int column) {
  // Start from the top of the column and find the first non-empty cell
  for (int row = 0; row < ROWS; ++row) {
    if (board[column][row] != 0) {
      board[column][row] = 0; // Set the cell to empty
      break;
    }
  }
}

int ConnectFourGame::evaluateBoardML() {
  // Check for a win for either player
  for (int col = 0; col < COLUMNS; ++col) {
    for (int row = 0; row < ROWS; ++row) {
      // Check horizontal, vertical, and diagonal win conditions
      if (checkWinML(col, row, 1)) {
        return -1000; // Player 1 (human) wins
      } else if (checkWinML(col, row, 2)) {
        return 1000;  // Player 2 (bot) wins
      }
    }
  }
  // Game is still undecided or a draw
  return 0;
}

int ConnectFourGame::minimaxML(int depth, bool isMaximizingPlayer, int alpha, int beta) {
  int boardVal = evaluateBoardML();

  if (depth == 0 || boardVal == 1000 || boardVal == -1000) {
    return boardVal;
  }

  if (isMaximizingPlayer) {
    int maxEval = -1000000; // Negative infinity
    for (int col = 0; col < COLUMNS; ++col) {
      if (isMoveLegalML(col)) {
        makeMoveML(col, 2); // Assuming 2 is the Bot
        int eval = minimaxML(depth - 1, false, alpha, beta);
        undoMoveML(col); // Undo the move
        maxEval = std::max(maxEval, eval);
        alpha = std::max(alpha, eval);
        if (beta <= alpha)
          break;
      }
    }
    return maxEval;
  } else {
    int minEval = 1000000; // Positive infinity
    for (int col = 0; col < COLUMNS; ++col) {
      if (isMoveLegalML(col)) {
        makeMoveML(col, 1); // Assuming 1 is the Human player
        int eval = minimaxML(depth - 1, true, alpha, beta);
        undoMoveML(col); // Undo the move
        minEval = std::min(minEval, eval);
        beta = std::min(beta, eval);
        if (beta <= alpha)
          break;
      }
    }
    return minEval;
  }
}

int ConnectFourGame::chooseBotMove() {
  int bestScore = -1000000;
  int bestMove = 0;
  for (int col = 0; col < COLUMNS; ++col) {
    if (isMoveLegalML(col)) {
      makeMoveML(col, 2); 
      int score = minimaxML(DEPTH, false, -1000000, 1000000); // Depth adjustable
      undoMoveML(col); // Undo the move
      if (score > bestScore) {
        bestScore = score;
        bestMove = col;
      }
    }
  }
  return bestMove;
}

void ConnectFourGame::makeMove(int column) {
  // Check if the column is valid and has empty slots
  if (column < 0 || column >= board.size() || board[column][0] != 0) {
    return; // Invalid move
  }
  // Find the first empty slot in the column from the bottom up
  int row = board[column].size() - 1;
  while (row >= 0 && board[column][row] != 0) {
    row--;
  }
  if (row >= 0) {
    // Place the player's piece in the empty slot
    board[column][row] = currentPlayer;
    // Check for a win or a draw
    checkForWin();
    if (winner == 0) {
      currentPlayer = (currentPlayer == 1) ? 2 : 1;
    }
  }
}

void ConnectFourGame::incrementWinnerScore(const std::string& player) {
  if (player == "Bot")
    return;
  std::string updateSql = "UPDATE leaderboard SET score = score + 1 WHERE name = '" + player + "'";
  char* errMsg = nullptr;

  if (sqlite3_exec(db, updateSql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
    std::cerr << "SQLite error: " << errMsg << std::endl;
    sqlite3_free(errMsg);
  }
}

void ConnectFourGame::displayMenu() {
  std::string message;
  if (winner > 0) {
    message = "Winner: " + (winner == 1 ? player1Name : player2Name);
  } else if (winner == -1) {
    message = "Game Drawn";
  }

  int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
  int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
  float textX = windowWidth / 2.0 - 50;
  float textY = windowHeight / 2.0;
  float bgWidth = 230.0f; 
  float bgHeight = 120.0f; 
  float bgX = textX - 20; 
  float bgY = textY - 30;

  // Render the background rectangle
  glColor3f(0.2f, 0.3f, 0.3f); 
  glBegin(GL_QUADS);
  glVertex2f(bgX, bgY);
  glVertex2f(bgX + bgWidth, bgY);
  glVertex2f(bgX + bgWidth, bgY + bgHeight);
  glVertex2f(bgX, bgY + bgHeight);
  glEnd();

  // Render the winner message
  glColor3f(1.0f, 1.0f, 1.0f); // White color for text
  renderText(message, textX, textY);

  // Render options
  renderText("Press R to Restart", textX, textY + 30);
  renderText("Press E to Exit", textX, textY + 60);
}

void ConnectFourGame::checkForWin() {
  for (int col = 0; col < board.size(); ++col) {
    for (int row = 0; row < board[col].size(); ++row) {
      int currentPiece = board[col][row];
      if (currentPiece == 0) continue; // Skip empty cells

      // Check horizontally
      if (col + 3 < board.size() &&
          currentPiece == board[col + 1][row] &&
          currentPiece == board[col + 2][row] &&
          currentPiece == board[col + 3][row]) {
        winner = currentPiece;
        return;
      }

      // Check vertically
      if (row + 3 < board[col].size() &&
          currentPiece == board[col][row + 1] &&
          currentPiece == board[col][row + 2] &&
          currentPiece == board[col][row + 3]) {
        winner = currentPiece;
        return;
      }

      // Check diagonal (down-right)
      if (col + 3 < board.size() && row + 3 < board[col].size() &&
          currentPiece == board[col + 1][row + 1] &&
          currentPiece == board[col + 2][row + 2] &&
          currentPiece == board[col + 3][row + 3]) {
        winner = currentPiece;
        return;
      }

      // Check diagonal (up-right)
      if (col + 3 < board.size() && row - 3 >= 0 &&
          currentPiece == board[col + 1][row - 1] &&
          currentPiece == board[col + 2][row - 2] &&
          currentPiece == board[col + 3][row - 3]) {
        winner = currentPiece;
        return;
      }
    }
  }

  // Check for a draw
  bool isDraw = true;
  for (const auto& column : board) {
    for (int cell : column) {
      if (cell == 0) { // If any cell is empty, it's not a draw
        isDraw = false;
        break;
      }
    }
    if (!isDraw) {
      break;
    }
  }

  if (isDraw && winner == 0) { 
    winner = -1;
  }
}

void ConnectFourGame::restartGame() {
  for (auto &column : board) {
    std::fill(column.begin(), column.end(), 0);
  }
  currentPlayer = 1;
  winner = 0;
}

void ConnectFourGame::exitGame() {
  restartGame();
  *(showScreen) = false;
  *(gameState) = false;
}
