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
    m_window = SDL_CreateWindow(
        "Tile Puzzle",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        Window::WINDOW_DIMENSIONS.x,
        Window::WINDOW_DIMENSIONS.y,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );

    if (m_window == nullptr)
        throw SDLInitializationException(SDL_GetError());

    m_renderer = std::make_unique<Renderer>(
        m_window,
        -1,
        SDL_RENDERER_ACCELERATED
    );

    SDL_SetRenderDrawBlendMode(m_renderer->getRenderer(), SDL_BLENDMODE_BLEND);

    // Load sprites
    m_player = std::make_shared<Player>(
	    Textures::PLAYER_SPRITE_PATH,
        &m_tileMap,
        10
    );

    m_rectangle = std::make_shared<Sprite>(
	    Textures::RECTANGLE_SPRITE_PATH,
        &m_tileMap
    );

    auto tiles = m_tileMap.getTiles();
    for (auto& row : *tiles) 
    {
        for (auto& tile : row)
        {
            tile = std::make_shared<Tile>(Tile::TileCode::BareGrass);
            tile->setCoordinates({
                static_cast<double>(&tile - row.data()) * Window::TILE_DIMENSIONS.x,
                static_cast<double>(&row - tiles->data()) * Window::TILE_DIMENSIONS.y
            });
            addBackgroundEntity(tile);
        }
    }
    m_rectangle->setCoordinates({ 100, 100 });
    constexpr SDL_Rect mouseRect = { 0, 0, 1, 1 };
    m_mouse = std::make_shared<Sprite>(mouseRect, SDL_Color{}, &m_tileMap);
    m_mouse->clearRenderFlag();

    auto a = m_rectangle->processSlices();
    for (auto& b : a)
        addForegroundEntity(b);
    addForegroundEntity(m_mouse);
    //addForegroundEntity(m_rectangle);
    addBackgroundEntity(m_player);
    std::cout << "finished init\n";
}

void Game::draw()
{
    // Render foreground entities
    m_renderer->renderAll(m_backgroundEntities, m_foregroundEntities);
    SDL_UpdateWindowSurface(m_window);
}

void Game::run()
{
    Counter counter(60);
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

            case SDL_MOUSEBUTTONDOWN:
                if (m_windowEvent.button.button == SDL_BUTTON_LEFT)
                {
                    //const Vector2<int> destination = m_tileMap.enclosingTileCenterCoordinates({ m_windowEvent.button.x, m_windowEvent.button.y }, m_player->getSdlRect());
                    //m_player->goTo(destination);
                    m_hoverTracker.getFocused()->onClick();
                }
                else if (m_windowEvent.button.button == SDL_BUTTON_RIGHT)
                {
                    std::cout << "Right mouse button clicked at: "
                        << m_windowEvent.button.x << ", "
                        << m_windowEvent.button.y << "\n";
                }
                break;
            default:
                break;
            }
        }
        counter.update();
        const double deltaTime = counter.getDeltaTime();
        //std::cout << "FPS: " << counter.getFps() << "\r";
        update(deltaTime);
        draw();
    }
}

void Game::update(const double deltaTime)
{
    for (const auto& entity : m_backgroundEntities)
        entity->update(deltaTime);

    Vector2<int> mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
    m_mouse->setCoordinates(mousePosition);
    bool mouseHoveredOverForegroundEntity = false;
    for (const auto& other : m_foregroundEntities)
    {
        if (other == m_mouse)
            continue;

        if (m_mouse->hasCollisionWith(*other,
            m_mouse->getCoordinates(),
            CollisionDetectionMethod::RectangularCollision))
        {
            mouseHoveredOverForegroundEntity = true;
            m_hoverTracker.setFocused(other);
        }
    }

    // Get the tile coordinates based on mouse position
    const Vector2<int> tileCoords = m_tileMap.enclosingTileCoordinates(mousePosition);
    const int x = tileCoords.x / Window::TILE_DIMENSIONS.x;
    const int y = tileCoords.y / Window::TILE_DIMENSIONS.y;
    std::shared_ptr<Entity> tile = m_tileMap.getTile(x, y);

    if (!mouseHoveredOverForegroundEntity)
    {
        m_hoverTracker.setFocused(tile);
    }
}

void Game::addBackgroundEntity(const std::shared_ptr<Entity>& entity)
{
    entity->cacheTexture(m_renderer->getRenderer());
    m_backgroundEntities.push_back(entity);
}

void Game::addForegroundEntity(const std::shared_ptr<Entity>& entity)
{
    entity->cacheTexture(m_renderer->getRenderer());
    m_foregroundEntities.push_back(entity);
}

//bool Game::canMoveTo(const Entity& entity, const Vector2<double> potentialPosition) const
//{
//    for (const auto& other : m_backgroundEntities)
//    {
//        if (other.get() == &entity)
//            continue;
//
//        if (potentialPosition.x + entity.getSdlRect().w > Window::WINDOW_DIMENSIONS.x 
//            || potentialPosition.x < 0
//            || potentialPosition.y + entity.getSdlRect().h > Window::WINDOW_DIMENSIONS.y
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

