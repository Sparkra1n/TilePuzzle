#include "Game.h"
#include <algorithm>
#include <functional>

Game::Game()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw SDLInitializationException(SDL_GetError());

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
        throw SDLInitializationException(SDL_GetError());

    // Create window
    m_window = SDL_CreateWindow("Subterranean Animism", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_screenWidth, m_screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    if (m_window == nullptr)
        throw SDLInitializationException(SDL_GetError());

    // Create window surface
    m_windowSurface = SDL_GetWindowSurface(m_window);
    if (m_windowSurface == nullptr)
        throw SDLInitializationException(SDL_GetError());

	// Load sprites
    m_player = std::make_shared<Player>("C:/Users/spark/Documents/Visual Studio 2022/Projects/SubterraneanAnimism/SubterraneanAnimism/sprites/sword.bmp", this, 5);
	m_rectangle = std::make_shared<Sprite<RectangularCollision>>("C:/Users/spark/Documents/Visual Studio 2022/Projects/SubterraneanAnimism/SubterraneanAnimism/sprites/rectangle.bmp", this);
    m_rectangle->setCoordinates({ 200, 200 });

    addEntity(m_player);
    addEntity(m_rectangle);
}

void Game::draw()
{
    SDL_FillRect(m_windowSurface, nullptr, SDL_MapRGB(m_windowSurface->format, 0, 0, 0));
    m_player->draw(m_windowSurface);
    m_rectangle->draw(m_windowSurface);
    //m_projectile->draw(m_windowSurface);
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
void Game::update(const double deltaTime) 
{
    for (const auto& entity : m_entities)
        entity->update(deltaTime);
}

void Game::addEntity(const std::shared_ptr<Entity>& entity)
{
    m_entities.push_back(entity);
}

bool Game::canMoveTo(const Entity& entity, Vector2<double> potentialPosition) const
{
    for (const auto& other : m_entities)
    {
        if (other.get() == &entity)
            continue;

        if (other->isSpecializedSprite())
        {
            // TODO: implement potentialPosition
            
            //const Sprite<RectangularCollision>* x = dynamic_cast<const Sprite<RectangularCollision>*>(other.get());
            //if (x)
            //    std::cout << "it should call the right one\n";

            bool a = entity.hasCollisionWith(*other, potentialPosition);
            std::cout << "hasCollision: " << a << "\n";
            if (a)
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

