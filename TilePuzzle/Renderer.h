#pragma once

#include <SDL.h>
#include <future>
#include <memory>
#include <thread>
#include <vector>
#include "GameBoard.h"

struct RendererDeleter
{
    void operator()(SDL_Renderer* renderer) const { SDL_DestroyRenderer(renderer); }
};

class Renderer
{
public:
    Renderer(SDL_Window* window, int rendererIndex, uint32_t rendererFlags);
    ~Renderer();

    SDL_Renderer* getRenderer() const { return m_renderer.get(); }

    template<typename... Args>
    void renderInLayers(std::vector<std::shared_ptr<Entity>>& first, Args&&... args) const
    {
        std::thread t(&Renderer::renderAll, this, std::cref(first));
        t.join();
        renderInLayers(std::forward<Args>(args)...);
    }

    void renderInLayers() const { SDL_RenderPresent(m_renderer.get());}

    void clear() const { SDL_RenderClear(m_renderer.get()); }
private:
    void renderAll(const std::vector<std::shared_ptr<Entity>>& entities) const;
    void render(const std::shared_ptr<Entity>& entity) const;
    std::unique_ptr<SDL_Renderer, RendererDeleter> m_renderer;
    std::thread m_renderingThread{};

};
