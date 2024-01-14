#include <GL/glut.h>
#include "menu.h"
#include "leaderboard.h"
#include "multiplayer.h"
#include "singleplayer.h"

namespace SingleplayerSystem {
  Singleplayer singleplayerGame; 
  bool show = false; 

  void init() {
    singleplayerGame.initShow(&show);
  }

  void display() {
    if (show) {
      singleplayerGame.display(); 
      glutPostRedisplay();
    }
  }

  void mouse(int button, int state, int x, int y) {
    if (show) {
      singleplayerGame.mouse(button, state, x, y); 
    }
  }
}

namespace MultiplayerSystem {
  Multiplayer multiplayerGame; 
  bool show = false; 

  void init() {
    multiplayerGame.initShow(&show);
  }

  void display() {
    if (show) {
      multiplayerGame.display(); 
      glutPostRedisplay();
    }
  }

  void mouse(int button, int state, int x, int y) {
    if (show) {
      multiplayerGame.mouse(button, state, x, y); 
    }
  }
}

namespace LeaderboardSystem {
  Leaderboard leaderboard;
  bool show = false;

  void init() {
    leaderboard.initShow(&show);
  }

  void display() {
    if (show) {
      leaderboard.display(); 
      glutPostRedisplay();
    }
  }

  void mouse(int button, int state, int x, int y) {
    if (show) {
      leaderboard.mouse(button, state, x, y); 
    }
  }
}

namespace MenuSystem {
  Menu menu;

  void init(int windowWidth, int windowHeight) {
    glClearColor(0.2, 0.3, 0.3, 1.0);

    float buttonWidth = static_cast<float>(windowWidth) * 0.5f;
    float buttonHeight = static_cast<float>(windowHeight) * 0.1f;
    float startX = (static_cast<float>(windowWidth) - buttonWidth) / 2.0f;
    float startY = static_cast<float>(windowHeight) * 0.2f;
    float gap = buttonHeight * 0.5f;

    menu.clearMenuItems();
    menu.addMenuItem("Single Player", startX, startY, buttonWidth, buttonHeight, [](){ SingleplayerSystem::show=true;SingleplayerSystem::init();});
    menu.addMenuItem("Multiplayer", startX, startY + buttonHeight + gap, buttonWidth, buttonHeight, [](){ MultiplayerSystem::show = true;MultiplayerSystem::init(); });
    menu.addMenuItem("Leaderboard", startX, startY + 2 * (buttonHeight + gap), buttonWidth, buttonHeight, [](){ LeaderboardSystem::show = true;LeaderboardSystem::init(); });
    menu.addMenuItem("Exit Game", startX, startY + 3 * (buttonHeight + gap), buttonWidth, buttonHeight, [](){ exit(0); });
  }

  void display() {
    menu.display();
    glutPostRedisplay();
  }

  void reshape(int w, int h) {
    menu.reshape(w, h);
    init(w, h);
  }

  void mouse(int button, int state, int x, int y) {
    menu.mouse(button, state, x, y);
  }
}

// Main display function
void display() {
  if (LeaderboardSystem::show) {
    LeaderboardSystem::display();
  } else if (MultiplayerSystem::show) {
    MultiplayerSystem::display();
  } else if (SingleplayerSystem::show) {
    SingleplayerSystem::display();
  } else {
    MenuSystem::display();
  }
}

// Main reshape function
void reshape(int w, int h) {
  MenuSystem::reshape(w, h);
}

// Main mouse function
void mouse(int button, int state, int x, int y) {
  if (LeaderboardSystem::show) {
    LeaderboardSystem::mouse(button, state, x, y);
  } else if (MultiplayerSystem::show) {
    MultiplayerSystem::mouse(button, state, x, y);
  } else if (SingleplayerSystem::show) {
    SingleplayerSystem::mouse(button, state, x, y);
  } else {
    MenuSystem::mouse(button, state, x, y);
  }
}

//Main keyboard function
void keyboardFunc(unsigned char key, int x, int y) {
    if (MultiplayerSystem::show) {
        MultiplayerSystem::multiplayerGame.keyboardFunc(key, x, y);
    } else if (SingleplayerSystem::show) {
      SingleplayerSystem::singleplayerGame.keyboardFunc(key, x, y);
    }
}

// Main function
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

  int windowWidth = 600;
  int windowHeight = 600;

  glutInitWindowPosition(200, 100);
  glutInitWindowSize(windowWidth, windowHeight);
  glutCreateWindow("Connect4");

  MenuSystem::init(windowWidth, windowHeight);

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutKeyboardFunc(keyboardFunc);

  glutMainLoop();

  return 0;
}

