//#include "Renderer.h"
//
//Renderer::Renderer(SDL_Window* window, const int rendererIndex, const uint32_t rendererFlags)
//{
//	SDL_Renderer* renderer = SDL_CreateRenderer(window, rendererIndex, rendererFlags);
//
//    if (!renderer)
//        throw std::runtime_error("Failed to create SDL_Renderer");
//
//    m_renderer = std::unique_ptr<SDL_Renderer, RendererDeleter>(renderer);
//}
//
//void Renderer::renderAll() const
//{
//    SDL_RenderClear(m_renderer.get());
//
//    for (const auto& sprite : m_sprites)
//    {
//        //sprite->render(m_renderer);
//    }
//
//    SDL_RenderPresent(m_renderer.get());
//}
//
//void Renderer::addSprite(const std::shared_ptr<Sprite>& sprite)
//{
//}
