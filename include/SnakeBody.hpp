#ifndef SNAKEBODY_HPP
#define SNAKEBODY_HPP

#include <vector>

//Defines part of snake body
//Each part has position (index on game board) and color (RGB)
class SnakeBody
{
    public:
        int positionX, positionY;
        int colorR, colorG, colorB;

        void setPosition(int x, int y);
        void setColor(int r, int g, int b);
};

#endif
