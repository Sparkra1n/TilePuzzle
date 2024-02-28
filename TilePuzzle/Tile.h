#pragma once
#include <array>

namespace Tile
{
    constexpr int BOARD_COLUMNS = 7;// 14;
    constexpr int BOARD_ROWS = 5; //10;
    constexpr Vector2 TILE_DIMENSIONS = { 86, 64 };
    constexpr Vector2 WINDOW_DIMENSIONS{ TILE_DIMENSIONS.x * BOARD_COLUMNS, TILE_DIMENSIONS.y * BOARD_ROWS };
    constexpr const char* GRASS_SPRITE_PATH = "C:/Users/spark/Documents/Visual Studio 2022/Projects/TilePuzzle/TilePuzzle/sprites/grass.bmp";

    enum class TileCode
    {
        BareGrass = 0,
        ShortGrass,
        TallGrass,
    };

    typedef std::array<std::array<std::shared_ptr<Sprite<NoCollision>>, BOARD_COLUMNS>, BOARD_ROWS> TileMap;
}