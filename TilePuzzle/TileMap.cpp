#include "TileMap.h"

Tile::TileCode Tile::charToTileCode(const char* c)
{
    int value = atoi(c);
    if (value >= static_cast<int>(TileCode::First) && value <= static_cast<int>(TileCode::Last))
        return static_cast<TileCode>(value);

    throw std::invalid_argument("Invalid tile code value");
}

Tile::Tile(TileCode tileCode, SDL_Renderer* cacheRenderer, const std::shared_ptr<Sprite>& residingEntity, const bool isGoalTile)
    : Sprite(Textures::GRASS_SPRITE_PATH, cacheRenderer), m_tileCode(tileCode), m_residingEntity(residingEntity), m_isGoalTile(isGoalTile)
{
    std::cout << cacheRenderer;
}

void Tile::setResidingEntity(const std::shared_ptr<Sprite>& residingEntity) { m_residingEntity = residingEntity; }
std::shared_ptr<Sprite> Tile::getResidingEntity() const { return m_residingEntity; }

std::vector<std::string> TileMap::splitString(const std::string& s, const char delimiter)
{
    std::vector<std::string> slices;
    std::istringstream stream(s);
    std::string token;

    while (std::getline(stream, token, delimiter)) 
        slices.push_back(token);

    return slices;
}

// Improved verifyBoard with single file traversal and no repeated splitting
bool TileMap::verifyBoard(std::ifstream& file, int& rows, int& columns)
{
    std::string line;
    int lineNumber = 0;
    //std::vector<std::vector<std::string>> allRows;

    // Read file once and store all rows
    while (std::getline(file, line)) 
    {
        std::vector<std::string> currentLineElements = splitString(line, '-');
        if (lineNumber == 0) {
            columns = static_cast<int>(currentLineElements.size());
        }
        else if (static_cast<int>(currentLineElements.size()) != columns) {
            std::cerr << "Inconsistent row length found on line " << lineNumber + 1 << "!\n";
            return false;
        }

        //allRows.push_back(std::move(currentLineElements));
        ++lineNumber;
    }

    rows = lineNumber;

    if (columns > MAX_COLUMNS || rows > MAX_ROWS) {
        std::cerr << "Board exceeds max dimensions!\n";
        return false;
    }

    return true;
}

TileMap::TileMap(const std::string& path, SDL_Renderer* cacheRenderer) : m_cacheRenderer(cacheRenderer)
{
    std::ifstream boardFile(path);

    if (!boardFile.is_open())
        throw std::runtime_error("Error: Could not open the input file: " + path);

    if (!verifyBoard(boardFile, m_boardRows, m_boardColumns))
        throw std::runtime_error("Error: Invalid board file: " + path);

    // Reset file to start after verification
    boardFile.clear();
    boardFile.seekg(0);

    std::string line;
    int rowNumber = 0;
    while (std::getline(boardFile, line)) 
    {
        std::vector<std::string> row = splitString(line, '-');
        for (int i = 0; i < m_boardColumns; ++i) 
        {
            auto tile = std::make_shared<Tile>(Tile::charToTileCode(row[i].c_str()), m_cacheRenderer);
            Vector2 coords = { i * Tile::TILE_DIMENSIONS.x, rowNumber * Tile::TILE_DIMENSIONS.y };
            tile->setPosition(coords);
            m_tileMap[{i, rowNumber}] = tile;
        }
        ++rowNumber;
    }
}

Vector2<int> TileMap::getEnclosingTilePosition(const Vector2<int>& position)
{
    int x = position.x - (position.x % Tile::TILE_DIMENSIONS.x);
    int y = position.y - (position.y % Tile::TILE_DIMENSIONS.y);
    return { x, y };
}

Vector2<int> TileMap::getEnclosingTileCenterPosition(const Vector2<int>& position, const SDL_Rect& spriteDimensions)
{
    int x = position.x - (position.x % Tile::TILE_DIMENSIONS.x) + (Tile::TILE_DIMENSIONS.x / 2) - (spriteDimensions.w / 2);
    int y = position.y - (position.y % Tile::TILE_DIMENSIONS.y) + (Tile::TILE_DIMENSIONS.y / 2) - (spriteDimensions.h / 2);
    return { x, y };
}

std::shared_ptr<Tile> TileMap::getEnclosingTile(const Vector2<int>& position) const
{
    auto tileCoordinates = getEnclosingTilePosition(position);
    int xIndex = tileCoordinates.x / Tile::TILE_DIMENSIONS.x;
    int yIndex = tileCoordinates.y / Tile::TILE_DIMENSIONS.y;
    if (xIndex >= m_boardColumns || yIndex >= m_boardRows)
        return nullptr;
    return m_tileMap.at({ xIndex, yIndex });
}

std::shared_ptr<Tile> TileMap::getTile(int x, int y) const
{
    if (x >= m_boardColumns || y >= m_boardRows || x < 0 || y < 0)
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
    Vector2<int> tileCoordinates = getEnclosingTilePosition(position);
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
    newTile->setPosition(tileCoordinates);
}

