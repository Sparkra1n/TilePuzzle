#include "GameBoard.h"
#include <iostream>
#include <cmath>
#include <iomanip>
#include "Factory.h"
#include "Player.h"
#include <iostream>

inline bool operator==(const SDL_Color first, const SDL_Color second) noexcept
{
    return first.r == second.r &&
        first.g == second.g &&
        first.b == second.b &&
        first.a == second.a;
}

inline bool operator!=(const SDL_Color first, const SDL_Color second) noexcept
{
    return !(first == second);
}

SDL_Color SpriteModifier::toSdlColor() const
{
    return
    {
        colorClamp(r),
        colorClamp(g),
        colorClamp(b),
        colorClamp(a)
    };
}

Sprite::Sprite(const char* path, SDL_Renderer* cacheRenderer, const Observer* observer)
    : m_renderFlag(true), m_observer(observer), m_cacheRenderer(cacheRenderer)
{
    m_surfaceOriginal = loadSurface(path);
    m_surface = loadSurface(path);
    m_rect.w = m_surface->w;
    m_rect.h = m_surface->h;
}

Sprite::Sprite(const SDL_Rect rect, const SDL_Color color, SDL_Renderer* cacheRenderer, const Observer* observer)
        : m_renderFlag(true),
          m_rect(rect),
          m_coordinates(rect.x, rect.y),
          m_observer(observer),
          m_cacheRenderer(cacheRenderer)
{
    SDL_Surface* surface = SDL_CreateRGBSurface(0, rect.w, rect.h, 32, 0, 0, 0, 0);
    if (!surface)
        throw SDLImageLoadException(SDL_GetError());

    SDL_FillRect(surface, nullptr, SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));
    m_surfaceOriginal = surface;
    m_surface = SDL_DuplicateSurface(surface);
}

Sprite::Sprite(const Sprite& other)
    : m_renderFlag(other.m_renderFlag),
      m_rect(other.m_rect),
      m_coordinates(other.m_coordinates),
      m_observer(other.m_observer),
      m_cacheRenderer(other.m_cacheRenderer)
{
    m_surfaceOriginal = SDL_ConvertSurface(other.m_surface, other.m_surface->format, 0);
    m_surface = SDL_ConvertSurface(other.m_surface, other.m_surface->format, 0);
    if (!m_surfaceOriginal || !m_surface)
        throw SDLImageLoadException(SDL_GetError());
}

Sprite::~Sprite()
{
    SDL_FreeSurface(m_surface);
    SDL_FreeSurface(const_cast<SDL_Surface*>(m_surfaceOriginal)); // Cast away const for freeing
}

void Sprite::setCoordinates(const Vector2<double> coordinates)
{
    m_coordinates = coordinates;
    m_rect.x = static_cast<int>(coordinates.x);
    m_rect.y = static_cast<int>(coordinates.y);
}

void Sprite::setXCoordinate(const double value)
{
    m_coordinates.x = value;
    m_rect.x = static_cast<int>(value);
}

void Sprite::setYCoordinate(const double value)
{
    m_coordinates.y = value;
    m_rect.y = static_cast<int>(value);
}

uint8_t Sprite::getPixelAlpha(const int x, const int y) const
{
    if (x >= 0 && x < m_surface->w && y >= 0 && y < m_surface->h)
    {
        uint8_t alpha, red, green, blue;
        SDL_GetRGBA(static_cast<uint32_t*>(m_surface->pixels)[y * m_surface->w + x],
            m_surface->format, &red, &green, &blue, &alpha);
        return alpha;
    }
    return 0;
}

SDL_Rect Sprite::getSdlRect() const
{
    return m_rect;
}

SDL_Color Sprite::generateRandomColor()
{
    static bool isSeeded = false;
    if (!isSeeded)
    {
        std::srand(static_cast<unsigned>(std::time(nullptr)));
        isSeeded = true;
    }

    const uint8_t r = std::rand() % 256;
    const uint8_t g = std::rand() % 256;
    const uint8_t b = std::rand() % 256;
    return { r, g, b, 255 };
}

SDL_Surface* Sprite::getSdlSurface() const
{
    return m_surface;
}

