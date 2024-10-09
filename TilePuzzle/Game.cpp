#include "Game.h"

Game::Game(SDL_Window* window, const std::string& levelPath) : m_window(window)
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw SDLInitializationException(SDL_GetError());

    // Initialize SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
        throw SDLInitializationException(SDL_GetError());

    m_renderer = std::make_unique<Renderer>(m_window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(m_renderer->getRenderer(), SDL_BLENDMODE_BLEND);

    // Load the level requested by WindowLoader
    loadLevel(levelPath);
}

void Game::loadLevel(const std::string& path)
{
    // Load level-specific resources
    m_tileMap = std::make_unique<TileMap>(path, m_renderer->getRenderer());

    // Load general resources
    for (auto& entity : m_tileMap->getTiles())
    {
        addBackgroundEntity(entity);
    }
    m_player = std::make_shared<Player>(Textures::PLAYER_SPRITE_PATH, m_renderer->getRenderer(), this, 10);
    addBackgroundEntity(m_player);



    //for (auto& entity : m_tileMap->getTiles())
    //{
    //    addBackgroundEntity(entity);
    //}
    constexpr SDL_Rect mouseRect = { 0, 0, 1, 1 };
    m_mouse = std::make_shared<Sprite>(mouseRect, SDL_Color{}, m_renderer->getRenderer(), this);
    m_mouse->clearRenderFlag();

    //addForegroundEntity(m_mouse);
    //addBackgroundEntity(m_player);

    //m_tileMap->getTile(6, 0)->setAsGoalTile();
}

void Game::run()
{
    Counter counter(60);
    bool alive = true;

    while (alive)
    {
        alive = handleEvents();
        counter.update();
        const double deltaTime = counter.getDeltaTime();
        std::cout << "FPS: " << counter.getFps() << "\r";
        update(deltaTime);
        m_renderer->renderInLayers(m_backgroundEntities, m_foregroundEntities);
        SDL_UpdateWindowSurface(m_window);
    }
}

bool Game::handleEvents()
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

    const Vector2<int> destination = m_tileMap->getEnclosingTileCenterPosition(
        { event.x, event.y }, 
        m_player->getSdlRect()
    );

    std::shared_ptr<Tile> tile = m_tileMap->getEnclosingTile(destination);

    // Unoccupied destination tile
    if (tile->getResidingEntity() == nullptr)
    {
        std::vector<std::shared_ptr<Tile>> tiles = m_tileMap->getPathToTile(
            m_tileMap->getEnclosingTile(m_player->getPosition()),
            tile
        );

        std::vector<Vector2<int>> path;
        path.reserve(tiles.size());
        for (const auto& i : tiles)
            path.push_back(m_tileMap->getEnclosingTileCenterPosition(
                i->getPosition(), 
                m_player->getSdlRect())
            );
        m_player->walk(path);
    }
    //FIXME: Go to a neighboring tile and push the slab
    else
    {
        std::shared_ptr<Tile> nextTileChoice = m_tileMap->getClosestAvailableAdjacentTile({ event.x, event.y }, m_player->getPosition());
        if (nextTileChoice)
        {
            std::vector<std::shared_ptr<Tile>> tiles = m_tileMap->getPathToTile(
                m_tileMap->getEnclosingTile(m_player->getPosition()),
                tile
            );

            std::vector<Vector2<int>> path;
            path.reserve(tiles.size());
            for (const auto& i : tiles)
                path.push_back(m_tileMap->getEnclosingTileCenterPosition(
                    i->getPosition(),
                    m_player->getSdlRect())
                );
            m_player->walk(path);
            m_tileMap->pushTile(m_tileMap->getEnclosingTile(destination)->getResidingEntity(), m_player->getPosition());
        }
    }
    //m_hoverTracker.getFocused()->onClick();
}

void Game::handleRightMouseButtonClick(const SDL_MouseButtonEvent& event)
{
    std::shared_ptr<Tile> enclosingTile = m_tileMap->getEnclosingTile({ event.x, event.y });
    if (enclosingTile->getResidingEntity() == nullptr)
    {
        auto newSlab = std::make_shared<GameObject>(Textures::SLAB_SPRITE_PATH, m_renderer->getRenderer());

        // move into tilemap
        newSlab->setPosition(TileMap::getEnclosingTileCenterPosition(enclosingTile->getPosition(), newSlab->getSdlRect()));
        enclosingTile->setResidingEntity(newSlab);
        addForegroundEntity(newSlab);
    }
}

void Game::update(const double deltaTime)
{
    for (const auto& entity : m_backgroundEntities)
        entity->update(deltaTime);
    for (const auto& entity : m_foregroundEntities)
        entity->update(deltaTime);

    Vector2<int> mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
    m_mouse->setPosition(mousePosition);
    bool mouseHoveredOverForegroundEntity = false;
    for (const auto& other : m_foregroundEntities)
    {
        if (other == m_mouse)
            continue;

        if (m_mouse->hasCollisionWith(*other,
            m_mouse->getPosition(),
            CollisionDetectionMethod::RectangularCollision))
        {
            mouseHoveredOverForegroundEntity = true;
            m_hoverTracker.setFocused(other);
        }
    }

    // Get the tile coordinates based on mouse position
    const Vector2<int> tileCoords = m_tileMap->getEnclosingTilePosition(mousePosition);
    const int x = tileCoords.x / Tile::TILE_DIMENSIONS.x;
    const int y = tileCoords.y / Tile::TILE_DIMENSIONS.y;
    std::shared_ptr<Entity> tile = m_tileMap->getTile(x, y);

    if (!mouseHoveredOverForegroundEntity)
    {
        m_hoverTracker.setFocused(tile);
    }

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

//bool Game::canMoveTo(const Entity& entity, const Vector2<double> potentialPosition) const
//{
//    for (const auto& other : m_backgroundEntities)
//    {
//        if (other.get() == &entity)
//            continue;
//
//        if (potentialPosition.x + entity.getSdlRect().w > WindowLoader::WINDOW_DIMENSIONS.x 
//            || potentialPosition.x < 0
//            || potentialPosition.y + entity.getSdlRect().h > WindowLoader::WINDOW_DIMENSIONS.y
//            || potentialPosition.y < 0)
//            return false;
//
//        if (entity.hasCollisionWith(*other, potentialPosition, CollisionDetectionMethod::PolygonCollision))
//            return false;
//    }
//    return true;
//}

Game::~Game()
{
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

