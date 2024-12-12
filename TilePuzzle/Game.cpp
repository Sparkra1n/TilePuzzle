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
    // Load level-specific resources
    m_gameBoard = std::make_unique<GameBoard>(path, m_renderer->getRenderer());

    // Load general resources
    for (auto& entity : m_gameBoard->getTiles())
        addBackgroundEntity(entity);

    m_player = std::make_shared<Player>("./sprites/sword.bmp", m_renderer->getRenderer(), this, 10);
    addBackgroundEntity(m_player);
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
        m_player->handleEvent(m_windowEvent);
        switch (m_windowEvent.type)
        {
        case SDL_QUIT:
            return false;

        case SDL_MOUSEBUTTONDOWN:
            if (m_windowEvent.button.button == SDL_BUTTON_LEFT)
            {
                handleLeftMouseButtonClick(m_windowEvent.button);
            }
            else if (m_windowEvent.button.button == SDL_BUTTON_RIGHT)
            {
                handleRightMouseButtonClick(m_windowEvent.button);
            }
            break;

        default:
            break;
        }
    }
    return true;
}

void Game::handleLeftMouseButtonClick(const SDL_MouseButtonEvent& event)
{
    Vector2<int> mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

    const Vector2<int> destination = m_gameBoard->positionToTileCenter(
        { event.x, event.y }, 
        m_player->getSdlRect()
    );

    std::shared_ptr<Tile> tile = m_gameBoard->getEnclosingTile(destination);

    // Unoccupied destination tile
    if (tile->getResidingEntity() == nullptr)
    {
        std::vector<std::shared_ptr<Tile>> tiles = m_gameBoard->getPathToTile(
            m_gameBoard->getEnclosingTile(m_player->getPosition()),
            tile
        );

        std::vector<Vector2<int>> path;
        path.reserve(tiles.size());
        for (const auto& i : tiles)
            path.push_back(m_gameBoard->positionToTileCenter(
                i->getPosition(), 
                m_player->getSdlRect())
            );
        m_player->walk(path);
    }
    //FIXME: Go to a neighboring tile and push the slab
    else
    {
        std::shared_ptr<Tile> nextTileChoice = m_gameBoard->getClosestAvailableTile({ event.x, event.y }, m_player->getPosition());
        if (nextTileChoice)
        {
            std::vector<std::shared_ptr<Tile>> tiles = m_gameBoard->getPathToTile(
                m_gameBoard->getEnclosingTile(m_player->getPosition()),
                tile
            );

            std::vector<Vector2<int>> path;
            path.reserve(tiles.size());
            for (const auto& i : tiles)
                path.push_back(m_gameBoard->positionToTileCenter(
                    i->getPosition(),
                    m_player->getSdlRect())
                );
            m_player->walk(path);
            m_gameBoard->pushTile(m_gameBoard->getEnclosingTile(destination)->getResidingEntity(), m_player->getPosition());
        }
    }
    //m_hoverTracker.getFocused()->onClick();
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

    m_gameBoard->update(m_gameState);
    //for (const auto& entity : m_backgroundEntities)
    //    entity->update(m_gameState);
    //for (const auto& entity : m_foregroundEntities)
    //    entity->update(m_gameState);

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

