#include "ReactangleShape.hpp"

ReactangleShape::ReactangleShape()
{
    width = 1;
    height = 1;
    x = 0;
    y = 0;
    r = 1.0f;
    g = 0.0f;
    b = 0.0f;
}

void ReactangleShape::setSize(float width, float height)
{
    this->width = width;
    this->height = height;
}

void ReactangleShape::setPosition(float x, float y)
{
    this->x = x;
    this->y = y;
}

void ReactangleShape::setColor(int r, int g, int b)
{
    this->r = (float)r / 255.0f;
    this->g = (float)g / 255.0f;
    this->b = (float)b / 255.0f;
}
