#pragma once
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
///
struct Point {
    float x, y;
};

struct Box {
    float x, y, length, height;
};

Text createText(Box box, std::string str, Font &font);

VertexArray definitionsOrActionsCreate(Point topLeft, Point bottomRight);

VertexArray decisionCreate(Point topLeft, Point bottomRight);

// same like definitionsOrActions
VertexArray singleStepCreate(Point topLeft, Point bottomRight);

VertexArray iterationWCreate(Point topLeft, Point bottomRight, float l, float h);

VertexArray iterationUCreate(Point topLeft, Point bottomRight, float l);


