#include "Game.hpp"

#include <iostream>
#include <vector>
#include <chrono>

Game::Game(int width, int height)
{
    if (width < 0 || height < 0)
    {
        windowed = false;
    }
    else
    {
        windowed = true;
    }

    windowWidth = width;
    windowHeight = height;
}

bool Game::initGame()
{
    unsigned timeSeed = std::chrono::system_clock::now().time_since_epoch().count();
    randomEngine.seed(timeSeed);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL Init failed with error: " << SDL_GetError() << std::endl;

        return false;
    }

    mainWindow = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_VULKAN);

    if (mainWindow == NULL)
    {
        std::cerr << "SDL window creation failed with error: " << SDL_GetError() << std::endl;

        return false;
    }

    if (!windowed)
    {
        SDL_SetWindowFullscreen(mainWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_ShowCursor(SDL_DISABLE);

        SDL_DisplayMode displayMode;
        SDL_GetCurrentDisplayMode(0, &displayMode);

        windowWidth = displayMode.w;
        windowHeight = displayMode.h;
    }

    

    if (!vulkanRenderer.initRenderer(mainWindow, windowWidth, windowHeight, ENABLE_DEBUG))
    {
        std::cerr << "Vulkan Renderer initialization failed!" << std::endl;

        return false;
    }

    board.generateFood();

    snake.setSize(windowWidth / 48, windowHeight / 27); //40x40 on 1920x1080

    food.setSize(windowWidth / 48, windowHeight / 27);
    food.setPosition(board.foodX * food.width, board.foodY * food.height);
    food.setColor(255, 0, 0);

    return true;
}

int Game::run()
{
    if (!initGame())
    {
        return EXIT_FAILURE;
    }

    SDL_Event event;
    bool isRunning = true;

    double delta = SDL_GetTicks();
    double previousTime = SDL_GetTicks();
    double elapsedTime = 0;

    //Main loop
    while(isRunning)
    {
        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }

        const Uint8 *state = SDL_GetKeyboardState(NULL);

        if (state[SDL_SCANCODE_UP])
        {
            board.setDirection(Board::Direction::UP);
        }

        if (state[SDL_SCANCODE_DOWN])
        {
            board.setDirection(Board::Direction::DOWN);
        }

        if (state[SDL_SCANCODE_LEFT])
        {
            board.setDirection(Board::Direction::LEFT);
        }

        if (state[SDL_SCANCODE_RIGHT])
        {
            board.setDirection(Board::Direction::RIGHT);
        }

        if (state[SDL_SCANCODE_ESCAPE])
        {
            isRunning = false;
        }

        double currentTime = SDL_GetTicks();
        delta = currentTime - previousTime;
        previousTime = currentTime;
        elapsedTime += delta;

        //Make move after every 100ms
        if (elapsedTime >= 100)
        {
            
            if (!board.moveSnake())
            {
                isRunning = false;
            }
            
            food.setColor(getRandomNumber(32, 255), getRandomNumber(32, 255), getRandomNumber(32, 255));

            elapsedTime = 0;
        }

        if (board.gotFood())
        {
            board.addBody();
            board.generateFood();
        }

        for (SnakeBody snakeBody : board.snake)
        {
            snake.setColor(snakeBody.colorR, snakeBody.colorG, snakeBody.colorB);
            snake.setPosition(snakeBody.positionX * snake.width, snakeBody.positionY * snake.height);

            vulkanRenderer.draw(snake);
        }
        
        
        food.setPosition(board.foodX * food.width, board.foodY * food.height);
        vulkanRenderer.draw(food);
        
        vulkanRenderer.render();
    }

    vulkanRenderer.destroyRenderer();

    SDL_DestroyWindow(mainWindow);

    SDL_Quit();

    return EXIT_SUCCESS;
}

int Game::getRandomNumber(int min, int max)
{
    return std::uniform_int_distribution<int>{min, max}(randomEngine);
}
