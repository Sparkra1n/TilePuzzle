#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <random>
#include "SDLExceptions.h"
#include "Vector2.h"
#include "Observer.h"
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

#include "GameState.h"


/**
 * @brief Struct to perform offset operations on SDL_Colors
 */
struct SpriteModifier
{
    SpriteModifier(const std::string& description, const int r, const int g, const int b, const int a)
        : name(description), r(r), g(g), b(b), a(a) {}

    SpriteModifier() = default;

    SpriteModifier(const std::string& description, const SDL_Color& other)
        : name(description), r(other.r), g(other.g), b(other.b), a(other.a) {}

    static constexpr uint8_t colorClamp(const int value)
    {
        return static_cast<uint8_t>(value > 255 ? 255 : (value < 0 ? 0 : value));
    }

    // Returns a properly clamped SDL_Color.
    SDL_Color toSdlColor() const;
    std::string name;
    int r, g, b, a;
};

inline std::ostream& operator<<(std::ostream& os, const SpriteModifier& color)
{
    os << "{" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << "}";
    return os;
}

//TODO: Put inside somewhere
enum class CollisionDetectionMethod
{
    NoCollision = 0,
    RectangularCollision,
    PolygonCollision
};

class Entity
{
public:
    Entity() = default;
    virtual ~Entity() = default;
    virtual void update(const GameState& state) {}
    virtual void onFocus() {}
    virtual void onBlur() {}
    virtual void onClick() {}
    virtual void applyModifier(const SpriteModifier& modifier) {}
    virtual void setRenderFlag() {}
    virtual void setCoordinates(Vector2<double> coordinates) {}
    virtual void setXCoordinate(double value) {}
    virtual void setYCoordinate(double value) {}
    virtual void walk(const std::vector<Vector2<int>>& path) {}
    virtual void clearRenderFlag() {}
    virtual void resetSurface() {}
    virtual void cacheTexture() {}
    [[nodiscard]] virtual Vector2<double> getPosition() const { return {}; }
    [[nodiscard]] virtual SDL_Rect getSdlRect() const { return SDL_Rect{}; }
    [[nodiscard]] virtual SDL_Texture* getCachedTexture() const { return nullptr; }
    [[nodiscard]] virtual std::vector<SDL_Rect> slice(int sliceThickness) const { return {}; }
    [[nodiscard]] virtual bool getRenderFlag() const { return true; }
    [[nodiscard]] virtual const Observer* getCollisionObserver() const { return nullptr; }

    /**
     * @param other other entity
     * @param potentialPosition this entity's potential position
     * @param collisionDetectionMethod 
     * @return 
     */
    [[nodiscard]] virtual bool hasCollisionWith(const Entity& other, 
                                                Vector2<double> potentialPosition, 
                                                CollisionDetectionMethod collisionDetectionMethod) const { return false; }
};

class Sprite : public Entity
{
public:
    Sprite(const char* path, SDL_Renderer* cacheRenderer, const Observer* observer = nullptr);
    Sprite(SDL_Rect rect, SDL_Color color, SDL_Renderer* cacheRenderer, const Observer* observer = nullptr);
    Sprite(const Sprite& other);
    ~Sprite() override;
    void onClick() override;
    void onFocus() override;
    void onBlur() override;
    void setCoordinates(Vector2<double> coordinates) override;
    void setXCoordinate(double value) override;
    void setYCoordinate(double value) override;
    void setRenderFlag() override { m_renderFlag = true; }
    void clearRenderFlag() override { m_renderFlag = false; }
    void applyModifier(const SpriteModifier& modifier) override;
    void resetSurface() override;
    void printSlices();

    // Modifiers
    void pushModifier(const SpriteModifier& modifier);
    void removeModifier(const std::string& name);
    SpriteModifier popModifier();
    void applyModifiers();

    // Caching system
    void cacheTexture() override;

    [[nodiscard]] bool getRenderFlag() const override { return m_renderFlag; }
    [[nodiscard]] static SDL_Color generateRandomColor();
    [[nodiscard]] uint8_t getPixelAlpha(int x, int y) const;
    [[nodiscard]] SDL_Rect getSdlRect() const override;
    [[nodiscard]] Vector2<double> getPosition() const override { return m_coordinates; }
    [[nodiscard]] SDL_Surface* getSdlSurface() const;
    [[nodiscard]] SDL_Texture* getCachedTexture() const override;
    [[nodiscard]] std::vector<SDL_Rect> slice(int sliceThickness) const override;
    [[nodiscard]] const Observer* getCollisionObserver() const override { return m_observer; }
    //[[nodiscard]] std::vector<std::shared_ptr<Sprite>> processSlices() const;

