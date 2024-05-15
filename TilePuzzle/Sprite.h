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
#include <tuple>
#include <vector>
#include <random>
#include "SDLExceptions.h"
#include "Vector2.h"
#include "Observer.h"

/**
 * @brief Color struct that allows values < 0 and > 255 in order to perform offset operations on SDL_Colors
 */
struct Color
{
    Color (const int r, const int g, const int b, const int a)
		: r(r), g(g), b(b), a(a) {}

    Color() = default;

    Color(const Color&) = default;

    explicit Color(const SDL_Color& other)
        : r(other.r), g(other.g), b(other.b), a(other.a) {}

    ~Color() = default;

    static constexpr uint8_t colorClamp(const int value)
    {
        return static_cast<uint8_t>(value > 255 ? 255 : (value < 0 ? 0 : value));
    }

    /**
     * @brief Returns a properly clamped SDL_Color.
     */
    SDL_Color toSdlColor() const
    {
        return
        {
            colorClamp(r),
            colorClamp(g),
            colorClamp(b),
            colorClamp(a)
        };
    }

    int r{};
    int g{};
    int b{};
    int a{};
};

inline std::ostream& operator<<(std::ostream& os, const Color& color)
{
    os << "{" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << "}";
    return os;
}

inline Color operator+(const Color first, const Color second) noexcept
{
    return
    {
        first.r + second.r,
        first.g + second.g,
        first.b + second.b,
        first.a + second.a
    };
}

inline Color operator-(const Color first, const Color second) noexcept
{
    return
    {
        first.r - second.r,
        first.g - second.g,
        first.b - second.b,
        first.a - second.a
    };
}

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

// Type for specifying collision detection system
struct NoCollision;
struct RectangularCollision;
struct PolygonCollision;

template <typename T>
struct IsAllowedCollisionMethod : std::disjunction<
    std::is_same<T, NoCollision>,
    std::is_same<T, RectangularCollision>,
    std::is_same<T, PolygonCollision>
> {};

template <typename T, std::enable_if_t<IsAllowedCollisionMethod<T>::value, bool> = true>
class Sprite;

// Brief Base class for all game objects
class Entity
{
public:
    Entity() = default;
    virtual ~Entity() = default;

    /**
     * @brief Performs any time-related calculations
     * @param deltaTime 
     */
    virtual void update(double deltaTime);

    /**
     * @brief Whether not the object is a Sprite
     * @returns false for any base class or class having Sprite as a member
     */
    [[nodiscard]] virtual bool isDummy() const;

    /**
     * @brief Returns the Sprite's SDL_Rect
     * @return SDL_Rect
     */
    [[nodiscard]] virtual SDL_Rect getSdlRect() const = 0;

    /**
     * @brief Caches the sprite's SDL_Surface to allow for faster rendering
     * @param renderer
     */
    virtual void cacheTexture(SDL_Renderer* renderer) = 0;

    /**
     * @brief Returns the Sprite's cached SDL_Texture
     * @return SDL_Texture*
     */
    [[nodiscard]] virtual SDL_Texture* getCachedTexture() const = 0;

    /**
     * @brief 
     * @param other other sprite
     * @param potentialPosition potential position vector of this sprite
     * @return bool
     */
    [[nodiscard]]
    virtual bool hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const { return false; }

    //virtual SpriteFlag* getSpriteFlag() = 0;

    virtual void setRgbaOffset_(SDL_Color offset) = 0;

    /**
     * @brief Show a hidden sprite
     */
    virtual void setRenderFlag() = 0;

    /**
     * @brief Skip the sprite when rendering
     */
    virtual void clearRenderFlag() = 0;

    /**
     * @brief Indicate the sprite texture needs to be re-cached
     */
    virtual void setCacheFlag() = 0;

    /**
     * @brief Clear the indication that the sprite texture needs to be re-cached
     */
    virtual void clearCacheFlag() = 0;

    /**
     * @brief A flag for whether the sprite is visible or hidden
     * @return
     */
    [[nodiscard]] virtual bool getRenderFlag() const { return true; }

    /**
     * @brief A flag for whether the sprite texture needs sto be cached again
     * @return
     */
    [[nodiscard]] virtual bool getCacheFlag() const { return false; }

    /**
     * @brief Resets SDL_Surface to its initial state by replacing it with a copy of the original surface.
     * 
     */
    virtual void resetSurface() = 0;

    //TODO: fix derived class not being able to access
//protected:
    [[nodiscard]]
    virtual bool hasCollisionWithImpl(const Sprite<NoCollision>& other, Vector2<double> potentialPosition) const { return false; }

    [[nodiscard]]
    virtual bool hasCollisionWithImpl(const Sprite<RectangularCollision>& other, Vector2<double> potentialPosition) const { return false; }

    [[nodiscard]]
    virtual bool hasCollisionWithImpl(const Sprite<PolygonCollision>& other, Vector2<double> potentialPosition) const { return false; }
};

class SpriteBase : public Entity
{
public:
    SpriteBase(const char* path, const Observer* observer = nullptr);
    SpriteBase(SDL_Rect rect, SDL_Color color);
    SpriteBase(const SpriteBase& other);
    ~SpriteBase() override;

    void setCoordinates(Vector2<double> coordinates);

    void setXCoordinate(double value);

    void setYCoordinate(double value);

    /**
     * @brief Caches the sprite's SDL_Surface to allow for faster rendering
     * @param renderer 
     */
    void cacheTexture(SDL_Renderer* renderer) override;

