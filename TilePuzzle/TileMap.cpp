#include "TileMap.h"

Tile::Tile(TileCode tileCode, const std::shared_ptr<Sprite>& residingEntity)
    : Sprite(Textures::GRASS_SPRITE_PATH), m_tileCode(tileCode), m_residingEntity(residingEntity) {}

void Tile::setResidingEntity(const std::shared_ptr<Sprite>& residingEntity) { m_residingEntity = residingEntity; }
std::shared_ptr<Sprite> Tile::getResidingEntity() const { return m_residingEntity; }

TileMap::TileMap(int rows, int columns)
{
    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < columns; ++j)
        {
            auto tile = std::make_shared<Tile>(Tile::TileCode::BareGrass);
            Vector2 coords = { j * Window::TILE_DIMENSIONS.x, i * Window::TILE_DIMENSIONS.y };
            tile->setCoordinates(coords);
            m_tileMap[{j, i}] = tile;
        }
    }
}

Vector2<int> TileMap::enclosingTileCoordinates(const Vector2<int>& position)
{
    int x = position.x - (position.x % Window::TILE_DIMENSIONS.x);
    int y = position.y - (position.y % Window::TILE_DIMENSIONS.y);
    return { x, y };
}

Vector2<int> TileMap::enclosingTileCenterCoordinates(const Vector2<int>& position, const SDL_Rect& spriteDimensions)
{
    int x = position.x - (position.x % Window::TILE_DIMENSIONS.x) + (Window::TILE_DIMENSIONS.x / 2) - (spriteDimensions.w / 2);
    int y = position.y - (position.y % Window::TILE_DIMENSIONS.y) + (Window::TILE_DIMENSIONS.y / 2) - (spriteDimensions.h / 2);
    return { x, y };
}

std::shared_ptr<Tile> TileMap::getEnclosingTile(const Vector2<int>& position) const
{
    auto tileCoordinates = enclosingTileCoordinates(position);
    int xIndex = tileCoordinates.x / Window::TILE_DIMENSIONS.x;
    int yIndex = tileCoordinates.y / Window::TILE_DIMENSIONS.y;
    if (xIndex >= Window::BOARD_COLUMNS || yIndex >= Window::BOARD_ROWS)
        return nullptr;
    return m_tileMap.at({ xIndex, yIndex });
}

std::shared_ptr<Tile> TileMap::getTile(int x, int y) const
{
    if (x >= Window::BOARD_COLUMNS || y >= Window::BOARD_ROWS)
        return nullptr;
    return m_tileMap.at({ x, y });
}

std::vector<std::shared_ptr<Tile>> TileMap::getTiles() const
{
    std::vector<std::shared_ptr<Tile>> tiles;
    for (const auto& pair : m_tileMap)
    {
        tiles.push_back(pair.second);
    }
    return tiles;
}

void TileMap::placeTile(const std::shared_ptr<Sprite>& entity, const Vector2<int>& position)
{
    Vector2<int> tileCoordinates = enclosingTileCoordinates(position);
    std::shared_ptr<Tile> newTile = getEnclosingTile(tileCoordinates);
    if (!newTile)
        return;

    for (auto& [coords, tile] : m_tileMap)
    {
        if (tile->getResidingEntity() == entity)
        {
            tile->setResidingEntity(nullptr);
            break;
        }
    }

    newTile->setResidingEntity(entity);
    newTile->setCoordinates(tileCoordinates);
}

void TileMap::pushTile(const std::shared_ptr<Sprite>& entity, const Vector2<int>& playerPosition)
{
    std::shared_ptr<Tile> playerTile = getEnclosingTile(playerPosition);
    std::shared_ptr<Tile> entityTile = getEnclosingTile(entity->getCoordinates());

    if (!playerTile || !entityTile)
        return;

    int dX = playerTile->getCoordinates().x - entityTile->getCoordinates().x;
    int dY = playerTile->getCoordinates().y - entityTile->getCoordinates().y;

    if (std::abs(dX) > Window::TILE_DIMENSIONS.x || std::abs(dY) > Window::TILE_DIMENSIONS.y)
        return;

    int dirX = 0, dirY = 0;
    if (std::abs(dX) > std::abs(dY))
    {
        dirX = (dX > 0) ? -1 : 1;
    }
    else
    {
        dirY = (dY > 0) ? -1 : 1;
    }

    int currentX = entityTile->getCoordinates().x / Window::TILE_DIMENSIONS.x;
    int currentY = entityTile->getCoordinates().y / Window::TILE_DIMENSIONS.y;

    std::shared_ptr<Tile> targetTile = nullptr;
    while (true)
    {
        int nextX = currentX + dirX;
        int nextY = currentY + dirY;

        if (nextX < 0 || nextX >= Window::BOARD_COLUMNS || nextY < 0 || nextY >= Window::BOARD_ROWS)
            break;

        std::shared_ptr<Tile> nextTile = m_tileMap.at({ nextX, nextY });
        if (nextTile->getResidingEntity() != nullptr)
            break;

        targetTile = nextTile;
        currentX = nextX;
        currentY = nextY;
    }

    if (targetTile)
    {
        entityTile->setResidingEntity(nullptr);
        targetTile->setResidingEntity(entity);
        Vector2 destination = enclosingTileCenterCoordinates(targetTile->getCoordinates(), entity->getSdlRect());
        entity->goTo(destination);
    }
}

