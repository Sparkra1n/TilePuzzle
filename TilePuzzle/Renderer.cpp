#include "Renderer.h"
#include "GameBoard.h"

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

void Renderer::renderAll(const std::vector<std::shared_ptr<Entity>>& entities) const
{
    for (const auto& entity : entities)
    {
        render(entity);
    }
}

void Renderer::render(const std::shared_ptr<Entity>& entity) const
{
    if (!entity->getRenderFlag())
        return;

    SDL_Rect entityRect = entity->getSdlRect();
    SDL_RenderCopy(m_renderer.get(), entity->getCachedTexture(), nullptr, &entityRect);
}

