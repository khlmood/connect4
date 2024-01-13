#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include <functional>

struct MenuItem {
    std::string title;
    float x, y, width, height;
    std::function<void()> onClick;
};

class Menu {
public:
    Menu();
    void display();
    void reshape(int w, int h);
    void mouse(int button, int state, int x, int y);
    void addMenuItem(const std::string& title, float x, float y, float width, float height, std::function<void()> onClickFunc);
    void clearMenuItems();

private:
    std::vector<MenuItem> menuItems;
    void drawMenuItem(const MenuItem& item);
};

#endif // MENU_H
