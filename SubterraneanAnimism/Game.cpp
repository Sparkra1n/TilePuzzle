#include "Game.h"

#include "FPSCounter.h"

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
        m_screenWidth, 
        m_screenHeight, 
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
    );

    if (m_window == nullptr)
        throw SDLInitializationException(SDL_GetError());

    m_renderer = std::make_unique<Renderer>(
        m_window, 
        -1,
        SDL_RENDERER_ACCELERATED
    );

	// Load sprites
    m_player = std::make_shared<Player>(
        "C:/Users/spark/Documents/Visual Studio 2022/Projects/SubterraneanAnimism/SubterraneanAnimism/sprites/sword.bmp",
        this,
        5
    );
	//m_rectangle = std::make_shared<Sprite<RectangularCollision>>(
 //       "C:/Users/spark/Documents/Visual Studio 2022/Projects/SubterraneanAnimism/SubterraneanAnimism/sprites/rectangle.bmp",
 //       this
 //   );
 //   m_rectangle->setCoordinates({ 200, 200 });

    m_curvy = std::make_shared<Sprite<PolygonCollision>> (
        "C:/Users/spark/Documents/Visual Studio 2022/Projects/SubterraneanAnimism/SubterraneanAnimism/sprites/curvy.bmp",
        this
    );
    m_curvy->setCoordinates({ 200, 200 });
    addEntity(m_curvy);
    addEntity(m_player);
    m_curvy->cacheTexture(m_renderer->getRenderer());
    m_player->cacheTexture(m_renderer->getRenderer());
    m_curvy->slice(5);
    m_curvy->printSlices();
    auto a = m_curvy->processSlices();
    for (auto& b : a)
    {
        addEntity(b);
        b->cacheTexture(m_renderer->getRenderer());
    }
}

void Game::draw() const
{
    m_renderer->setDrawColor(0, 0, 0, 255);
    m_renderer->clear();
    m_renderer->renderAll(m_entities);
	SDL_UpdateWindowSurface(m_window);
}

void Game::run()
{
    FPSCounter fpsCounter;
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
        fpsCounter.update();
        std::cout << "FPS: " << fpsCounter.getFPS() << "\r";
        draw();
        update(fpsCounter.getDeltaTime());
    }
}
void Game::update(const double deltaTime) const
{
    for (const auto& entity : m_entities)
        entity->update(deltaTime);
}

void Game::addEntity(const std::shared_ptr<Entity>& entity)
{
    m_entities.push_back(entity);
}

bool Game::canMoveTo(const Entity& entity, const Vector2<double> potentialPosition) const
{
    for (const auto& other : m_entities)
    {
        if (other.get() == &entity)
            continue;

        if (other->isSpecializedSprite())
        {
            if (entity.hasCollisionWith(*other, potentialPosition))
                return false;
        }
    }
    return true;
}

Game::~Game()
{
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}

