#pragma once
#include "Vector2.h"

namespace Window
{
    static constexpr int BOARD_COLUMNS = 7;
    static constexpr int BOARD_ROWS = 5;
    static constexpr Vector2<int> TILE_DIMENSIONS = { 86, 64 };
    static constexpr Vector2<int> WINDOW_DIMENSIONS{ TILE_DIMENSIONS.x * BOARD_COLUMNS, TILE_DIMENSIONS.y * BOARD_ROWS };
}
