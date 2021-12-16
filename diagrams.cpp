#include "diagrams.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

using namespace std;
using namespace sf;

Text createText(Box box, string str, Font &font) {
    Text text;
    text.setFont(font);
    text.setString(str);
    float st = 1, dr = 450;
    while(st <= dr) {
        float mij = (st+dr)/2;
        text.setCharacterSize(mij);
        if(text.getLocalBounds().width <= box.length && text.getLocalBounds().height <= box.height)
            st = mij+1;
        else
            dr = mij-1;
    }
    text.setCharacterSize(st);
    text.setLetterSpacing(0.5);
    text.setPosition(box.x+(box.length-text.getLocalBounds().width)/2-5, box.y+(box.height-text.getLocalBounds().height)/2);
    return text;
}

CircleShape createCircle(Point middle, float r, Color colorFill, Color colorLine, Font &font) {
    CircleShape circle(r);
    circle.setPosition(middle.x-r, middle.y-r);
    circle.setFillColor(colorFill);
    circle.setOutlineThickness(1);
    circle.setOutlineColor(colorLine);
    return circle;
}

RectangleShape createRect(Point topLeft, Point bottomRight, Color colorFill, Color colorLine) {
    RectangleShape rectangle;
    rectangle.setSize(Vector2f(bottomRight.x-topLeft.x, bottomRight.y-topLeft.y));
    rectangle.setFillColor(colorFill);
    rectangle.setOutlineColor(colorLine);
    rectangle.setOutlineThickness(1);
    rectangle.setPosition(topLeft.x, topLeft.y);
    return rectangle;
}

VertexArray definitionsOrActionsCreate(Point topLeft, Point bottomRight) {
    VertexArray lines(LineStrip, 5);
    lines[0].position = Vector2f(topLeft.x, topLeft.y);
    lines[1].position = Vector2f(topLeft.x, bottomRight.y);
    lines[2].position = Vector2f(bottomRight.x, bottomRight.y);
    lines[3].position = Vector2f(bottomRight.x, topLeft.y);
    lines[4].position = Vector2f(topLeft.x, topLeft.y);
    return lines;
}


VertexArray decisionCreate(Point topLeft, Point bottomRight) {
    VertexArray lines(LineStrip, 8);
    lines[0].position = Vector2f(topLeft.x, topLeft.y);
    lines[1].position = Vector2f(topLeft.x, bottomRight.y);
    lines[2].position = Vector2f(topLeft.x+(bottomRight.x-topLeft.x)/2, bottomRight.y);
    lines[3].position = Vector2f(topLeft.x, topLeft.y);
    lines[4].position = Vector2f(bottomRight.x, topLeft.y);
    lines[5].position = Vector2f(topLeft.x+(bottomRight.x-topLeft.x)/2, bottomRight.y);
    lines[6].position = Vector2f(bottomRight.x, bottomRight.y);
    lines[7].position = Vector2f(bottomRight.x, topLeft.y);
    return lines;
}


// same like definitionsOrActions
VertexArray singleStepCreate(Point topLeft, Point bottomRight) {
    VertexArray lines(LineStrip, 5);
    lines[0].position = Vector2f(topLeft.x, topLeft.y);
    lines[1].position = Vector2f(topLeft.x, bottomRight.y);
    lines[2].position = Vector2f(bottomRight.x, bottomRight.y);
    lines[3].position = Vector2f(bottomRight.x, topLeft.y);
    lines[4].position = Vector2f(topLeft.x, topLeft.y);
    return lines;
}


VertexArray iterationWCreate(Point topLeft, Point bottomRight, float l, float h) {
    VertexArray lines(LineStrip, 7);
    lines[0].position = Vector2f(topLeft.x, topLeft.y);
    lines[1].position = Vector2f(bottomRight.x, topLeft.y);
    lines[2].position = Vector2f(bottomRight.x, topLeft.y+h);
    lines[3].position = Vector2f(topLeft.x+l, topLeft.y+h);
    lines[4].position = Vector2f(topLeft.x+l, bottomRight.y);
    lines[5].position = Vector2f(topLeft.x, bottomRight.y);
    lines[6].position = Vector2f(topLeft.x, topLeft.y);
    return lines;
}


VertexArray iterationUCreate(Point topLeft, Point bottomRight, float l) {
    VertexArray lines(LineStrip, 7);
    lines[0].position = Vector2f(topLeft.x, topLeft.y);
    lines[1].position = Vector2f(topLeft.x+l, topLeft.y);
    lines[2].position = Vector2f(topLeft.x+l, bottomRight.y-l);
    lines[3].position = Vector2f(bottomRight.x, bottomRight.y-l);
    lines[4].position = Vector2f(bottomRight.x, bottomRight.y);
    lines[5].position = Vector2f(topLeft.x, bottomRight.y);
    lines[6].position = Vector2f(topLeft.x, topLeft.y);
    return lines;
}


