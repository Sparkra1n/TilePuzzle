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
        "Subterranean Animism",
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

    m_cursor = std::make_shared <Sprite<NoCollision>>(CURSOR_SPRITE_PATH);

    // Load sprites
    m_player = std::make_shared<Player>(
        PLAYER_SPRITE_PATH,
        this,
        5
    );

    for (auto& row : m_tileMap) 
    {
        for (auto& tile : row) 
        {
            tile = std::make_shared<Sprite<NoCollision>>(Tile::GRASS_SPRITE_PATH);
            tile->setCoordinates({
                static_cast<double>(&tile - row.data()) * Tile::TILE_DIMENSIONS.x,
                static_cast<double>(&row - m_tileMap.data()) * Tile::TILE_DIMENSIONS.y
            });
            tile->cacheTexture(m_renderer->getRenderer());
            addEntity(tile);
        }
    }
    addEntity(m_player);
    addEntity(m_cursor);
}

void Game::draw()
{
    //m_renderer->renderTiles(m_tileMap);
    // Render foreground entities
    m_renderer->renderAll(m_entities);
    SDL_UpdateWindowSurface(m_window);
}

void Game::run()
{
    FPSCounter fpsCounter(60);
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
                    //std::cout << "Left mouse button clicked at: "
                    //    << m_windowEvent.button.x << ", "
                    //    << m_windowEvent.button.y << std::endl;
                    Vector2<int> destination = enclosingTileCenter({ m_windowEvent.button.x, m_windowEvent.button.y }, m_player->getSDLRect());
                   /* std::cout << "Will walk " << destination - Vector2(m_player->getSDLRect().x, m_player->getSDLRect().y) << " to " << destination << "\n";*/
                    m_player->walkTo(destination);
                }
                else if (m_windowEvent.button.button == SDL_BUTTON_RIGHT)
                {
                    std::cout << "Right mouse button clicked at: "
                        << m_windowEvent.button.x << ", "
                        << m_windowEvent.button.y << std::endl;
                }
                break;
            default:
                break;
            }
        }
        fpsCounter.update();
        const double deltaTime = fpsCounter.getDeltaTime();
        update(deltaTime);
        draw();
    }
}

void Game::update(const double deltaTime) const
{
    for (const auto& entity : m_entities)
        entity->update(deltaTime);

    Vector2<int> mousePosition;
    SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
    //std::cout << mousePosition << "\r";
    mousePosition = enclosingTile(mousePosition);
    m_cursor->setCoordinates(mousePosition);
}

void Game::addEntity(const std::shared_ptr<Entity>& entity)
{
    entity->cacheTexture(m_renderer->getRenderer());
    m_entities.push_back(entity);
}

bool Game::canMoveTo(const Entity& entity, const Vector2<double> potentialPosition) const
{
    for (const auto& other : m_entities)
    {
        if (other.get() == &entity)
        {
            continue;
        }

        if (potentialPosition.x + entity.getSDLRect().w > Tile::WINDOW_DIMENSIONS.x 
            || potentialPosition.x < 0
            || potentialPosition.y + entity.getSDLRect().h > Tile::WINDOW_DIMENSIONS.y
            || potentialPosition.y < 0)
        {
            return false;
        }

        if (entity.hasCollisionWith(*other, potentialPosition))
        {
            return false;
        }
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