SDL_Texture* Sprite::getCachedTexture() const
{
    return m_texture;
}

SDL_Surface* Sprite::loadSurface(const char* path)
{
    SDL_Surface* surface = SDL_LoadBMP(path);
    if (!surface)
        throw SDLImageLoadException(SDL_GetError());
    return surface;
}

void Sprite::resetSurface()
{
    SDL_FreeSurface(m_surface);
    m_surface = SDL_DuplicateSurface(const_cast<SDL_Surface*>(m_surfaceOriginal));
}

void Sprite::applyModifier(const SpriteModifier& modifier)
{
    //const SpriteModifier deltaColor = SpriteModifier(offset) - m_rgbaOffset;
    // Lock surface to access pixel data
    if (SDL_LockSurface(m_surface) != 0)
        return;
    
    auto* pixels = static_cast<uint32_t*>(m_surface->pixels); // Access pixel data

    const int width = m_surface->w;
    const int height = m_surface->h;

    // Manipulate each pixel
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            const uint32_t pixel = pixels[y * width + x];
            uint8_t r, g, b, a;
            SDL_GetRGBA(pixel, m_surface->format, &r, &g, &b, &a);

            // Apply offset and colorClamp operation to each color component
            r = SpriteModifier::colorClamp(r + modifier.r);
            g = SpriteModifier::colorClamp(g + modifier.g);
            b = SpriteModifier::colorClamp(b + modifier.b);
            a = SpriteModifier::colorClamp(a + modifier.a);

            // Update pixel
            pixels[y * width + x] = SDL_MapRGBA(m_surface->format, r, g, b, a);
        }
    }

    // Unlock surface
    SDL_UnlockSurface(m_surface);
}

bool Sprite::hasCollisionWith(const Entity& other, 
    const Vector2<double> potentialPosition, 
    const CollisionDetectionMethod collisionDetectionMethod) const
{
    switch (collisionDetectionMethod)
    {
    case CollisionDetectionMethod::NoCollision:
        return false;

    case CollisionDetectionMethod::RectangularCollision:
    {
        SDL_Rect selfRect = m_rect;
        selfRect.x = static_cast<int>(potentialPosition.x);
        selfRect.y = static_cast<int>(potentialPosition.y);
        SDL_Rect otherRect = other.getSdlRect();
        return SDL_HasIntersection(&selfRect, &otherRect) == SDL_TRUE;
    }

    case CollisionDetectionMethod::PolygonCollision:
    {
        // TODO: Caching and acquiring slices

        // If the SDL_Rects don't intersect, there's no need for further collision checking
        if (!hasCollisionWith(other, potentialPosition, CollisionDetectionMethod::RectangularCollision))
            return false;


        const auto selfSlices = slice(2);
        const auto otherSlices = other.slice(2);

        // TODO: cache slices and determine relevant slices to check instead of looping through all of them
        for (const auto& otherSlice : otherSlices)
        {
            const SDL_Rect otherSliceCopy = otherSlice;
            for (const auto& selfSlice : selfSlices)
            {
                // Adjust the position of selfSlice based on potentialPosition
                SDL_Rect adjustedSelfSlice = selfSlice;
                adjustedSelfSlice.x += static_cast<int>(potentialPosition.x - other.getSdlRect().x);
                adjustedSelfSlice.y += static_cast<int>(potentialPosition.y - other.getSdlRect().y);

                // Check for intersection between slices
                if (SDL_HasIntersection(&adjustedSelfSlice, &otherSliceCopy) == SDL_TRUE)
                    return true;
            }
        }
        return false;
    }

    default:
        return false;
    }
}

void Sprite::onClick()
{
    std::cout << "(Sprite) notifying observer of the click\n";
}

