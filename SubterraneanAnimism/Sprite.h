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
class Sprite : public Entity
{
public:
    Sprite(const char* path, const Observer* observer = nullptr) : m_observer(observer)
    {
        m_image = loadSurface(path);

        m_position.w = m_image->w;
        m_position.h = m_image->h;
    }

    Sprite(const Sprite& other) 
        : m_observer(other.m_observer), 
          m_position(other.m_position), 
          m_coordinates(other.m_coordinates)
    {
        m_image = SDL_ConvertSurface(other.m_image, other.m_image->format, 0);
        if (!m_image)
            throw SDLImageLoadException(SDL_GetError());
    }

    ~Sprite() override
    {
        SDL_FreeSurface(m_image);
    }

    void update(double deltaTime) override
    {
        // Update logic goes here
    }

    void draw(SDL_Surface* windowSurface) override
    {
        SDL_BlitSurface(m_image, nullptr, windowSurface, &m_position);
    }

    void setScreenPosition(int x, int y) override
    {
        m_position.x = x;
        m_position.y = y;
    }

    void setCoordinates(const Vector2<double> coordinates) override
    {
        m_coordinates = coordinates;
    }

    [[nodiscard]] bool hasCollisionWith(const Entity& other)
    {
        if (other.isSpecializedSprite())
        {
            const Sprite<CollisionDetectionMethod>* spriteOther = static_cast<const Sprite<CollisionDetectionMethod>*>(&other);
            if (spriteOther)
                return CollisionDetectionMethod::hasCollisionWith(*this, *spriteOther);
        }
        return false;
    }

    [[nodiscard]] bool isCollisionSprite() const
    {
        return !std::is_same_v<NoCollision, CollisionDetectionMethod>;
    }

    [[nodiscard]] virtual bool isSpecializedSprite() const override { return true; }

    [[nodiscard]] SDL_Rect getScreenPosition() const override
    {
        return m_position;
    }

    [[nodiscard]] const Observer* getCollisionObserver()
    {
        return m_observer;
    }

    [[nodiscard]] Vector2<double> getCoordinates() override
    {
        return m_coordinates;
    }

private:
    const Observer* m_observer;
    SDL_Rect m_position{};
    Vector2<double> m_coordinates{};
    SDL_Surface* m_image{};

    static SDL_Surface* loadSurface(const char* path)
    {
        SDL_Surface* surface = SDL_LoadBMP(path);
        if (!surface)
            throw SDLImageLoadException(SDL_GetError());
        return surface;
    }
};

struct NoCollision
{
    template <typename T>
    static bool hasCollisionWith(const Sprite<NoCollision>& self, const T& other)
    {
        return false;
    }
};

struct RectangularCollision
{
    template <typename T, typename = std::enable_if_t<std::is_base_of<Entity, T>::value>>
    static bool hasCollisionWith(const Sprite<RectangularCollision>& self, const T& other)
    {
        return false;
    }

    template <>
    static bool hasCollisionWith(const Sprite<RectangularCollision>& self, const Sprite<RectangularCollision>& other)
    {
        if (other.isCollisionSprite())
        {
            const SDL_Rect selfRect = self.getScreenPosition();
            const SDL_Rect otherRect = other.getScreenPosition();
            return SDL_HasIntersection(&selfRect, &otherRect) == SDL_TRUE;
        }
        return false;
    }
};

struct PolygonCollision
{
    static bool hasCollisionWith(const Entity& self, const Entity& other)
    {
        return false; // Placeholder, replace with actual logic
    }
};
