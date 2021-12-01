#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include "diagrams.h"
#define WIDTH 1200
#define HEIGHT 900

using namespace std;
using namespace sf;

Font font;

CircleShape createCircle(int x, int y, int r) {
    CircleShape circle(r);
    circle.setPosition(x-r, y-r);
    circle.setFillColor(Color(0, 0, 0, 0));
    circle.setOutlineThickness(2);
    circle.setOutlineColor(Color(255, 255, 255));
    return circle;
}


Text createText(int x, int y, string s) {
    Text text;
    text.setFont(font);
    text.setPosition(x, y);
    text.setString(s);
    text.setCharacterSize(30);
    text.setLetterSpacing(0.5);
    return text;
}

void pollEvents(RenderWindow* window)
{
    Event event;
    while (window -> pollEvent(event)) {
        if (event.type == Event::Closed)
            window -> close();
        if (event.type == Event::KeyPressed)
        {
            if (event.key.code == Keyboard::Escape)
                window -> close();
        }
    }
}

vector <VertexArray> shapes;
void updateWindow(RenderWindow* window)
{
    window -> clear();
    for(int i = 0; i < shapes.size(); i++)
        window -> draw(shapes[i]);
    window -> display();
}

int main() {
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "NS Diagram");

    if(!font.loadFromFile("./font.ttf")) {
        cout << "Not found file";
        exit(0);
    }

    shapes.push_back(definitionsOrActionsCreate(20, 20, 100, 40));
    shapes.push_back(decisionCreate(20, 60, 100, 80));
    shapes.push_back(iterationWCreate(20, 100, 100, 140, 15));
    shapes.push_back(iterationUCreate(20, 160, 100, 200, 15));
    cout << shapes[shapes.size()-1].getBounds().top << ' ' << shapes[shapes.size()-1].getBounds().height << '\n';

    while(window.isOpen()) {

        pollEvents(&window);
        updateWindow(&window);
    }

    return 0;
}
