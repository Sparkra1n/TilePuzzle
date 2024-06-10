#pragma once

#include <array>
#include <iostream>
#include <vector>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include "SDLExceptions.h"
#include "Counter.h"
#include "EntityStateTracker.h"
#include "Sprite.h"
#include "Textures.h"
#include "Tile.h"
#include "Vector2.h"
#include "Player.h"
#include "Slab.h"
#include "Renderer.h"
#include "TileMap.h"

//TODO: Each Tile should point to the Entity that is sitting on it
//TODO: When a Sprite's onClick fn is called, it should be passed as a ClickEvent to its observer.

class Game final : public Observer
{
public:
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    static Game& get() { static Game instance; return instance; }
    void draw();
    void run();
    void update(double deltaTime);
    void addBackgroundEntity(const std::shared_ptr<Entity>& entity);
    void addForegroundEntity(const std::shared_ptr<Entity>& entity);
    //bool canMoveTo(const Entity& entity, Vector2<double> potentialPosition) const override;

private:
    Game();
    ~Game() override;
    TileMap m_tileMap;
    std::vector<std::shared_ptr<Entity>> m_backgroundEntities;   // Collection of background entities in the game
    std::vector<std::shared_ptr<Entity>> m_foregroundEntities;   // Collection of foreground entities in the game
    std::shared_ptr<Sprite> m_cursor;
    std::shared_ptr<Sprite> m_mouse;
    std::shared_ptr<Player> m_player;                            // Player sprite
    std::shared_ptr<Sprite> m_rectangle;                         // Example sprite
    std::shared_ptr<Sprite> m_projectile;                        // Example Slab
    SDL_Window* m_window{};                                      // SDL window instance
    SDL_Event m_windowEvent{};                                   // SDL event for window handling
    std::unique_ptr<Renderer> m_renderer;
    EntityStateTracker m_hoverTracker;
};
