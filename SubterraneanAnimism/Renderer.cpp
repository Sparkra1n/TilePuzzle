#include "Renderer.h"

Renderer::Renderer(SDL_Window* window, const int rendererIndex, const uint32_t rendererFlags)
{
	SDL_Renderer* renderer = SDL_CreateRenderer(window, rendererIndex, rendererFlags);

    if (!renderer)
        throw std::runtime_error("Failed to create SDL_Renderer");

    m_renderer = std::unique_ptr<SDL_Renderer, RendererDeleter>(renderer);
}

void Renderer::renderAll(const std::vector<std::shared_ptr<Entity>>& entities) const
{
    SDL_RenderClear(m_renderer.get());

    for (const auto& entity : entities)
    {
        SDL_Texture* entityTexture = SDL_CreateTextureFromSurface(m_renderer.get(), entity->getSDLSurface());
        SDL_Rect entityRect = entity->getScreenPositionAndDimensions();
        SDL_RenderCopy(m_renderer.get(), entityTexture, nullptr, &entityRect);
    }

    SDL_RenderPresent(m_renderer.get());
}

const SDL_Renderer* Renderer::getRenderer() const
{
    return m_renderer.get();
}

void Renderer::setDrawColor(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) const
{
	SDL_SetRenderDrawColor(m_renderer.get(), r, g, b, a);
}

void Renderer::clear() const
{
	SDL_RenderClear(m_renderer.get());
}

void Renderer::renderPresent() const
{
    SDL_RenderPresent(m_renderer.get());
}


