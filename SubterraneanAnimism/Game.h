#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include "SDL.h"
#include "SDL_image.h"
#include "SDLExceptions.h"
#include "Sprite.h"
#include "CollisionObserver.h"

class Game : public CollisionObserver
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
    void update(double deltaTime) const;

    /**
     * @brief Adds a CollisionSprite to the game, registering it for updates and collisions.
     * @param sprite A reference to the CollisionSprite to be added.
     */
    void addSprite(CollisionSprite& sprite);

    //void notifyCollision(CollisionSprite* sprite, double& potentialX, double& potentialY) override;

    bool canMoveTo(CollisionSprite& collisionSprite, double potentialX, double potentialY) const override;

    void updateSprites();

    /**
     * @brief Constructs a Game instance, initializing SDL and creating a window.
     */
    Game();

    /**
     * @brief Destructs the Game instance, freeing resources.
     */
    ~Game() override;
private:
    std::vector<std::shared_ptr<Sprite>> m_sprites; // Collection of sprites in the game.
	std::shared_ptr<PlayerSprite> m_player;         // Player sprite
    SDL_Window* m_window{};                         // SDL window instance
    SDL_Surface* m_windowSurface{};                 // SDL window surface
    SDL_Event m_windowEvent{};                      // SDL event for window handling

    //SDL_Renderer* m_renderer{};
    //SDL_Event m_event{};

    int m_screenWidth = 640;
    int m_screenHeight = 480;
};
