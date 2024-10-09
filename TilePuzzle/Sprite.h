/**
 * @file Sprite.h
 * @brief SDL Sprite classes
 * @author Ani
 * @version 0.1
 * @date 2023-11-26
 */

#pragma once
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <random>
#include "SDLExceptions.h"
#include "Vector2.h"
#include "Observer.h"
#include "Textures.h"

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

//inline SpriteModifier operator+(const SpriteModifier first, const SpriteModifier second) noexcept
//{
//    return
//    {
//        first.r + second.r,
//        first.g + second.g,
//        first.b + second.b,
//        first.a + second.a
//    };
//}

//inline SpriteModifier operator-(const SpriteModifier first, const SpriteModifier second) noexcept
//{
//    return
//    {
//        first.r - second.r,
//        first.g - second.g,
//        first.b - second.b,
//        first.a - second.a
//    };
//}

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
    virtual void update(const double deltaTime) {}
    virtual void onFocus() {}
    virtual void onBlur() {}
    virtual void onClick() { std::cout << this << " clicked\n"; }
    virtual void applyModifier(SpriteModifier modifier) {}
    virtual void setRenderFlag() {}
    virtual void setPosition(Vector2<double> coordinates) {}
    virtual void setXCoordinate(double value) {}
    virtual void setYCoordinate(double value) {}
    virtual void walk(const std::vector<Vector2<int>>& path) {}
    virtual void clearRenderFlag() {}
    virtual void resetSurface() {}
    virtual void cacheTexture() {}
    [[nodiscard]] virtual bool isDummy() const { return true; }
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

class EntityStateTracker
{
public:
    EntityStateTracker() = default;
    ~EntityStateTracker() = default;

    void setFocused(const std::shared_ptr<Entity>& entity)
    {
        // Skip if repeat
        if (entity == m_current)
            return;

        if (m_previous)
        {
            m_previous->onBlur();
            m_previous = m_current;
            m_previous->onBlur();
        }
        m_previous = m_current;
        entity->onFocus();
        m_current = entity;
    }

    [[nodiscard]] std::shared_ptr<Entity> getFocused() const { return m_current; }
    [[nodiscard]] std::shared_ptr<Entity> getBlurred() const { return m_previous; }

protected:
    std::shared_ptr<Entity> m_current;
    std::shared_ptr<Entity> m_previous;
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
    void setPosition(Vector2<double> coordinates) override;
    void setXCoordinate(double value) override;
    void setYCoordinate(double value) override;
    void setRenderFlag() override { m_renderFlag = true; }
    void clearRenderFlag() override { m_renderFlag = false; }
    //void setCacheFlag() override { m_cacheFlag = true; }
    //void clearCacheFlag() override { m_cacheFlag = false; }
    void applyModifier(SpriteModifier modifier) override;
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
    [[nodiscard]] virtual bool isCollisionSprite() const;
    [[nodiscard]] bool isDummy() const override;
    [[nodiscard]] uint8_t getPixelAlpha(int x, int y) const;
    [[nodiscard]] SDL_Rect getSdlRect() const override;
    [[nodiscard]] Vector2<double> getPosition() const override { return m_coordinates; }
    [[nodiscard]] SDL_Surface* getSdlSurface() const;
    [[nodiscard]] SDL_Texture* getCachedTexture() const override;
    [[nodiscard]] std::vector<SDL_Rect> slice(int sliceThickness) const override;
    [[nodiscard]] const Observer* getCollisionObserver() const override { return m_observer; }
    [[nodiscard]] std::vector<std::shared_ptr<Sprite>> processSlices() const;

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
    //bool m_cacheFlag{};
    //SpriteModifier m_rgbaOffset{};
    SDL_Rect m_rect{};
    Vector2<double> m_coordinates{};
    const SDL_Surface* m_surfaceOriginal;
    SDL_Surface* m_surface{};
    SDL_Texture* m_texture{};
    const Observer* m_observer;
    std::vector<SDL_Rect> m_slices{};
    std::vector<SpriteModifier> m_modifierStack;  // Collection of active modifiers
    std::vector<SDL_Texture*> m_cachedTextures;  // Cache for textures at each stage
    SDL_Renderer* m_cacheRenderer;
};

class GameObject : public Sprite
{
public:
    GameObject(const char* path, 
        SDL_Renderer* cacheRenderer, const double speed = 0,
        const Observer* observer = nullptr)
	    : Sprite(path, cacheRenderer, observer), m_speed(speed) {}

    GameObject(const SDL_Rect rect, 
        const SDL_Color color, SDL_Renderer* cacheRenderer,
        const double speed = 1, 
        const Observer* observer = nullptr)
	    : Sprite(rect, color, cacheRenderer, observer), m_speed(speed) {}

    void update(double deltaTime) override;
	void walk(const std::vector<Vector2<int>>& path) override { m_checkpoints = path; }
    virtual void handleEvent(const SDL_Event& event) {}
    void setSpeed(const double speed) { m_speed = speed; }
    [[nodiscard]] double getSpeed() const { return m_speed; } // A speed of 0 would imply that the sprite is immovable
    [[nodiscard]] bool isDummy() const override { return false; }
protected:
    std::vector<Vector2<int>> m_checkpoints;
    double m_speed;
};


