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
        Tile::WINDOW_DIMENSIONS.x,
        Tile::WINDOW_DIMENSIONS.y,
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

    // Initialize background
    m_board =
    {

    };

    // Load sprites
    m_player = std::make_shared<Player>(
        PLAYER_SPRITE_PATH,
        this,
        10
    );

    m_rectangle = std::make_shared<Sprite>(
        RECTANGLE_SPRITE_PATH,
        this
    );

    for (auto& row : m_tileMap) 
    {
        for (auto& tile : row)
        {
            tile = std::make_shared<Sprite>(GRASS_SPRITE_PATH);
            tile->setCoordinates({
                static_cast<double>(&tile - row.data()) * Tile::TILE_DIMENSIONS.x,
                static_cast<double>(&row - m_tileMap.data()) * Tile::TILE_DIMENSIONS.y
            });
            addBackgroundEntity(tile);
        }
    }
    m_rectangle->setCoordinates({ 100, 100 });
    constexpr SDL_Rect mouseRect = { 0, 0, 1, 1 };
    m_mouse = std::make_shared<Sprite>(mouseRect, SDL_Color{}, this);
    m_mouse->clearRenderFlag();

    auto a = m_rectangle->processSlices();
    for (auto& b : a)
        addForegroundEntity(b);
    addForegroundEntity(m_mouse);
    //addForegroundEntity(m_rectangle);
    addBackgroundEntity(m_player);
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
                    const Vector2<int> destination = enclosingTileCenter({ m_windowEvent.button.x, m_windowEvent.button.y }, m_player->getSdlRect());
                    m_player->goTo(destination);
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
        std::cout << "FPS: " << counter.getFps() << "\r";
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
    const Vector2<int> tileCoords = enclosingTile(mousePosition);
    const int x = tileCoords.x / Tile::TILE_DIMENSIONS.x;
    const int y = tileCoords.y / Tile::TILE_DIMENSIONS.y;
    const std::shared_ptr<Entity> tile = m_tileMap[y][x];

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

bool Game::canMoveTo(const Entity& entity, const Vector2<double> potentialPosition) const
{
    for (const auto& other : m_backgroundEntities)
    {
        if (other.get() == &entity)
            continue;

        if (potentialPosition.x + entity.getSdlRect().w > Tile::WINDOW_DIMENSIONS.x 
            || potentialPosition.x < 0
            || potentialPosition.y + entity.getSdlRect().h > Tile::WINDOW_DIMENSIONS.y
            || potentialPosition.y < 0)
            return false;

        if (entity.hasCollisionWith(*other, potentialPosition, CollisionDetectionMethod::PolygonCollision))
            return false;
    }
    return true;
}

Vector2<int> Game::enclosingTile(const Vector2<int> position)
{
    int x = position.x - position.x % Tile::TILE_DIMENSIONS.x;
    int y = position.y - position.y % Tile::TILE_DIMENSIONS.y;
    return {x, y};
}

Vector2<int> Game::enclosingTileCenter(const Vector2<int> position, const SDL_Rect spriteDimensions)
{
    int x = position.x - position.x % Tile::TILE_DIMENSIONS.x + Tile::TILE_DIMENSIONS.x / 2 - spriteDimensions.w / 2;
    int y = position.y - position.y % Tile::TILE_DIMENSIONS.y + Tile::TILE_DIMENSIONS.y / 2 - spriteDimensions.h / 2;
    return { x, y };
}

Game::~Game()
{
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

