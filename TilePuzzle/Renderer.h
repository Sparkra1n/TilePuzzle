#pragma once

#include <SDL.h>
#include <future>
#include <memory>
#include <thread>
#include <vector>
#include "Sprite.h"
#include "Tile.h"

struct RendererDeleter
{
    void operator()(SDL_Renderer* renderer) const { SDL_DestroyRenderer(renderer); }
};

class Renderer
{
public:
    Renderer(SDL_Window* window, int rendererIndex, uint32_t rendererFlags);
    ~Renderer();

	SDL_Renderer* getRenderer() const;

    template<typename... Args>
    void renderAll(std::vector<std::shared_ptr<Entity>>& first, Args&&... args) const
    {
        std::thread t(&Renderer::render, this, std::cref(first));
        t.join();
        renderAll(std::forward<Args>(args)...);
    }

	void renderAll() const
    {
        SDL_RenderPresent(m_renderer.get());
    }

    void clear() const;
private:
    void render(const std::vector<std::shared_ptr<Entity>>& entities) const;
    std::unique_ptr<SDL_Renderer, RendererDeleter> m_renderer;
    std::thread m_renderingThread{};

};
