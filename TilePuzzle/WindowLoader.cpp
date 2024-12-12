/**
 * @file WindowLoader.cpp
 */

#include "WindowLoader.h"

std::shared_ptr<SDL_Window> WindowLoader::loadStartScreen()
{
    window = createWindow("Start Menu");
    return window;
}

std::shared_ptr<SDL_Window> WindowLoader::loadBoard(const std::string& path)
{
    window = createWindow("Game Board");
    game = std::make_unique<Game>(window.get(), path);
    game->run();
    return window;
}

std::shared_ptr<SDL_Window> WindowLoader::createWindow(const std::string& title)
{
    SDL_Window* window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_DIMENSIONS.x,
        WINDOW_DIMENSIONS.y,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );

    if (window == nullptr)
        throw SDLInitException(SDL_GetError());

    return { window, SDL_DestroyWindow };
}

