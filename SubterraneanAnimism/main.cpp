#include "Game.h"
#include <iostream>

int main(int argc, char** argv)
{
    std::cout << "Starting...\n";
    try {
		Game game;
        game.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
