#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

VertexArray definitionsOrActionsCreate(float x, float y, float sx, float sy);

VertexArray decisionCreate(float x, float y, float sx, float sy);

// same like definitionsOrActions
VertexArray singleStepCreate(float x, float y, float sx, float sy);

VertexArray iterationWCreate(float x, float y, float sx, float sy, float l, float h);

VertexArray iterationUCreate(float x, float y, float sx, float sy, float l);

