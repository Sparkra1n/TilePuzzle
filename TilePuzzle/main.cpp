#include "WindowLoader.h"
#include <iostream>

// TODO: Each sprite should have multiple states it can exist as, and each of those should be cached.
// TODO: That way, they can easily switch from one to another.

int main(int argc, char** argv)
{
    WindowLoader loader;

    //if (argc > 1)
    //{
    //    // Load the specified level
    //    std::string levelPath = argv[1];
    //    loader.loadBoard(levelPath);
    //}
    //else
    //{
    //    // Load start menu
    //    loader.loadStartScreen();
    //}

    loader.loadBoard("start.txt");

    return 0;
}
