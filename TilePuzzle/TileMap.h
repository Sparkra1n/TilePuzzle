#pragma once
#include "Sprite.h"
#include "Textures.h"
#include "Vector2.h"
#include <cmath>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <limits>
#include <queue>
#include <unordered_set>
#include <sstream>
#include <fstream>
#include <vector>

class Tile : public Sprite
{
public:
    static constexpr Vector2<int> TILE_DIMENSIONS = { 86, 64 };

    enum class TileCode
    {
        First = 0,
        Empty = First,
        Grass,
        Stone,
        Water,
        Last = Water  // Last is always set to the final element
    };

    struct TileCodeTraits
    {
        static constexpr auto first = TileCode::First;
        static constexpr auto last = TileCode::Last;
    };

    Tile(TileCode tileCode, SDL_Renderer* cacheRenderer, const std::shared_ptr<Sprite>& residingEntity = nullptr, bool isGoalTile = false);

    void setAsGoalTile() { m_isGoalTile = true; }
    void setResidingEntity(const std::shared_ptr<Sprite>& residingEntity);
    [[nodiscard]] bool isGoalTile() const { return m_isGoalTile; }
    [[nodiscard]] std::shared_ptr<Sprite> getResidingEntity() const;
    static TileCode charToTileCode(const char* c);
private:
    TileCode m_tileCode;
    std::shared_ptr<Sprite> m_residingEntity;
    bool m_isGoalTile;
};

class TileMap : public Observer
{
public:
    TileMap(const std::string& path, SDL_Renderer* cacheRenderer);

    void print();
    void placeTile(const std::shared_ptr<Sprite>& entity, const Vector2<int>& position);
    void pushTile(const std::shared_ptr<Sprite>& entity, const Vector2<int>& playerPosition) const;
    [[nodiscard]] static Vector2<int> getEnclosingTilePosition(const Vector2<int>& position);
    [[nodiscard]] static Vector2<int> getEnclosingTileCenterPosition(const Vector2<int>& position, const SDL_Rect& spriteDimensions);
    [[nodiscard]] std::shared_ptr<Tile> getEnclosingTile(const Vector2<int>& position) const;
    [[nodiscard]] std::shared_ptr<Tile> getTile(int x, int y) const;
    [[nodiscard]] std::vector<std::shared_ptr<Tile>> getTiles() const;
    [[nodiscard]] Vector2<int> getTileCoordinates(const std::shared_ptr<Tile>& tile) const;
    [[nodiscard]] std::shared_ptr<Tile> getClosestAvailableAdjacentTile(const Vector2<int>& tilePosition, const Vector2<int>& playerCoordinates) const;
    [[nodiscard]] std::vector<std::shared_ptr<Tile>> getPathToTile(const std::shared_ptr<Tile>& startTile, const std::shared_ptr<Tile>& goalTile) const;
    [[nodiscard]] bool isSolved();

    static constexpr int MAX_ROWS = 5;
    static constexpr int MAX_COLUMNS = 7;

private:
    SDL_Renderer* m_cacheRenderer;
    int m_boardRows;
    int m_boardColumns;
    std::unordered_map<Vector2<int>, std::shared_ptr<Tile>> m_tileMap;
    static std::vector<std::string> splitString(const std::string& s, char delimiter);
    static bool verifyBoard(std::ifstream& file, int& rows, int& columns);
    struct AStarNode
    {
        AStarNode(std::shared_ptr<Tile> tile, std::shared_ptr<AStarNode> parent, double gCost, double hCost)
            : m_tile(std::move(tile)),
            m_parent(std::move(parent)),
            m_gValue(gCost),
            m_hValue(hCost) {}

        [[nodiscard]] double getFValue() const { return m_gValue + m_hValue; }
        [[nodiscard]] double getGValue() const { return m_gValue; }
        [[nodiscard]] double getHValue() const { return m_hValue; }
        std::shared_ptr<Tile> getTile() { return m_tile; }
        std::shared_ptr<AStarNode> getParent() { return m_parent; }
        bool operator>(const AStarNode& other) const { return getFValue() > other.getFValue(); }

    private:
        std::shared_ptr<Tile> m_tile;
        std::shared_ptr<AStarNode> m_parent;
        double m_gValue;
        double m_hValue;
    };

    static std::vector<std::shared_ptr<Tile>> reversePath(const std::shared_ptr<AStarNode>& node);
    static double heuristic(const Vector2<int>& a, const Vector2<int>& b);
    [[nodiscard]] std::vector<std::shared_ptr<Tile>> getNeighborTiles(const std::shared_ptr<Tile>& tile) const;
};