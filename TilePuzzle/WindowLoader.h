#pragma once
#include <SDL.h>
#include <memory>
#include <string>
#include "Game.h"
#include "Vector2.h"
#include <memory>
#include "GameBoard.h"
#include "Vector2.h"
#include <cmath>
#include <fstream>

class WindowLoader
{
public:
    std::shared_ptr<SDL_Window> loadStartScreen();
    std::shared_ptr<SDL_Window> loadBoard(const std::string& path);
    static constexpr Vector2<int> WINDOW_DIMENSIONS = { 800, 600 };
private:
    std::shared_ptr<SDL_Window> window;
    std::unique_ptr<Game> game;
    static std::shared_ptr<SDL_Window> createWindow(const std::string& title);
};
