#include "diagrams.h"
#include <SFML/Graphics.hpp>

using namespace sf;

VertexArray definitionsOrActionsCreate(int x, int y, int sx, int sy) {
    VertexArray lines(LineStrip, 5);
    lines[0].position = Vector2f(x, y);
    lines[1].position = Vector2f(x, sy);
    lines[2].position = Vector2f(sx, sy);
    lines[3].position = Vector2f(sx, y);
    lines[4].position = Vector2f(x, y);
    return lines;
}


VertexArray decisionCreate(int x, int y, int sx, int sy) {
    VertexArray lines(LineStrip, 8);
    lines[0].position = Vector2f(x, y);
    lines[1].position = Vector2f(x, sy);
    lines[2].position = Vector2f(x+(sx-x)/2, sy);
    lines[3].position = Vector2f(x, y);
    lines[4].position = Vector2f(sx, y);
    lines[5].position = Vector2f(x+(sx-x)/2, sy);
    lines[6].position = Vector2f(sx, sy);
    lines[7].position = Vector2f(sx, y);
    return lines;
}


// same like definitionsOrActions
VertexArray singleStepCreate(int x, int y, int sx, int sy) {
    VertexArray lines(LineStrip, 5);
    lines[0].position = Vector2f(x, y);
    lines[1].position = Vector2f(x, sy);
    lines[2].position = Vector2f(sx, sy);
    lines[3].position = Vector2f(sx, y);
    lines[4].position = Vector2f(x, y);
    return lines;
}


VertexArray iterationWCreate(int x, int y, int sx, int sy, int l) {
    VertexArray lines(LineStrip, 7);
    lines[0].position = Vector2f(x, y);
    lines[1].position = Vector2f(sx, y);
    lines[2].position = Vector2f(sx, y+l);
    lines[3].position = Vector2f(x+l, y+l);
    lines[4].position = Vector2f(x+l, sy);
    lines[5].position = Vector2f(x, sy);
    lines[6].position = Vector2f(x, y);
    return lines;
}


VertexArray iterationUCreate(int x, int y, int sx, int sy, int l) {
    VertexArray lines(LineStrip, 7);
    lines[0].position = Vector2f(x, y);
    lines[1].position = Vector2f(x+l, y);
    lines[2].position = Vector2f(x+l, sy-l);
    lines[3].position = Vector2f(sx, sy-l);
    lines[4].position = Vector2f(sx, sy);
    lines[5].position = Vector2f(x, sy);
    lines[6].position = Vector2f(x, y);
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
}


Text createText(int x, int y, string s) {
    Text text;
    text.setFont(font);
    text.setPosition(x, y);
    text.setString(s);
    text.setCharacterSize(30);
    text.setLetterSpacing(0.5);
    return text;
}*/
