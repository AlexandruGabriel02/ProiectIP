#include "diagrams.h"
#include <SFML/Graphics.hpp>

using namespace sf;

Text createText(Point topLeft, char s[], Font font) {
    Text text;
    text.setFont(font);
    text.setString(s);
    text.setCharacterSize(5);
    text.setLetterSpacing(0.5);
    text.setPosition(topLeft.x-text.getLocalBounds().width/2, topLeft.y);
    return text;
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

/*
Font font;

CircleShape createCircle(int x, int y, int r) {
    CircleShape circle(r);
    circle.setPosition(x-r, y-r);
    circle.setFillColor(Color(0, 0, 0, 0));
    circle.setOutlineThickness(2);
    circle.setOutlineColor(Color(255, 255, 255));
    return circle;
}*/


