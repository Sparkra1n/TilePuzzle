#pragma once

#include <memory>
#include <SDL.h>
#include <vector>
#include "Sprite.h"

struct RendererDeleter
{
	void operator()(SDL_Renderer* renderer) const { SDL_DestroyRenderer(renderer); }
};

class Renderer
{
public:
	Renderer(SDL_Window* window, int rendererIndex, uint32_t rendererFlags);
	~Renderer() = default;
	[[nodiscard]] const SDL_Renderer* getRenderer() const;
	void renderAll(const std::vector<std::shared_ptr<Entity>>& entities) const;
	void setDrawColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a) const;
	void clear() const;
	void renderPresent() const;
private:
	std::unique_ptr<SDL_Renderer, RendererDeleter> m_renderer;
};
