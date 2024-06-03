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
    virtual void setRgbaOffset(SDL_Color offset) {}
    virtual void setRenderFlag() {}
    virtual void setCoordinates(Vector2<double> coordinates) {}
    virtual void setXCoordinate(double value) {}
    virtual void setYCoordinate(double value) {}
    virtual void clearRenderFlag() {}
    virtual void setCacheFlag() {}
    virtual void clearCacheFlag() {}
    virtual void resetSurface() {}
    virtual void cacheTexture(SDL_Renderer* renderer) {}
    [[nodiscard]] virtual bool isDummy() const { return true; }
    [[nodiscard]] virtual Vector2<double> getCoordinates() const { return {}; }
    [[nodiscard]] virtual SDL_Rect getSdlRect() const { return SDL_Rect{}; }
    [[nodiscard]] virtual SDL_Texture* getCachedTexture() const { return nullptr; }
    [[nodiscard]] virtual std::vector<SDL_Rect> slice(int sliceThickness) const { return {}; }
    [[nodiscard]] virtual bool getRenderFlag() const { return true; }
    [[nodiscard]] virtual bool getCacheFlag() const { return false; }
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

//TODO: What if the sprite checks if the mouse is hovering over it when update is called on it?

class Sprite : public Entity
{
public:
    Sprite(const char* path, const Observer* observer = nullptr);
    Sprite(SDL_Rect rect, SDL_Color color, const Observer* observer = nullptr);
    Sprite(const Sprite& other);
    ~Sprite() override;

	void onFocus() override;
	void onBlur() override;
    void setCoordinates(Vector2<double> coordinates) override;
    void setXCoordinate(double value) override;
    void setYCoordinate(double value) override;
    void cacheTexture(SDL_Renderer* renderer) override;
    void setRenderFlag() override { m_renderFlag = true; }
    void clearRenderFlag() override { m_renderFlag = false; }
    void setCacheFlag() override { m_cacheFlag = true; }
    void clearCacheFlag() override { m_cacheFlag = false; }
    void setRgbaOffset(SDL_Color offset) override;
    void resetSurface() override;
    void printSlices();

    [[nodiscard]] bool getCacheFlag() const override { return m_cacheFlag; }
    [[nodiscard]] bool getRenderFlag() const override { return m_renderFlag; }
    [[nodiscard]] static SDL_Color generateRandomColor();
    [[nodiscard]] virtual bool isCollisionSprite() const;
    [[nodiscard]] bool isDummy() const override;
    [[nodiscard]] uint8_t getPixelAlpha(int x, int y) const;
    [[nodiscard]] SDL_Rect getSdlRect() const override;
    [[nodiscard]] Vector2<double> getCoordinates() const override { return m_coordinates; }
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
    bool m_cacheFlag{};
    Color m_rgbaOffset{};
    SDL_Rect m_rect{};
    Vector2<double> m_coordinates{};
    const SDL_Surface* m_surfaceOriginal;
    SDL_Surface* m_surface{};
    SDL_Texture* m_texture{};
    const Observer* m_observer;
    std::vector<SDL_Rect> m_slices{};
};

class ExtendedSprite : public Sprite
{
public:
    ExtendedSprite(const char* path, 
        const double speed = 1, 
        const Observer* observer = nullptr)
	    : Sprite(path, observer), m_speed(speed) {}

    ExtendedSprite(const SDL_Rect rect, 
        const SDL_Color color, 
        const double speed = 1, 
        const Observer* observer = nullptr)
	    : Sprite(rect, color, observer), m_speed(speed) {}

	//void onClick() override {}
	//void onFocus() override {}
	//void onBlur() override {}
    virtual void goTo(const Vector2<int> coordinates) { m_targetPosition = coordinates; }
    virtual void handleEvent(const SDL_Event& event) {}
    void setSpeed(const double speed) { m_speed = speed; }
    [[nodiscard]] double getSpeed() const { return m_speed; }
    [[nodiscard]] bool isDummy() const override { return false; }
protected:
    Vector2<int> m_targetPosition{};
    double m_speed;
};