#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include "SDLExceptions.h"
#include "Sprite.h"
#include "Vector2.h"
#include "Player.h"
#include "Projectile.h"
#include "Renderer.h"

class Game : public Observer
{
public:
    Game();
    ~Game() override;
    void draw();
    void run();
    void update(double deltaTime);
    void addEntity(const std::shared_ptr<Entity>& entity);
    [[nodiscard]] bool canMoveTo(const Entity& entity, Vector2<double> potentialPosition) const override;
private:
    std::vector<std::shared_ptr<Entity>> m_entities;             // Collection of sprites in the game
	std::shared_ptr<Player> m_player;                            // Player sprite
    std::shared_ptr<Sprite<RectangularCollision>> m_rectangle;   // Example collider
    std::shared_ptr<Sprite<NoCollision>> m_projectile;           // Example Projectile

    SDL_Window* m_window{};                         // SDL window instance
    SDL_Surface* m_windowSurface{};                 // SDL window surface
    SDL_Event m_windowEvent{};                      // SDL event for window handling

    //std::unique_ptr<Renderer> m_renderer{};

    int m_screenWidth = 640;
    int m_screenHeight = 480;
};
