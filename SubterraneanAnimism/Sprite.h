#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include "SDLExceptions.h"
#include "Entity.h"
#include "Vector2.h"
#include "Observer.h"
#include <vector>
#include <iostream>

struct NoCollision;
struct RectangularCollision;
struct PolygonCollision;

template <typename T>
struct IsAllowedCollisionMethod : std::disjunction<
    std::is_same<T, NoCollision>,
    std::is_same<T, RectangularCollision>,
    std::is_same<T, PolygonCollision>
> {};

template <typename CollisionDetectionMethod,
    std::enable_if_t<IsAllowedCollisionMethod<CollisionDetectionMethod>::value, bool> = true
>
class SpriteBase : public Entity 
{
public:
    SpriteBase(const char* path, const Observer* observer = nullptr)
    {
        m_image = loadSurface(path);
        m_position.w = m_image->w;
        m_position.h = m_image->h;
    }

    SpriteBase(const SpriteBase& other)
        : m_position(other.m_position), m_coordinates(other.m_coordinates)
    {
        m_image = SDL_ConvertSurface(other.m_image, other.m_image->format, 0);
        if (!m_image)
            throw SDLImageLoadException(SDL_GetError());
    }

    ~SpriteBase() override { SDL_FreeSurface(m_image); }

    void update(double deltaTime) override
    {
        // Update logic goes here
    }

    void draw(SDL_Surface* windowSurface) override
    {
        SDL_BlitSurface(m_image, nullptr, windowSurface, &m_position);
    }

    void setDimensions(const int x, const int y) override
    {
        m_position.x = x;
        m_position.y = y;
    }

    void setCoordinates(const Vector2<double> coordinates) override
    {
        m_coordinates = coordinates;
        m_position.x = static_cast<int>(coordinates.x);
        m_position.y = static_cast<int>(coordinates.y);
    }

    [[nodiscard]] bool hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const override
    {
        if (other.isSpecializedSprite())
        {
            const auto* spriteOther = static_cast<const SpriteBase<CollisionDetectionMethod>*>(&other);
            return CollisionDetectionMethod::hasCollisionWith(*this, *spriteOther, potentialPosition);
        }
        return false;
    }

    [[nodiscard]] virtual bool isCollisionSprite() const { return false; }
    [[nodiscard]] bool isSpecializedSprite() const override { return true; }
    [[nodiscard]] SDL_Rect getScreenPosition() const override { return m_position; }
    [[nodiscard]] Vector2<double> getCoordinates() override { return m_coordinates; }

private:
    static SDL_Surface* loadSurface(const char* path)
    {
        SDL_Surface* surface = SDL_LoadBMP(path);
        if (!surface)
            throw SDLImageLoadException(SDL_GetError());
        return surface;
    }

    SDL_Rect m_position{};
    Vector2<double> m_coordinates{};
    SDL_Surface* m_image{};
};

template <typename CollisionDetectionMethod,
std::enable_if_t<IsAllowedCollisionMethod<CollisionDetectionMethod>::value, bool> = true
>
class Sprite : public SpriteBase<CollisionDetectionMethod> {};

template <>
class Sprite<NoCollision> : public SpriteBase<NoCollision>
{
public:
    Sprite(const char* path) : SpriteBase(path) {}
};

template <>
class Sprite<PolygonCollision> : SpriteBase<PolygonCollision>
{
    [[nodiscard]] bool isCollisionSprite() const
    {
        return true;
    }
};

template <>
class Sprite<RectangularCollision> : public SpriteBase<RectangularCollision>
{
public:
    Sprite(const char* path, const Observer* observer)
        : SpriteBase(path), m_observer(observer) {}

    [[nodiscard]] const Observer* getCollisionObserver() const { return m_observer; }
private:
    const Observer* m_observer;
};

struct NoCollision
{
    template <typename T>
    static bool hasCollisionWith(const Sprite<NoCollision>& self, const T& other, Vector2<double> potentialPosition)
    {
        return false;
    }
};

struct RectangularCollision
{
    template <typename T, typename = std::enable_if_t<std::is_base_of_v<Entity, T>>>
    static bool hasCollisionWith(const SpriteBase<RectangularCollision>& self, const T& other, Vector2<double> potentialPosition)
    {
        return false;
    }

    template <>
    static bool hasCollisionWith(const SpriteBase<RectangularCollision>& self, const SpriteBase<RectangularCollision>& other, Vector2<double> potentialPosition)
    {
        if (other.isCollisionSprite())
        {
            SDL_Rect selfRect = self.getScreenPosition();
            selfRect.x = static_cast<int>(potentialPosition.x);
            selfRect.y = static_cast<int>(potentialPosition.y);
            const SDL_Rect otherRect = other.getScreenPosition();
            return SDL_HasIntersection(&selfRect, &otherRect) == SDL_TRUE;
        }
        return false;
    }
};

struct PolygonCollision
{
    template <typename T>
    static bool hasCollisionWith(const SpriteBase<T>& self, const SpriteBase<T>& other, Vector2<double> potentialPosition) 
    {
        return false; // Placeholder, replace with actual logic
    }
};