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
    [[nodiscard]] SDL_Renderer* getRenderer() const;
    void renderAll(std::vector<std::shared_ptr<Entity>> entities) const;
    void clear() const;
    void renderPresent() const;
private:
    void renderAsync(const std::vector<std::shared_ptr<Entity>>& entities) const;
    std::unique_ptr<SDL_Renderer, RendererDeleter> m_renderer;
    std::thread m_renderingThread{};

};