std::vector<SDL_Rect> Sprite::slice(const int sliceThickness ) const
{
    std::vector<SDL_Rect> slicedRects;

    if (sliceThickness <= 0)
        return slicedRects;

    for (int y = 0; y < m_rect.h; y += sliceThickness)
    {
        int sliceStartX = -1;
        int sliceEndX = -1;

        for (int x = 0; x < m_rect.w; ++x)
        {
            const int alpha = getPixelAlpha(x, y);

            if (alpha > 0)
            {
                if (sliceStartX == -1)
                    sliceStartX = x;

                sliceEndX = x;
            }
            else if (sliceStartX != -1)
            {
                SDL_Rect sliceRect;
                sliceRect.x = sliceStartX + m_rect.x;
                sliceRect.y = y + m_rect.y;
                sliceRect.h = sliceThickness;

                while (sliceEndX < m_rect.w && getPixelAlpha(sliceEndX, y) != 0)
                    ++sliceEndX;

                sliceRect.w = sliceEndX - sliceStartX;
                slicedRects.push_back(sliceRect);

                sliceStartX = -1;
            }
        }

        if (sliceStartX != -1)
        {
            SDL_Rect sliceRect;
            sliceRect.x = sliceStartX + m_rect.x;
            sliceRect.y = y + m_rect.y;
            sliceRect.h = sliceThickness;

            while (m_rect.w && getPixelAlpha(sliceEndX, y) != 0)
                ++sliceEndX;

            sliceRect.w = m_rect.w - sliceStartX;
            slicedRects.push_back(sliceRect);
        }
    }

    return slicedRects;
}

void Sprite::printSlices()
{
    for (const auto& [x, y, w, h] : m_slices)
        std::cout << "SDL_Rect { x: " << x << ", y: " << y << ", w: " << w << ", h: " << h << " }\n";
}

//// Test function
//std::vector<std::shared_ptr<Sprite>> Sprite::processSlices() const
//{
//    std::vector<std::shared_ptr<Sprite>> rects;
//    const auto slices = slice(10);
//    rects.reserve(slices.size());
//    for (const auto& slice : slices)
//        rects.emplace_back(std::make_shared<Sprite>(slice, generateRandomColor()));
//    return rects;
//}

void Sprite::onFocus()
{
    pushModifier({"Cursor", 30, 30, 30, 0 });
}

void Sprite::onBlur()
{
    popModifier();
}

void GameObject::update(const GameState& state)
{
    if (!m_checkpoints.empty())
    {
        Vector2<double> target = m_checkpoints.front();

        // Move horizontally if x coordinates are different
        if (std::abs(target.x - getWindowCoordinates().x) > 1)
        {
            Vector2<double> coordinates = getWindowCoordinates();
            const int sign = coordinates.x < target.x ? 1 : -1;
            coordinates.x += m_speed * state.deltaTime * sign;
            setCoordinates(coordinates);
        }

        // Move vertically if y coordinates are different
        else if (std::abs(target.y - getWindowCoordinates().y) > 1) 
        {
            Vector2<double> coordinates = getWindowCoordinates();
            const int sign = coordinates.y < target.y ? 1 : -1;
            coordinates.y += m_speed * state.deltaTime * sign;
            setCoordinates(coordinates);
        }

        // Reached the checkpoint
        else
        {
            setCoordinates(target);
            m_checkpoints.erase(m_checkpoints.begin());
        }
    }
}

void Sprite::pushModifier(const SpriteModifier& modifier)
{
    m_modifierStack.push_back(modifier);  // Add the modifier to the back
    applyModifiers();                     // Apply all modifiers in order
}

void Sprite::removeModifier(const std::string& name)
{
    for (int i = 0; i != m_modifierStack.size(); ++i)
    {
        if (m_modifierStack[i].name == name)
        {
            m_modifierStack.erase(std::next(m_modifierStack.begin(), i));
            m_cachedTextures.erase(std::next(m_cachedTextures.begin(), i));
            break;
        }
    }
    applyModifiers();
}

SpriteModifier Sprite::popModifier()
{
    if (!m_modifierStack.empty())
    {
        SpriteModifier topModifier = m_modifierStack.back();
        m_modifierStack.pop_back();
        if (m_cachedTextures.size() > 1)
        {
            // Destroy the last cached texture
            SDL_DestroyTexture(m_cachedTextures.back());
            m_cachedTextures.pop_back();
        }
        // Apply remaining modifiers in order
        applyModifiers();
        return topModifier;
    }

    return {};
}

void Sprite::applyModifiers()
{
    // Start clean
     resetSurface();

    // Apply modifiers from front (bottom) to back (top)
     for (const auto& modifier : m_modifierStack)
         applyModifier(modifier);

    // Cache the modified texture for efficient rendering
    cacheTexture();
}

