#pragma once

#include "Sprite.h"
#include "Textures.h"
#include "Window.h"
#include "Vector2.h"
#include <cmath>
#include <memory>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <limits>
#include <queue>
#include <unordered_set>

class Tile : public Sprite
{
public:
    enum class TileCode
    {
        BareGrass = 0,
        ShortGrass,
        TallGrass
    };

    Tile(TileCode tileCode, const std::shared_ptr<Sprite>& residingEntity = nullptr);

    void setResidingEntity(const std::shared_ptr<Sprite>& residingEntity);
    [[nodiscard]] std::shared_ptr<Sprite> getResidingEntity() const;

private:
    TileCode m_tileCode;
    std::shared_ptr<Sprite> m_residingEntity;
};

class TileMap : public Observer
{
public:
    TileMap(int rows, int columns);

    void print();
    void placeTile(const std::shared_ptr<Sprite>& entity, const Vector2<int>& position);
    void pushTile(const std::shared_ptr<Sprite>& entity, const Vector2<int>& playerPosition);
    [[nodiscard]] static Vector2<int> enclosingTileCoordinates(const Vector2<int>& position);
    [[nodiscard]] static Vector2<int> enclosingTileCenterCoordinates(const Vector2<int>& position, const SDL_Rect& spriteDimensions);
    [[nodiscard]] std::shared_ptr<Tile> getEnclosingTile(const Vector2<int>& position) const;
    [[nodiscard]] std::shared_ptr<Tile> getTile(int x, int y) const;
    [[nodiscard]] std::vector<std::shared_ptr<Tile>> getTiles() const;
    [[nodiscard]] Vector2<int> getTileCoordinates(const std::shared_ptr<Tile>& tile) const;
    [[nodiscard]] std::shared_ptr<Tile> getClosestAvailableAdjacentTile(const Vector2<int>& tilePosition, const Vector2<int>& playerCoordinates) const;

private:
    std::unordered_map<Vector2<int>, std::shared_ptr<Tile>> m_tileMap;
};

struct AStarNode
{
    AStarNode(std::shared_ptr<Tile> tile, std::shared_ptr<AStarNode> parent, double gCost, double hCost)
        : m_tile(std::move(tile)),
        m_parent(std::move(parent)),
        m_gValue(gCost),
        m_hValue(hCost) {}

    double getFValue() const { return m_gValue + m_hValue; }
    double getGValue() const { return m_gValue; }
    double getHValue() const { return m_hValue; }
    std::shared_ptr<Tile> getTile() { return m_tile; }
    std::shared_ptr<AStarNode> getParent() { return m_parent; }
    bool operator>(const AStarNode& other) const { return getFValue() > other.getFValue(); }

private:
    std::shared_ptr<Tile> m_tile;
    std::shared_ptr<AStarNode> m_parent;
    double m_gValue;
    double m_hValue;
};

class AStar
{
public:
    std::vector<std::shared_ptr<Tile>> getPathToTile(const TileMap& map, const Vector2<int>& start, const Vector2<int>& goal);

private:
    static std::vector<std::shared_ptr<Tile>> reconstructPath(const std::shared_ptr<AStarNode>& node);
    static double heuristic(const Vector2<int>& a, const Vector2<int>& b);
    static std::vector<std::shared_ptr<Tile>> getNeighborTiles(const TileMap& map, const std::shared_ptr<Tile>& tile);
};