#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include "SDLExceptions.h"
#include "Sprite.h"

class Game
{
public:
    void draw();
    void run();
    void update(double deltaTime);
    Game();
    ~Game();
private:
	Sprite m_image;

    SDL_Window* m_window{};
    SDL_Surface* m_windowSurface{};
    SDL_Event m_windowEvent{};

    //SDL_Renderer* m_renderer{};
    SDL_Event m_event{};

    int m_screenWidth = 640;
    int m_screenHeight = 480;
};
