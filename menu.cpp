#include "menu.h"
#include <GL/glut.h>

Menu::Menu() {
  // Initialize menu properties if needed
}

void Menu::display() {
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();

  for (const auto& item : menuItems) {
    drawMenuItem(item);
  }

  glutSwapBuffers();
}

void Menu::reshape(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, h, 0);
  glMatrixMode(GL_MODELVIEW);
}

void Menu::mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    for (const auto& item : menuItems) {
      if (x >= item.x && x <= item.x + item.width && y >= item.y && y <= item.y + item.height) {
        item.onClick();
      }
    }
  }
}

void Menu::addMenuItem(const std::string& title, float x, float y, float width, float height, std::function<void()> onClickFunc) {
  menuItems.push_back({title, x, y, width, height, onClickFunc});
}

void Menu::drawMenuItem(const MenuItem& item) {
  // Draw button background
  glColor3f(0.1, 0.1, 0.1); // Dark background for button
  glBegin(GL_QUADS);
  glVertex2f(item.x, item.y);
  glVertex2f(item.x + item.width, item.y);
  glVertex2f(item.x + item.width, item.y + item.height);
  glVertex2f(item.x, item.y + item.height);
  glEnd();

  // Draw button text
  glColor3f(1.0, 1.0, 1.0); // White color for text
  glRasterPos2f(item.x + (item.width / 2) - (item.title.size() * 4.5), item.y + 35);
  for (char c : item.title) {
    glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
  }
}

void Menu::clearMenuItems() {
    menuItems.clear();  // Clears all elements from the vector
}
