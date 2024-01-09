#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>

#include "CollisionSprite.h"
#include "SDLExceptions.h"
#include "Sprite.h"
#include "Vector2.h"
#include "Player.h"
#include "Projectile.h"
#include "Renderer.h"

class Game : public Observer
{
public:
    /**
     * @brief Draws the game window, updating the display.
     */
    void draw();

    /**
       * @brief Runs the game loop, handling events, updating, and drawing continuously.
       */
    void run();

    /**
      * @brief Updates the game state based on the elapsed time.
      * @param deltaTime The time elapsed since the last update.
      */
    void update(double deltaTime);

    /**
     * @brief Adds a CollisionSprite to the game, registering it for updates and collisions.
     * @param sprite A shared ptr reference to the CollisionSprite to be added.
     */
    void addEntity(const std::shared_ptr<Entity>& sprite);

    [[nodiscard]] bool canMoveTo(const Entity& entity) const override;

    /**
     * @brief Constructs a Game instance, initializing SDL and creating a window.
     */
    Game();

    /**
     * @brief Destructs the Game instance, freeing resources.
     */
    ~Game() override;
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
