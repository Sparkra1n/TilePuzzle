#include "Renderer.h"
#include "Tile.h"

Renderer::Renderer(
    SDL_Window* window,
    const int rendererIndex,
    const uint32_t rendererFlags)
{
    SDL_Renderer* renderer = SDL_CreateRenderer(window, rendererIndex, rendererFlags);

    if (!renderer)
        throw std::runtime_error("Failed to create SDL_Renderer");

    m_renderer = std::unique_ptr<SDL_Renderer, RendererDeleter>(renderer);
}

Renderer::~Renderer()
{
    if (m_renderingThread.joinable())
    {
        m_renderingThread.join();
    }
}

void Renderer::render(const std::vector<std::shared_ptr<Entity>>& entities) const
{
    for (const auto& entity : entities)
    {
        if (!entity->getRenderFlag())
            continue;

        SDL_Rect entityRect = entity->getSDLRect();
        SDL_RenderCopy(m_renderer.get(), entity->getCachedTexture(), nullptr, &entityRect);
    }
}

SDL_Renderer* Renderer::getRenderer() const
{
    return m_renderer.get();
}

void Renderer::clear() const
{
    SDL_RenderClear(m_renderer.get());
}