    /**
     * @param other other entity
     * @param potentialPosition this entity's potential position
     * @param collisionDetectionMethod
     * @return
     */
    [[nodiscard]] bool hasCollisionWith(const Entity& other, 
        Vector2<double> potentialPosition, 
        CollisionDetectionMethod collisionDetectionMethod) const override;

protected:
    static SDL_Surface* loadSurface(const char* path);
    bool m_renderFlag{}; // Controls the visibility of the sprite
    SDL_Rect m_rect{};
    Vector2<double> m_coordinates{};
    const SDL_Surface* m_surfaceOriginal;
    SDL_Surface* m_surface{};
    SDL_Texture* m_texture{};
    const Observer* m_observer;
    std::vector<SDL_Rect> m_slices{};
    std::vector<SpriteModifier> m_modifierStack;  // Collection of active modifiers
    std::vector<SDL_Texture*> m_cachedTextures;   // Cache for textures at each stage
    SDL_Renderer* m_cacheRenderer;
};

class Tile : public Sprite
{
public:
    static constexpr Vector2<int> TILE_DIMENSIONS = { 86, 64 };

    Tile(const std::string& texturePath, SDL_Renderer* cacheRenderer,
        const std::shared_ptr<Sprite>& residingEntity = nullptr,
        bool isGoalTile = false
    );

    void setAsGoalTile() { m_isGoalTile = true; }
    void setResidingEntity(const std::shared_ptr<Sprite>& residingEntity);
    [[nodiscard]] bool isGoalTile() const { return m_isGoalTile; }
    [[nodiscard]] std::shared_ptr<Sprite> getResidingEntity() const;

private:
    std::shared_ptr<Sprite> m_residingEntity;
    bool m_isGoalTile;
};

class GameObject : public Sprite
{
public:
    enum class PhysicsType
    {
        Movable,
        Immovable
    };

    GameObject(const char* path, 
        SDL_Renderer* cacheRenderer, 
        const PhysicsType type,
        const double speed = 0, 
        const Observer* observer = nullptr)
        : Sprite(path, cacheRenderer, observer),
          m_speed(type == PhysicsType::Immovable ? 0 : speed),
          m_physicsType(type) {}


    GameObject(const SDL_Rect rect, 
        const SDL_Color color, 
        SDL_Renderer* cacheRenderer, 
        PhysicsType type,
        const double speed = 1, 
        const Observer* observer = nullptr)
        : Sprite(rect, color, cacheRenderer, observer),
          m_speed(type == PhysicsType::Immovable ? 0 : speed),
          m_physicsType(type) {}

    void update(const GameState& state) override;
    void walk(const std::vector<Vector2<int>>& path) override { m_checkpoints = path; }
    virtual void handleEvent(const SDL_Event& event) {}
    void setSpeed(const double speed) { m_speed = speed; }
    [[nodiscard]] double getSpeed() const { return m_speed; }
protected:
    std::vector<Vector2<int>> m_checkpoints;
    double m_speed;
    PhysicsType m_physicsType;
};

class GameBoard : public Observer
{
public:
    GameBoard(const std::string& path, SDL_Renderer* cacheRenderer);
    void update(const GameState& state);
    void pushTile(const std::shared_ptr<Sprite>& entity, const Vector2<int>& playerPosition) const;
    void readDimensions(const std::string& path);
    [[nodiscard]] static Vector2<int> positionToTileLeftCorner(const Vector2<int>& position);
    [[nodiscard]] static Vector2<int> positionToTileCenter(const Vector2<int>& position, const SDL_Rect& spriteDimensions);
    [[nodiscard]] std::shared_ptr<Tile> getEnclosingTile(const Vector2<int>& position) const;
    [[nodiscard]] std::shared_ptr<Tile> getTile(int x, int y) const;
    [[nodiscard]] std::vector<std::shared_ptr<Tile>> getTiles() const;
    [[nodiscard]] Vector2<int> getTileCoordinates(const std::shared_ptr<Tile>& tile) const;
    [[nodiscard]] std::shared_ptr<Tile> getClosestAvailableTile(const Vector2<int>& tilePosition, const Vector2<int>& playerCoordinates) const;
    [[nodiscard]] std::vector<std::shared_ptr<Tile>> getPathToTile(const std::shared_ptr<Tile>& startTile, const std::shared_ptr<Tile>& goalTile) const;
    [[nodiscard]] bool isSolved();

    static constexpr int MAX_ROWS = 7;
    static constexpr int MAX_COLUMNS = 7;

private:
    SDL_Renderer* m_cacheRenderer;
	int m_boardRows{};
    int m_boardColumns{};
    Vector2<int> m_boardBoundaries{};

    std::shared_ptr<Entity> m_hoveredEntity;
    std::vector<std::vector<std::shared_ptr<Tile>>> m_tiles;

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

    static std::vector<std::vector<std::string>> loadMatrix(const std::string& path, int offset, int expectedRows, int expectedColumns);
    static std::vector<std::shared_ptr<Tile>> reversePath(const std::shared_ptr<AStarNode>& node);
    static double heuristic(const Vector2<int>& a, const Vector2<int>& b);
    [[nodiscard]] std::vector<std::shared_ptr<Tile>> getNeighborTiles(const std::shared_ptr<Tile>& tile) const;
};