// Create a cached texture after applying all modifiers
void Sprite::cacheTexture()
{
    if (m_texture != nullptr)
        SDL_DestroyTexture(m_texture);

    m_texture = SDL_CreateTextureFromSurface(m_cacheRenderer, m_surface);
    if (!m_texture)
        throw SDLImageLoadException(SDL_GetError());

    // Cache this texture for future use
    m_cachedTextures.push_back(m_texture);
}

Tile::Tile(const std::string& texturePath,
    SDL_Renderer* cacheRenderer,
    const std::shared_ptr<Sprite>& residingEntity,
    bool isGoalTile)
    : Sprite(texturePath.c_str(), cacheRenderer),
    m_residingEntity(residingEntity),
    m_isGoalTile(isGoalTile)
{}

void GameBoard::onClick(const GameState& state)
{
    if (state.mousePosition.x > m_boardBounds.x || state.mousePosition.y > m_boardBounds.y)
        return;

    std::cout << "click\n";
    const Vector2<int> destination = centerScreenCoordinates(state.mousePosition, m_player->getSdlRect());
    std::shared_ptr<Tile> tile = getEnclosingTile(destination);

    // Unoccupied destination tile
    if (tile->getResidingEntity() == nullptr)
    {
        std::vector<std::shared_ptr<Tile>> tiles = getPathToTile(getEnclosingTile(m_player->getWindowCoordinates()), tile);
        std::vector<Vector2<int>> path;
        path.reserve(tiles.size());
        for (const auto& i : tiles)
            path.push_back(centerScreenCoordinates(
                i->getWindowCoordinates(),
                m_player->getSdlRect())
            );
        m_player->walk(path);
    }
    
    //FIXME: Go to a neighboring tile and push the slab
    else
    {
        std::shared_ptr<Tile> nextTileChoice = getClosestAvailableTile(state.mousePosition, m_player->getWindowCoordinates());
        if (nextTileChoice)
        {
            std::vector<std::shared_ptr<Tile>> tiles = getPathToTile(getEnclosingTile(m_player->getWindowCoordinates()), tile);
            std::vector<Vector2<int>> path;
            path.reserve(tiles.size());
            for (const auto& i : tiles)
                path.push_back(centerScreenCoordinates(i->getWindowCoordinates(), m_player->getSdlRect()));

            m_player->walk(path);
            pushTile(getEnclosingTile(destination)->getResidingEntity(), m_player->getWindowCoordinates());
        }
    }
    //m_hoverTracker.getFocused()->onClick();
}

void GameBoard::update(const GameState& state)
{
    Vector2<int> mousePosition = state.mousePosition;

    if (mousePosition.x > m_boardBounds.x)
        mousePosition.x = m_boardBounds.x;

    if (mousePosition.y > m_boardBounds.y)
        mousePosition.y = m_boardBounds.y;

    std::shared_ptr<Tile> hoveredTile = getEnclosingTile(mousePosition);
    std::shared_ptr<Entity> residingEntity = hoveredTile->getResidingEntity();
    if (residingEntity)
    {
        if (residingEntity != m_hoveredEntity)
        {
            residingEntity->onFocus();

            if (m_hoveredEntity)
                m_hoveredEntity->onBlur();
            m_hoveredEntity = residingEntity;
        }
    }
    else
    {
        hoveredTile->onFocus();
        if (m_hoveredEntity)
            m_hoveredEntity->onBlur();
        m_hoveredEntity = hoveredTile;
    }

    m_player->update(state);
}

void Tile::setResidingEntity(const std::shared_ptr<Sprite>& residingEntity)
{
    m_residingEntity = residingEntity;
}

std::shared_ptr<Sprite> Tile::getResidingEntity() const
{
    return m_residingEntity;
}

