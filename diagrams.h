#pragma once
#include <SFML/Graphics.hpp>

using namespace sf;

VertexArray definitionsOrActionsCreate(int x, int y, int sx, int sy);

VertexArray decisionCreate(int x, int y, int sx, int sy);

// same like definitionsOrActions
VertexArray singleStepCreate(int x, int y, int sx, int sy);

VertexArray iterationWCreate(int x, int y, int sx, int sy, int l);

VertexArray iterationUCreate(int x, int y, int sx, int sy, int l);

