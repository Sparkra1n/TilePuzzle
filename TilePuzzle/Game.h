#pragma once

#include <array>
#include <iostream>
#include <vector>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include "SDLExceptions.h"
#include "Counter.h"
#include "Sprite.h"
#include "Sprites.h"
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
    void addBackgroundEntity(const std::shared_ptr<Entity>& entity);
    void addForegroundEntity(const std::shared_ptr<Entity>& entity);
    bool canMoveTo(const Entity& entity, Vector2<double> potentialPosition) const override;

    /**
     * @brief Returns the top left coordinate of the tile enclosing the mouse.
     * @param position
     * @return Vector2<int>
     */
    static Vector2<int> enclosingTile(Vector2<int> position);

    /**
     * @brief Returns the offset coordinate to place a sprite exactly in the center of a tile.
     * @param position
     * @param spriteDimensions
     * @return Vector2<int>
     */
    static Vector2<int> enclosingTileCenter(Vector2<int> position, SDL_Rect spriteDimensions);
private:
    Game();
    ~Game() override;
    std::array<std::array<Tile::TileCode, Tile::BOARD_COLUMNS>, Tile::BOARD_ROWS> m_board{};
    Tile::TileMap m_tileMap;
    std::vector<std::shared_ptr<Entity>> m_backgroundEntities;   // Collection of background entities in the game
    std::vector<std::shared_ptr<Entity>> m_foregroundEntities;   // Collection of foreground entities in the game
    std::shared_ptr<Sprite<NoCollision>> m_cursor;
    std::shared_ptr<Sprite<RectangularCollision>> m_mouse;
    std::shared_ptr<Player> m_player;                            // Player sprite
    std::shared_ptr<Sprite<RectangularCollision>> m_rectangle;   // Example sprite
    std::shared_ptr<Sprite<NoCollision>> m_projectile;           // Example Projectile
    SDL_Window* m_window{};                                      // SDL window instance
    SDL_Event m_windowEvent{};                                   // SDL event for window handling
    std::unique_ptr<Renderer> m_renderer;
};