GameBoard::GameBoard(const std::string& path, const std::shared_ptr<Player>& player, SDL_Renderer* cacheRenderer)
    : m_cacheRenderer(cacheRenderer), m_player(player)
{
    if (!m_player)
        throw std::runtime_error("Player must be initialized");

    readDimensions(path);

    // Determine offsets for matrices in the file
    int offset = 1; // First line is dimensions
    std::vector<std::vector<std::string>> tileKeys = loadMatrix(path, offset, m_boardRows, m_boardColumns);
    offset += m_boardRows;
    std::vector<std::vector<std::string>> immovableKeys = loadMatrix(path, offset, m_boardRows, m_boardColumns);
    offset += m_boardRows;
    std::vector<std::vector<std::string>> movableKeys = loadMatrix(path, offset, m_boardRows, m_boardColumns);

    // Lay tiles on the board
    for (int i = 0; i < tileKeys.size(); ++i)
    {
        std::vector<std::shared_ptr<Tile>> convertedRow;
        for (int j = 0; j < tileKeys[i].size(); ++j) 
        {
            const std::string& textureKey = tileKeys[i][j];
            try {
                auto tile = Factory::create<Tile>(m_cacheRenderer, textureKey);
                tile->setCoordinates(Vector2{ Tile::TILE_DIMENSIONS.x * i, Tile::TILE_DIMENSIONS.y * j });
                convertedRow.push_back(tile);
            }
            catch (const std::out_of_range&) {
                throw std::runtime_error("Invalid tile texture key: " + textureKey);
            }
        }
        m_tiles.push_back(convertedRow);
    }

    // Place immovable objects
    for (int i = 0; i < immovableKeys.size(); ++i) 
    {
        for (int j = 0; j < immovableKeys[i].size(); ++j) 
        {
            const std::string& textureKey = immovableKeys[i][j];
            if (textureKey != "Empty") 
            {
                try {
                    auto gameObject = Factory::create<GameObject>(
                        m_cacheRenderer, textureKey, GameObject::PhysicsType::Immovable
                    );
                    gameObject->setCoordinates(Vector2{ Tile::TILE_DIMENSIONS.x * i, Tile::TILE_DIMENSIONS.y * j });
                }
                catch (const std::out_of_range&) {
                    throw std::runtime_error("Invalid immovable texture key: " + textureKey);
                }
            }
        }
    }

    // Place movable objects
    for (int i = 0; i < movableKeys.size(); ++i) 
    {
        for (int j = 0; j < movableKeys[i].size(); ++j) 
        {
            const std::string& textureKey = movableKeys[i][j];
            if (textureKey != "Empty") 
            {
                try {
                    auto gameObject = Factory::create<GameObject>(
                        m_cacheRenderer, textureKey, GameObject::PhysicsType::Movable, 1.0
                    );
                    gameObject->setCoordinates(Vector2{ Tile::TILE_DIMENSIONS.x * i, Tile::TILE_DIMENSIONS.y * j });
                }
                catch (const std::out_of_range&) {
                    throw std::runtime_error("Invalid movable texture key: " + textureKey);
                }
            }
        }
    }
}

void GameBoard::readDimensions(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Could not open file: " + path);

    std::string line;
    std::getline(file, line);
    std::stringstream dimensionsStream(line);
    dimensionsStream >> m_boardRows;
    dimensionsStream.ignore(1);
    dimensionsStream >> m_boardColumns;

    if (m_boardRows <= 0 || m_boardColumns <= 0 || m_boardRows > MAX_ROWS || m_boardColumns > MAX_COLUMNS)
        throw std::runtime_error("Invalid board dimensions in file: " + path);

    m_boardBounds = 
    {
        (m_boardRows) * Tile::TILE_DIMENSIONS.x - 5,
        (m_boardColumns) * Tile::TILE_DIMENSIONS.y - 5
    };
    std::cout << m_boardBounds << "\n";
}

std::vector<std::vector<std::string>> GameBoard::loadMatrix(const std::string& path, int offset, int expectedRows, int expectedColumns)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw std::runtime_error("Could not open file: " + path);

    // Skip lines up to the matrix start
    std::string line;
    for (int i = 0; i < offset; ++i) 
    {
        if (!std::getline(file, line))
            throw std::runtime_error("Unexpected end of file before matrix data");
    }

    // Read the matrix
    std::vector<std::vector<std::string>> matrix;
    for (int i = 0; i < expectedRows; ++i)
    {
        if (!std::getline(file, line))
            throw std::runtime_error("Unexpected end of file in matrix data");

        if (line.empty()) continue;

        std::vector<std::string> rowElements;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ','))
            rowElements.push_back(cell);

        if (static_cast<int>(rowElements.size()) != expectedColumns)
            throw std::runtime_error("Row size mismatch in matrix data");

        matrix.push_back(rowElements);
    }

    return matrix;
}

