//#pragma once
//
//#include <memory>
//#include <stdexcept>
//#include <SDL.h>
//#include <vector>
//
//#include "Sprite.h"
//
//struct RendererDeleter
//{
//	void operator()(SDL_Renderer* renderer) const { SDL_DestroyRenderer(renderer); }
//};
//
//class Renderer
//{
//public:
//	Renderer(SDL_Window* window, const int rendererIndex, const uint32_t rendererFlags);
//
//	Renderer() = default;
//
//	void renderAll() const;
//
//	void addSprite(const std::shared_ptr<Sprite>& sprite);
//
//	[[nodiscard]] const SDL_Renderer* getRenderer() const { return m_renderer.get(); }
//
//	~Renderer() = default;
//private:
//	std::unique_ptr<SDL_Renderer, RendererDeleter> m_renderer;
//	std::vector<std::shared_ptr<Sprite>> m_sprites;
//};