void TileMap::pushTile(const std::shared_ptr<Sprite>& entity, const Vector2<int>& playerPosition) const
{
    std::shared_ptr<Tile> playerTile = getEnclosingTile(playerPosition);
    std::shared_ptr<Tile> entityTile = getEnclosingTile(entity->getPosition());

    if (!playerTile || !entityTile)
        return;

    int dX = playerTile->getPosition().x - entityTile->getPosition().x;
    int dY = playerTile->getPosition().y - entityTile->getPosition().y;

    if (std::abs(dX) > Tile::TILE_DIMENSIONS.x || std::abs(dY) > Tile::TILE_DIMENSIONS.y)
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

    int currentX = entityTile->getPosition().x / Tile::TILE_DIMENSIONS.x;
    int currentY = entityTile->getPosition().y / Tile::TILE_DIMENSIONS.y;

    std::shared_ptr<Tile> targetTile = nullptr;
    while (true)
    {
        int nextX = currentX + dirX;
        int nextY = currentY + dirY;

        if (nextX < 0 || nextX >= m_boardColumns || nextY < 0 || nextY >= m_boardRows)
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
        Vector2 destination = getEnclosingTileCenterPosition(targetTile->getPosition(), entity->getSdlRect());
        entity->walk({destination});
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

    Vector2<int> coordinates = getEnclosingTilePosition(tilePosition);
    int tileX = coordinates.x / Tile::TILE_DIMENSIONS.x;
    int tileY = coordinates.y / Tile::TILE_DIMENSIONS.y;

    std::shared_ptr<Tile> closestTile = nullptr;
    double minDistance = std::numeric_limits<double>::max();

    for (const auto& dir : directions)
    {
        int newX = tileX + dir.x;
        int newY = tileY + dir.y;

        if (newX >= 0 && newX < m_boardColumns && newY >= 0 && newY < m_boardRows)
        {
            std::shared_ptr<Tile> adjacentTile = m_tileMap.at({ newX, newY });

            if (adjacentTile->getResidingEntity() == nullptr)
            {
                double distance = std::sqrt(std::pow(newX * Tile::TILE_DIMENSIONS.x - playerCoordinates.x, 2) +
                    std::pow(newY * Tile::TILE_DIMENSIONS.y - playerCoordinates.y, 2));

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

std::vector<std::shared_ptr<Tile>> TileMap::reversePath(const std::shared_ptr<AStarNode>& node)
{
    std::vector<std::shared_ptr<Tile>> path;
    std::shared_ptr<TileMap::AStarNode> current = node;
    while (current)
    {
        path.push_back(current->getTile());
        current = current->getParent();
    }
    std::reverse(path.begin(), path.end());
    return path;
}

double TileMap::heuristic(const Vector2<int>& a, const Vector2<int>& b)
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<std::shared_ptr<Tile>> TileMap::getNeighborTiles(const std::shared_ptr<Tile>& tile) const
{
    static const std::vector<Vector2<int>> directions =
    {
        { 0, -1 },
        { -1, 0 },
        { 1, 0 },
        { 0, 1 }
    };

    const Vector2<int> position = getTileCoordinates(tile);
    std::vector<std::shared_ptr<Tile>> neighbors;
    for (const auto& dir : directions)
    {
        auto neighbor = getTile(position.x + dir.x, position.y + dir.y);
        if (neighbor)
            neighbors.push_back(neighbor);
    }
    return neighbors;
}

std::vector<std::shared_ptr<Tile>> TileMap::getPathToTile(const std::shared_ptr<Tile>& startTile, const std::shared_ptr<Tile>& goalTile) const
{
    if (!startTile || !goalTile) 
        return {};

    auto compare = [](const std::shared_ptr<AStarNode>& a, const std::shared_ptr<AStarNode>& b) -> bool
    {
        return *a > *b;
    };

    std::priority_queue<std::shared_ptr<AStarNode>, std::vector<std::shared_ptr<AStarNode>>, decltype(compare)> openList(compare);
    std::unordered_map<std::shared_ptr<Tile>, std::shared_ptr<AStarNode>> allNodes;
    std::unordered_set<std::shared_ptr<Tile>> closedList;

    auto startNode = std::make_shared<AStarNode>(startTile,
        nullptr, 
        0.0, 
        heuristic(getTileCoordinates(startTile), getTileCoordinates(goalTile))
    );

    openList.push(startNode);
    allNodes[startTile] = startNode;

    while (!openList.empty())
    {
	    std::shared_ptr<AStarNode> current = openList.top();
        openList.pop();
        closedList.insert(current->getTile());

        if (current->getTile() == goalTile)
            return reversePath(current);

        std::vector<std::shared_ptr<Tile>> neighbors = getNeighborTiles(current->getTile());
        for (const auto& neighbor : neighbors)
        {
            if (closedList.count(neighbor) || neighbor->getResidingEntity() != nullptr)
                continue;

            double tentativeG = current->getGValue() + 1.0;
            double hCost = heuristic(getTileCoordinates(neighbor), getTileCoordinates(goalTile));
            auto neighborNode = std::make_shared<AStarNode>(neighbor, current, tentativeG, hCost);

            if (allNodes.find(neighbor) == allNodes.end() || tentativeG < allNodes[neighbor]->getGValue()) 
            {
                openList.push(neighborNode);
                allNodes[neighbor] = neighborNode;
            }
        }
    }
    return {};
}

bool TileMap::isSolved()
{
	for (auto& [_, tile] : m_tileMap)
	{
        if (tile->isGoalTile())
            if (tile->getResidingEntity() == nullptr)
                return false;
	}
    return true;
}