Vector2<int> GameBoard::snapScreenCoordinates(Vector2<int> coordinates)
{
    int x = coordinates.x - (coordinates.x % Tile::TILE_DIMENSIONS.x);
    int y = coordinates.y - (coordinates.y % Tile::TILE_DIMENSIONS.y);
    return { x, y };
}

Vector2<int> GameBoard::centerScreenCoordinates(Vector2<int> coordinates, const SDL_Rect& spriteDimensions)
{
    int x = coordinates.x - (coordinates.x % Tile::TILE_DIMENSIONS.x) + (Tile::TILE_DIMENSIONS.x / 2) - (spriteDimensions.w / 2);
    int y = coordinates.y - (coordinates.y % Tile::TILE_DIMENSIONS.y) + (Tile::TILE_DIMENSIONS.y / 2) - (spriteDimensions.h / 2);
    return { x, y };
}

Vector2<int> GameBoard::getGameBoardCoordinates(Vector2<int> coordinates) const
{
    return { coordinates.x / Tile::TILE_DIMENSIONS.x, coordinates.y / Tile::TILE_DIMENSIONS.y };
}

std::shared_ptr<Tile> GameBoard::getEnclosingTile(const Vector2<int>& position) const
{
    Vector2<int> tileCoordinates = snapScreenCoordinates(position);
    int xIndex = tileCoordinates.x / Tile::TILE_DIMENSIONS.x;
    int yIndex = tileCoordinates.y / Tile::TILE_DIMENSIONS.y;
    if (xIndex >= m_boardColumns || yIndex >= m_boardRows)
        return nullptr;
    return m_tiles[xIndex][yIndex];
}

std::shared_ptr<Tile> GameBoard::getTile(int x, int y) const
{
    if (x >= m_boardColumns || y >= m_boardRows || x < 0 || y < 0)
        return nullptr;
    return m_tiles[x][y];
}

std::vector<std::shared_ptr<Tile>> GameBoard::getTiles() const
{
    size_t total_size = 0;
    for (const auto& v : m_tiles)
        total_size += v.size();

    std::vector<std::shared_ptr<Tile>> tiles;
    tiles.reserve(total_size);

    for (const auto& v : m_tiles)
        tiles.insert(tiles.end(), v.begin(), v.end());

    return tiles;
}

void GameBoard::pushTile(const std::shared_ptr<Sprite>&entity, const Vector2<int>&playerPosition) const
{
    std::shared_ptr<Tile> playerTile = getEnclosingTile(playerPosition);
    std::shared_ptr<Tile> entityTile = getEnclosingTile(entity->getWindowCoordinates());

    if (!playerTile || !entityTile)
        return;

    int dX = playerTile->getWindowCoordinates().x - entityTile->getWindowCoordinates().x;
    int dY = playerTile->getWindowCoordinates().y - entityTile->getWindowCoordinates().y;

    if (std::abs(dX) > Tile::TILE_DIMENSIONS.x || std::abs(dY) > Tile::TILE_DIMENSIONS.y)
        return;

    int dirX = 0, dirY = 0;
    if (std::abs(dX) > std::abs(dY))
        dirX = (dX > 0) ? -1 : 1;
    else
        dirY = (dY > 0) ? -1 : 1;

    int currentX = entityTile->getWindowCoordinates().x / Tile::TILE_DIMENSIONS.x;
    int currentY = entityTile->getWindowCoordinates().y / Tile::TILE_DIMENSIONS.y;

    std::shared_ptr<Tile> targetTile = nullptr;
    while (true)
    {
        int nextX = currentX + dirX;
        int nextY = currentY + dirY;

        if (nextX < 0 || nextX >= m_boardColumns || nextY < 0 || nextY >= m_boardRows)
            break;

        std::shared_ptr<Tile> nextTile = m_tiles[nextX][nextY];
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
        Vector2 destination = centerScreenCoordinates(targetTile->getWindowCoordinates(), entity->getSdlRect());
        entity->walk({ destination });
    }
}

