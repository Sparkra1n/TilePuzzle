#pragma once

#include <array>
#include <iostream>
#include <vector>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include "SDLExceptions.h"
#include "FPSCounter.h"
#include "Sprite.h"
#include "Tile.h"
#include "Vector2.h"
#include "Player.h"
#include "Projectile.h"
#include "Renderer.h"

class Game : public Observer
{
public:
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;
    static Game& get() { static Game instance; return instance; }
    void draw();
    void run();
    void update(double deltaTime) const;
    void addEntity(const std::shared_ptr<Entity>& entity);
    bool canMoveTo(const Entity& entity, Vector2<double> potentialPosition) const override;
private:
    Game();
    ~Game() override;
    static constexpr const char* PLAYER_SPRITE_PATH = "C:/Users/spark/Documents/Visual Studio 2022/Projects/SubterraneanAnimism/SubterraneanAnimism/sprites/sword.bmp";
    static constexpr const char* CURSOR_SPRITE_PATH = "C:/Users/spark/Documents/Visual Studio 2022/Projects/SubterraneanAnimism/SubterraneanAnimism/sprites/Highlight.bmp";
    std::array<std::array<Tile::TileCode, Tile::BOARD_COLUMNS>, Tile::BOARD_ROWS> m_board{};
    std::shared_ptr<SDL_Texture> m_backgroundTexture;
    std::shared_ptr<SDL_Texture> m_foregroundTexture;
    std::vector<std::shared_ptr<Entity>> m_entities;             // Collection of sprites in the game
    std::shared_ptr<Sprite<NoCollision>> m_cursor;
    std::shared_ptr<Player> m_player;                            // Player sprite
    Tile::TileMap m_tileMap;
    std::shared_ptr<Sprite<NoCollision>> m_rectangle;            // Example sprite
    std::shared_ptr<Sprite<NoCollision>> m_projectile;           // Example Projectile
    std::shared_ptr<Sprite<PolygonCollision>> m_curvy;
    SDL_Window* m_window{};                                      // SDL window instance
    SDL_Event m_windowEvent{};                                   // SDL event for window handling
    std::unique_ptr<Renderer> m_renderer;
};
