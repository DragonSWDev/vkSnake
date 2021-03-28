#include "Board.hpp"

#include <chrono>

Board::Board()
{
    unsigned timeSeed = std::chrono::system_clock::now().time_since_epoch().count();
    randomEngine.seed(timeSeed);

    SnakeBody snakeHead;

    snakeHead.setPosition(23, 12);
    snakeHead.setColor(getRandomNumber(32, 255), getRandomNumber(32, 255), getRandomNumber(32, 255));

    snake.push_back(snakeHead);

    SnakeBody body;

    body.setPosition(23, 13);
    body.setColor(getRandomNumber(32, 255), getRandomNumber(32, 255), getRandomNumber(32, 255));

    snake.push_back(body);

    snakeDirection = Direction::UP;
}

void Board::setDirection(Direction dir)
{
    if (snakeDirection == Direction::LEFT && dir == Direction::RIGHT)
    {
        return;
    }

    if (snakeDirection == Direction::RIGHT && dir == Direction::LEFT)
    {
        return;
    }

    if (snakeDirection == Direction::UP && dir == Direction::DOWN)
    {
        return;
    }

    if (snakeDirection == Direction::DOWN && dir == Direction::UP)
    {
        return;
    }

    snakeDirection = dir;
}

void Board::generateFood()
{
    foodX = getRandomNumber(0, 47);
    foodY = getRandomNumber(0, 26);

    while (foodOnSnake())
    {
        foodX = getRandomNumber(0, 47);
        foodY = getRandomNumber(0, 26);
    }
}

void Board::addBody()
{
    SnakeBody lastBody = snake[0];

    SnakeBody newBody;
    newBody.setPosition(lastBody.positionX, lastBody.positionY);
    newBody.setColor(getRandomNumber(32, 255), getRandomNumber(32, 255), getRandomNumber(32, 255));

    snake.insert(snake.begin(), newBody);
}

bool Board::moveSnake()
{
    SnakeBody lastBody = snake[0];
    snake.erase(snake.begin());

    lastBody.setPosition(snake[snake.size() - 1].positionX, snake[snake.size() - 1].positionY);

    switch(snakeDirection)
    {
        case Direction::UP:
            lastBody.positionY--;

            if (lastBody.positionY < 0)
            {
                lastBody.positionY = 26;
            }

            break;

        case Direction::DOWN:
            lastBody.positionY++;

            if (lastBody.positionY > 26)
            {
                lastBody.positionY = 0;
            }

            break;

        case Direction::LEFT:
            lastBody.positionX--;

            if (lastBody.positionX < 0)
            {
                lastBody.positionX = 47;
            }

            break;

        case Direction::RIGHT:
            lastBody.positionX++;

            if (lastBody.positionX > 47)
            {
                lastBody.positionX = 0;
            }

            break;
    }

    snake.push_back(lastBody);

    for (int i = 1; i < snake.size() - 1; i++)
    {
        if (lastBody.positionX == snake[i].positionX && lastBody.positionY == snake[i].positionY)
        {
            return false;
        }
    }

    return true;
}

bool Board::gotFood()
{
    SnakeBody head = snake[snake.size() - 1];

    if (head.positionX == foodX && head.positionY == foodY)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool Board::foodOnSnake()
{
    for (int i = 0; i < snake.size() - 1; i++)
    {
        if (snake[i].positionX == foodX && snake[i].positionY == foodY)
        {
            return true;
        }
    }

    return false;
}

int Board::getRandomNumber(int min, int max)
{
    return std::uniform_int_distribution<int>{min, max}(randomEngine);
}
