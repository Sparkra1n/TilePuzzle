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


	// Load sprites
    m_player = std::make_shared<PlayerSprite>("C:/Users/spark/Documents/Visual Studio 2022/Projects/SubterraneanAnimism/SubterraneanAnimism/sprites/sword.bmp");
    m_rectangle = std::make_shared<CollisionSprite>("C:/Users/spark/Documents/Visual Studio 2022/Projects/SubterraneanAnimism/SubterraneanAnimism/sprites/rectangle.bmp");
    m_rectangle->setPosition(200, 200);

    addSprite(m_player);
    addSprite(m_rectangle);
}

void Game::draw()
{
    SDL_FillRect(m_windowSurface, nullptr, SDL_MapRGB(m_windowSurface->format, 0, 0, 0));
    m_player->draw(m_windowSurface);
    m_rectangle->draw(m_windowSurface);
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

void Game::addSprite(const std::shared_ptr<CollisionSprite>& sprite)
{
    m_sprites.push_back(sprite);
    sprite->setObserver(this);
}

bool Game::canMoveTo(const CollisionSprite& collisionSprite, double potentialX, double potentialY) const
{
	SDL_Rect potentialSprite1Position = collisionSprite.getPosition();
    potentialSprite1Position.x = static_cast<int>(potentialX);
    potentialSprite1Position.y = static_cast<int>(potentialY);

    for (const auto& sprite : m_sprites)
    {
        // Skip checking if sprite collides with itself
        if (sprite.get() == &collisionSprite)
            continue;

        // Check if movement will result in collision
        SDL_Rect sprite2Position = sprite.get()->getPosition();
        if (SDL_HasIntersection(&sprite2Position, &potentialSprite1Position) == SDL_TRUE)
        {
            return false;
        }
    }
    return true;
}

Game::~Game()
{
    SDL_FreeSurface(m_windowSurface);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

