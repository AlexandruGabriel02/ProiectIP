#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

Font font;

struct Point {
    float x, y;
};

Text createText(Point topLeft, char s[], Font font);

VertexArray definitionsOrActionsCreate(Point topLeft, Point bottomRight);

VertexArray decisionCreate(Point topLeft, Point bottomRight);

// same like definitionsOrActions
VertexArray singleStepCreate(Point topLeft, Point bottomRight);

VertexArray iterationWCreate(Point topLeft, Point bottomRight, float l, float h);

VertexArray iterationUCreate(Point topLeft, Point bottomRight, float l);


