#ifndef BOARD_HPP
#define BOARD_HPP

#include "SnakeBody.hpp"

#include <vector>
#include <random>

//2D board used to snake movement and generating food
//It has fixed size of 48x27 and snake/food size depends on resolution
//For 1920x1080 it's 40x40

class Board
{
    public:
        enum Direction { UP = 0, DOWN = 1, LEFT = 2, RIGHT = 3 };
        std::vector<SnakeBody> snake;
        int foodX, foodY; 

        Board();

        void setDirection(Direction dir);
        void generateFood();
        void addBody();
        bool moveSnake();
        bool gotFood();
        bool foodOnSnake();

    private:
        std::mt19937 randomEngine;
        Direction snakeDirection;

        int getRandomNumber(int min, int max);
    
};

#endif
