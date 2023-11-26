#include "Game.h"

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

    m_image = loadSurface("C:/Users/spark/Documents/Visual Studio 2022/Projects/SubterraneanAnimism/SubterraneanAnimism/sprites/sword.bmp");
}

void Game::draw()
{
    SDL_FillRect(m_windowSurface, nullptr, SDL_MapRGB(m_windowSurface->format, 0, 0, 0));
    SDL_BlitSurface(m_image, nullptr, m_windowSurface, &m_imagePosition);
	SDL_UpdateWindowSurface(m_window);
}

void Game::run()
{
    bool alive = true;
    while (alive)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                alive = false;
        }
        update(1.0 / 60.0);
        draw();
    }
}
void Game::update(const double deltaTime)
{
    m_imageX = m_imageX + 5 * deltaTime;
    m_imagePosition.x = static_cast<int>(m_imageX);
}

SDL_Surface* Game::loadSurface(const char* path)
{
	 SDL_Surface* surface = SDL_LoadBMP(path);
    if (!surface)
        throw SDLImageLoadException(SDL_GetError());
    return surface;
}

Game::~Game()
{
    SDL_FreeSurface(m_windowSurface);
    //SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

