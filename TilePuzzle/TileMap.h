#pragma once

#include "Tile.h"
#include "Window.h"

class TileMap : public Observer
{
public:
    /**
 * @brief Returns the top left coordinate of the tile enclosing the mouse.
 * @param position
 * @return Vector2<int>
 */
	Vector2<int> enclosingTileCoordinates(Vector2<int> position)
    {
        int x = position.x - position.x % Window::TILE_DIMENSIONS.x;
        int y = position.y - position.y % Window::TILE_DIMENSIONS.y;
        return { x, y };
    }

    /**
     * @brief Returns the offset coordinate to place a sprite exactly in the center of a tile.
     * @param position
     * @param spriteDimensions
     * @return Vector2<int>
     */
	Vector2<int> enclosingTileCenterCoordinates(Vector2<int> position, SDL_Rect spriteDimensions)
    {
        int x = position.x - position.x % Window::TILE_DIMENSIONS.x + Window::TILE_DIMENSIONS.x / 2 - spriteDimensions.w / 2;
        int y = position.y - position.y % Window::TILE_DIMENSIONS.y + Window::TILE_DIMENSIONS.y / 2 - spriteDimensions.h / 2;
        return { x, y };
    }

    const Tile* getEnclosingTile(Vector2<int> position) const override
    {
        uint32_t xIndex = position.x / Window::TILE_DIMENSIONS.x;
        uint32_t yIndex = position.y / Window::TILE_DIMENSIONS.y;
        auto a =  m_tiles[yIndex][xIndex];
        return a.get();
    }

    std::shared_ptr<Entity> getTile(uint32_t x, uint32_t y)
	{
        if (x > Window::BOARD_COLUMNS || y > Window::BOARD_ROWS)
            return nullptr;
        return m_tiles[y][x];
	}

    enum class Direction
    {
        Nowhere = 0,
        North,
        South,
        East,
        West
    };

    using Tiles = std::array<std::array<std::shared_ptr<Tile>, Window::BOARD_COLUMNS>, Window::BOARD_ROWS>;

    [[nodiscard]] Tiles* getTiles() { return &m_tiles; }

private:
    Tiles m_tiles;
};
