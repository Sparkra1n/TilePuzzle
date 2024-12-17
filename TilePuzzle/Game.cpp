#include "Game.h"

Game::Game(SDL_Window* window, const std::string& levelPath) : m_window(window)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw SDLInitException(SDL_GetError());

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
        throw SDLInitException(SDL_GetError());

    m_renderer = std::make_unique<Renderer>(m_window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(m_renderer->getRenderer(), SDL_BLENDMODE_BLEND);

    // Load the level requested by WindowLoader
    loadLevel(levelPath);
}

void Game::loadLevel(const std::string& path)
{
    std::cout << "load player\n";
    m_player = std::make_shared<Player>("./sprites/sword.bmp", m_renderer->getRenderer(), this, 10);
    addForegroundEntity(m_player);

    // Load level-specific resources
    m_gameBoard = std::make_unique<GameBoard>(path, m_player, m_renderer->getRenderer());

    // Load general resources
    for (auto& entity : m_gameBoard->getTiles())
        addBackgroundEntity(entity);
}

void Game::run()
{
    Counter counter(60);
    bool alive = true;

    while (alive)
    {
        alive = handleInputEvents();
        counter.update();
        update(counter.getDeltaTime());
        m_renderer->renderInLayers(m_backgroundEntities, m_foregroundEntities);
        SDL_UpdateWindowSurface(m_window);
    }
}

bool Game::handleInputEvents()
{
    while (SDL_PollEvent(&m_windowEvent) > 0)
    {
        switch (m_windowEvent.type)
        {
        case SDL_QUIT:
            return false;

        case SDL_MOUSEBUTTONDOWN:
            if (m_windowEvent.button.button == SDL_BUTTON_LEFT)
                handleLeftMouseButtonClick(m_windowEvent.button);

            else if (m_windowEvent.button.button == SDL_BUTTON_RIGHT)
                handleRightMouseButtonClick(m_windowEvent.button);
            break;

        default:
            break;
        }
    }
    return true;
}

void Game::handleLeftMouseButtonClick(const SDL_MouseButtonEvent& event)
{
	m_gameBoard->onClick(m_gameState);
}

void Game::handleRightMouseButtonClick(const SDL_MouseButtonEvent& event)
{

}

void Game::update(const double deltaTime)
{
    Vector2<int> mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
    m_gameState.mousePosition = mousePosition;
    m_gameState.deltaTime = deltaTime;
    //std::cout << mousePosition << "\r";
    m_gameBoard->update(m_gameState);

    // Check if the game has finished
    //if (m_tileMap->isSolved())
        //std::cout << "Game finished!\n";
}

void Game::addBackgroundEntity(const std::shared_ptr<Entity>& entity)
{
    entity->cacheTexture();
    m_backgroundEntities.push_back(entity);
}

void Game::addForegroundEntity(const std::shared_ptr<Entity>& entity)
{
    entity->cacheTexture();
    m_foregroundEntities.push_back(entity);
}

Game::~Game()
{
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

