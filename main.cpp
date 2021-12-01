#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include "diagrams.h"
#define WIDTH 1200
#define HEIGHT 900

using namespace std;
using namespace sf;

Font font;


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