    /**
     * @brief Show the sprite.
     */
    void setRenderFlag() override { m_renderFlag = true; }

    /**
     * @brief Hide the sprite.
     */
    void clearRenderFlag() override { m_renderFlag = false; }

    /**
     * @brief Indicate the sprite texture needs to be re-cached
     */
    void setCacheFlag() override { m_cacheFlag = true; }

    /**
     * @brief Clear the indication that the sprite texture needs to be re-cached
     */
    void clearCacheFlag() override { m_cacheFlag = false; }

    /**
     * @brief Returns the flag on whether the sprite needs to be cached.
     * @return bool
     */
    [[nodiscard]] bool getCacheFlag() const override { return m_cacheFlag; }

    void setRgbaOffset_(SDL_Color offset) override;

    [[nodiscard]] static SDL_Color generateRandomColor();

    [[nodiscard]] virtual bool isCollisionSprite() const;

    [[nodiscard]] bool isDummy() const override;

    [[nodiscard]] uint8_t getPixelAlpha(int x, int y) const;

    [[nodiscard]] SDL_Rect getSdlRect() const override;

    [[nodiscard]] Vector2<double> getCoordinates() const;

    /**
     * @brief Returns the Sprite's cached SDL_Surface
     * @return SDL_Surface*
     */
    [[nodiscard]] SDL_Surface* getSdlSurface() const;

    /**
     * @brief Returns the Sprite's cached SDL_Texture
     * @return SDL_Texture*
     */
    [[nodiscard]] SDL_Texture* getCachedTexture() const override;

    /**
     * @brief A flag for whether the sprite is visible or hidden
     * @return
     */
    [[nodiscard]] bool getRenderFlag() const override { return m_renderFlag; }

    void resetSurface() override;

protected:
    static SDL_Surface* loadSurface(const char* path);
    bool m_renderFlag{};     // Whether the sprite is visible
    bool m_cacheFlag{};      // Whether the SDL_Texture need to be re-cached

    Color m_rgbaOffset{}; // RGBA offset
    SDL_Rect m_rect{};
    Vector2<double> m_coordinates{};

    //TODO: making this const will cause problems with functions like SDL_FreeSurface
	const SDL_Surface* m_surfaceOriginal; // A copy to allow m_surface to be reset when needed.
    SDL_Surface* m_surface{}; // This is where the sprite data is manipulated.
    SDL_Texture* m_texture{}; // This is where the cached sprite is stored.
};

template <typename T, std::enable_if_t<IsAllowedCollisionMethod<T>::value, bool>>
class Sprite : public SpriteBase {};

template <>
class Sprite<NoCollision> : public SpriteBase
{
public:
    Sprite(const char* path);
    Sprite(SDL_Rect rect, SDL_Color color);
};

template <>
class Sprite<RectangularCollision> : public SpriteBase
{
public:
    Sprite(const char* path, const Observer* observer);
    Sprite(SDL_Rect rect, SDL_Color color, const Observer* observer);

    [[nodiscard]] const Observer* getCollisionObserver() const;

    [[nodiscard]] bool isCollisionSprite() const override;

    /**
     * @brief Whether a RectangularCollision Sprite will collide with anything
     * @param other other sprite
     * @param potentialPosition potential position vector of this sprite
     * @return bool
     */
    [[nodiscard]]
    bool hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const override;

protected:
    /**
     * @brief Checks collisions between RectangularCollision and RectangularCollision
     * @param other other sprite
     * @param potentialPosition potential position vector of the other sprite
     * @return bool
     */
    [[nodiscard]]
    bool hasCollisionWithImpl(const Sprite<RectangularCollision>& other, Vector2<double> potentialPosition) const override;

    /**
     * @brief Checks collisions between RectangularCollision and PolygonCollision
     * @param other other sprite
     * @param potentialPosition potential position vector of the other sprite
     * @return bool
     */
    [[nodiscard]]
    bool hasCollisionWithImpl(const Sprite<PolygonCollision>& other, Vector2<double> potentialPosition) const override;

    const Observer* m_observer;
};

template<>
class Sprite<PolygonCollision> : public SpriteBase
{
public:
    Sprite(const char* path, const Observer* observer);

    [[nodiscard]] std::vector<SDL_Rect> slice(int sliceThickness) const;

    [[nodiscard]] const Observer* getCollisionObserver() const;

    [[nodiscard]] bool isCollisionSprite() const override;

    // Test function
    [[nodiscard]]
    std::vector<std::shared_ptr<Sprite<NoCollision>>> processSlices();

    /**
     * @brief
     * @param other other sprite
     * @param potentialPosition potential position vector of this sprite
     * @return bool
     */
    [[nodiscard]]
    bool hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const override;

protected:
    /**
     * @brief Checks collisions between PolygonCollision and PolygonCollision
     * @param other other sprite
     * @param potentialPosition potential position vector of the other sprite
     * @return bool
     */
    [[nodiscard]]
    bool hasCollisionWithImpl(const Sprite<PolygonCollision>& other, Vector2<double> potentialPosition) const override;

    /**
     * @brief Checks collisions between PolygonCollision and RectangularCollision
     * @param other other sprite
     * @param potentialPosition potential position vector of the other sprite
     * @return bool
     */
    [[nodiscard]]
    bool hasCollisionWithImpl(const Sprite<RectangularCollision>& other, Vector2<double> potentialPosition) const override;

    const Observer* m_observer;
    std::vector<SDL_Rect> m_slices;
};