std::shared_ptr<Tile> TileMap::getClosestAvailableAdjacentTile(const Vector2<int>& tilePosition, const Vector2<int>& playerCoordinates) const
{
    static const std::vector<Vector2<int>> directions =
    {
        { 0, -1 },
        { -1, 0 },
        { 1, 0 },
        { 0, 1 }
    };

    Vector2<int> coordinates = enclosingTileCoordinates(tilePosition);
    int tileX = coordinates.x / Window::TILE_DIMENSIONS.x;
    int tileY = coordinates.y / Window::TILE_DIMENSIONS.y;

    std::shared_ptr<Tile> closestTile = nullptr;
    double minDistance = std::numeric_limits<double>::max();

    for (const auto& dir : directions)
    {
        int newX = tileX + dir.x;
        int newY = tileY + dir.y;

        if (newX >= 0 && newX < Window::BOARD_COLUMNS && newY >= 0 && newY < Window::BOARD_ROWS)
        {
            std::shared_ptr<Tile> adjacentTile = m_tileMap.at({ newX, newY });

            if (adjacentTile->getResidingEntity() == nullptr)
            {
                double distance = std::sqrt(std::pow(newX * Window::TILE_DIMENSIONS.x - playerCoordinates.x, 2) +
                    std::pow(newY * Window::TILE_DIMENSIONS.y - playerCoordinates.y, 2));

                if (distance < minDistance)
                {
                    closestTile = adjacentTile;
                    minDistance = distance;
                }
            }
        }
    }

    return closestTile;
}

Vector2<int> TileMap::getTileCoordinates(const std::shared_ptr<Tile>& tile) const
{
    auto it = std::find_if(m_tileMap.begin(), m_tileMap.end(),
        [&tile](const auto& p) { return p.second == tile; });

    if (it != m_tileMap.end())
        return it->first;

    return { -1, -1 };
}

void TileMap::print()
{
    for (auto& [position, tile] : m_tileMap)
    {
        std::cout << "{ " << position << ", " << (tile->getResidingEntity() == nullptr ? "1" : "0") << " }\n";
    }
}

std::vector<std::shared_ptr<Tile>> AStar::reconstructPath(const std::shared_ptr<AStarNode>& node)
{
    std::vector<std::shared_ptr<Tile>> path;
    auto current = node;
    while (current)
    {
        path.push_back(current->getTile());
        current = current->getParent();
    }
    std::reverse(path.begin(), path.end());
    return path;
}

double AStar::heuristic(const Vector2<int>& a, const Vector2<int>& b)
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<std::shared_ptr<Tile>> AStar::getNeighborTiles(const TileMap& map, const std::shared_ptr<Tile>& tile)
{
    static const std::vector<Vector2<int>> directions =
    {
        { 0, -1 },
        { -1, 0 },
        { 1, 0 },
        { 0, 1 }
    };

    const Vector2<int> position = map.getTileCoordinates(tile);
    std::vector<std::shared_ptr<Tile>> neighbors;
    for (const auto& dir : directions)
    {
        auto neighbor = map.getTile(position.x + dir.x, position.y + dir.y);
        if (neighbor)
            neighbors.push_back(neighbor);
    }
    return neighbors;
}

std::vector<std::shared_ptr<Tile>> AStar::getPathToTile(
    const TileMap& map,
    const Vector2<int>& start,
    const Vector2<int>& goal)
{
    auto startTile = map.getTile(start.x, start.y);
    auto goalTile = map.getTile(goal.x, goal.y);
    if (!startTile || !goalTile) return {};

    auto compare = [](const std::shared_ptr<AStarNode>& a, const std::shared_ptr<AStarNode>& b) -> bool
        {
            return *a > *b;
        };

    std::priority_queue<std::shared_ptr<AStarNode>, std::vector<std::shared_ptr<AStarNode>>, decltype(compare)> openList(compare);
    std::unordered_map<std::shared_ptr<Tile>, std::shared_ptr<AStarNode>> allNodes;
    std::unordered_set<std::shared_ptr<Tile>> closedList;

    auto startNode = std::make_shared<AStarNode>(startTile, nullptr, 0.0, heuristic(start, goal));
    openList.push(startNode);
    allNodes[startTile] = startNode;

    while (!openList.empty())
    {
        auto current = openList.top();
        openList.pop();
        closedList.insert(current->getTile());

        if (current->getTile() == goalTile)
            return reconstructPath(current);

        auto neighbors = getNeighborTiles(map, current->getTile());
        for (const auto& neighbor : neighbors)
        {
            if (closedList.count(neighbor))
                continue;

            double tentativeG = current->getGValue() + 1.0; // Assuming uniform cost for simplicity
            double hCost = heuristic(neighbor->getCoordinates(), goal);
            auto neighborNode = std::make_shared<AStarNode>(neighbor, current, tentativeG, hCost);

            if (allNodes.find(neighbor) == allNodes.end() || tentativeG < allNodes[neighbor]->getGValue()) {
                openList.push(neighborNode);
                allNodes[neighbor] = neighborNode;
            }
        }
    }
    return {};
}