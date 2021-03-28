#include "Game.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
    int width = 960, height = 540;

    if (argc > 1)
    {
        if (strcmp(argv[1],"-fullscreen") == 0)
        {
            width = -1;
            height = -1;
        }
    }


    Game game(width, height);
    return game.run();
}
