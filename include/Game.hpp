#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
#include <random>

#include "renderer/VulkanRenderer.hpp"
#include "renderer/ReactangleShape.hpp"
#include "Board.hpp"

#define ENABLE_DEBUG false //Enable Vulkan validation layers

//Main class used to init and run game

class Game
{
public:
    Game(int width, int height);

    int run();

private:
    SDL_Window* mainWindow;
    std::mt19937 randomEngine;
    int windowWidth, windowHeight;
    bool windowed;

    VulkanRenderer vulkanRenderer;
    Board board;
    ReactangleShape snake, food;

    bool initGame();
    int getRandomNumber(int min, int max);
};

#endif
