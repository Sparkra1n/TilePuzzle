#include "Game.h"
#include <iostream>

Game::Game()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw SDLInitializationException(SDL_GetError());

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
        throw SDLInitializationException(SDL_GetError());

    // Create window
    m_window = SDL_CreateWindow("Sprite Renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_screenWidth, m_screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (m_window == nullptr)
        throw SDLInitializationException(SDL_GetError());

    // Create window surface
    m_windowSurface = SDL_GetWindowSurface(m_window);
    if (m_windowSurface == nullptr)
        throw SDLInitializationException(SDL_GetError());

    m_image = Sprite("C:/Users/spark/Documents/Visual Studio 2022/Projects/SubterraneanAnimism/SubterraneanAnimism/sprites/sword.bmp");
}

void Game::draw()
{
    SDL_FillRect(m_windowSurface, nullptr, SDL_MapRGB(m_windowSurface->format, 0, 0, 0));
   /* SDL_BlitSurface(m_image, nullptr, m_windowSurface, &m_imagePosition);*/
    m_image.draw(m_windowSurface);
	SDL_UpdateWindowSurface(m_window);
}

void Game::run()
{
    bool alive = true;
    while (alive)
    {
        while (SDL_PollEvent(&m_windowEvent) > 0)
        {
            m_image.handleEvent(m_windowEvent);
            switch (m_windowEvent.type)
            {
            case SDL_QUIT:
                alive = false;
                break;
            }
        }

        update(1.0 / 60.0);
        draw();
    }
}
void Game::update(const double deltaTime)
{
    //m_imageX = m_imageX + 5 * deltaTime;
    //m_imagePosition.x = static_cast<int>(m_imageX);
    m_image.update(deltaTime);
}



Game::~Game()
{
    SDL_FreeSurface(m_windowSurface);
    //SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

