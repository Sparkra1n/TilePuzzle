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

    m_player  = std::make_shared<PlayerSprite>("C:/Users/spark/Documents/Visual Studio 2022/Projects/SubterraneanAnimism/SubterraneanAnimism/sprites/sword.bmp", 1.0);
}

void Game::draw()
{
    SDL_FillRect(m_windowSurface, nullptr, SDL_MapRGB(m_windowSurface->format, 0, 0, 0));
    m_player->draw(m_windowSurface);
	SDL_UpdateWindowSurface(m_window);
}

void Game::run()
{
    bool alive = true;
    while (alive)
    {
        while (SDL_PollEvent(&m_windowEvent) > 0)
        {
            m_player->handleEvent(m_windowEvent);
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
void Game::update(const double deltaTime) const
{
    for (const auto& sprite : m_sprites)
        sprite->update(deltaTime);
}

void Game::addSprite(CollisionSprite& sprite)
{
    m_sprites.push_back(std::make_shared<CollisionSprite>(sprite));
    sprite.setObserver(*this);
}

bool Game::canMoveTo(CollisionSprite& collisionSprite, double potentialX, double potentialY)
{
    // Check for collision with other sprites in the game
    for (const auto& sprite : m_sprites)
    {
        if (sprite->hasCollisionWith(collisionSprite))
            return false; 
    }
    return true;
}

Game::~Game()
{
    SDL_FreeSurface(m_windowSurface);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