std::shared_ptr<Tile> GameBoard::getClosestAvailableTile(const Vector2<int>&tilePosition, const Vector2<int>&playerCoordinates) const
{
    static const std::vector<Vector2<int>> directions =
    {
        { 0, -1 },
        { -1, 0 },
        { 1, 0 },
        { 0, 1 }
    };

    Vector2<int> coordinates = snapScreenCoordinates(tilePosition);
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
            std::shared_ptr<Tile> adjacentTile = m_tiles[newX][newY];

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

std::vector<std::shared_ptr<Tile>> GameBoard::reversePath(const std::shared_ptr<AStarNode>&node)
{
    std::vector<std::shared_ptr<Tile>> path;
    std::shared_ptr<AStarNode> current = node;
    while (current)
    {
        path.push_back(current->getTile());
        current = current->getParent();
    }
    std::reverse(path.begin(), path.end());
    return path;
}

double GameBoard::heuristic(const Vector2<int>&a, const Vector2<int>&b)
{
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<std::shared_ptr<Tile>> GameBoard::getNeighborTiles(const std::shared_ptr<Tile>& tile) const
{
    static const std::vector directions = 
    {
        Vector2{ 0, -1 }, Vector2{ -1, 0 },
        Vector2{ 1, 0  }, Vector2{  0, 1 }
    };

    Vector2<int> position = getGameBoardCoordinates(tile->getWindowCoordinates());

    std::vector<std::shared_ptr<Tile>> neighbors;
    neighbors.reserve(directions.size());

    for (const auto& [dx, dy] : directions)
    {
        if (auto neighbor = getTile(position.x + dx, position.y + dy))
            neighbors.push_back(neighbor);
    }

    return neighbors;
}


std::vector<std::shared_ptr<Tile>> GameBoard::getPathToTile(const std::shared_ptr<Tile>& startTile, const std::shared_ptr<Tile>& goalTile) const
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
        heuristic(startTile->getWindowCoordinates(), goalTile->getWindowCoordinates())
    );

    openList.push(startNode);
    allNodes[startTile] = startNode;

    while (!openList.empty())
    {
        auto current = openList.top();
        openList.pop();
        closedList.insert(current->getTile());

        //std::cout << "Current Tile: " << current->getTile()->getWindowCoordinates() << "\n";
        if (current->getTile() == goalTile)
        {
            //std::cout << "Goal Reached!\n";
            return reversePath(current);
        }

        auto neighbors = getNeighborTiles(current->getTile());
        for (const auto& neighbor : neighbors)
        {
            if (closedList.count(neighbor))
            {
                //std::cout << "Skipping Closed Tile: " << neighbor->getWindowCoordinates() << "\n";
                continue;
            }
            if (neighbor->getResidingEntity() != nullptr)
            {
                //std::cout << "Skipping Blocked Tile: " << neighbor->getWindowCoordinates() << "\n";
                continue;
            }

            double tentativeG = current->getGValue() + 1.0;
            double hCost = heuristic(neighbor->getWindowCoordinates(), goalTile->getWindowCoordinates());
            auto neighborNode = std::make_shared<AStarNode>(neighbor, current, tentativeG, hCost);

            //std::cout << "Evaluating Neighbor: " << neighbor->getWindowCoordinates()
                //<< " G: " << tentativeG << " H: " << hCost << " F: " << (tentativeG + hCost) << "\n";

            if (allNodes.find(neighbor) == allNodes.end() || tentativeG < allNodes[neighbor]->getGValue())
            {
                //std::cout << "Pushing to Open List: " << neighbor->getWindowCoordinates() << "\n";
                openList.push(neighborNode);
                allNodes[neighbor] = neighborNode;
            }
        }
    }
    //std::cout << "No Path Found!\n";
    return {};
}

bool GameBoard::isSolved()
{
    for (const auto& column : m_tiles)
    {
        for (const auto& tile : column)
        {
            if (!tile->isGoalTile())
                continue;

            if (tile->getResidingEntity() == nullptr)
                return false;
        }
    }
    return true;
}