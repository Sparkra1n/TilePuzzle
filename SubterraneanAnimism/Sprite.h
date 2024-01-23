#pragma once

#include <iostream>

#include "SDL.h"
#include "SDL_image.h"
#include "SDLExceptions.h"
#include "Entity.h"
#include "Vector2.h"
#include "Observer.h"
#include <vector>
#include <random>
#include "Sprite.h"

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
        m_rect.w = m_image->w;
        m_rect.h = m_image->h;
    }

    SpriteBase(const SpriteBase& other)
        : m_rect(other.m_rect), m_coordinates(other.m_coordinates)
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
        SDL_BlitSurface(m_image, nullptr, windowSurface, &m_rect);
    }

    void setCoordinates(const Vector2<double> coordinates) override
    {
        m_coordinates = coordinates;
        m_rect.x = static_cast<int>(coordinates.x);
        m_rect.y = static_cast<int>(coordinates.y);
    }

    [[nodiscard]] bool hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const override
    {
        if (other.isSpecializedSprite())
        {
            const auto* spriteOther = static_cast<const SpriteBase<CollisionDetectionMethod>*>(&other);

            // Call the correct collision detection method
            return CollisionDetectionMethod::hasCollisionWith(*this, *spriteOther, potentialPosition);
        }
        return false;
    }

    [[nodiscard]] uint8_t getPixelAlpha(const int x, const int y) const
    {
        if (x >= 0 && x < m_image->w && y >= 0 && y < m_image->h)
        {
            uint8_t alpha, red, green, blue;
            SDL_GetRGBA(static_cast<uint32_t*>(m_image->pixels)[y * m_image->w + x],
                m_image->format, &red, &green, &blue, &alpha);
            return alpha;
        }
        return 0;
    }

    static SDL_Color generateRandomColor()
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

    [[nodiscard]] virtual bool isCollisionSprite() const { return false; }
    [[nodiscard]] bool isSpecializedSprite() const override { return true; }
    [[nodiscard]] SDL_Rect getScreenPositionAndDimensions() const override { return m_rect; }
    [[nodiscard]] Vector2<double> getCoordinates() override { return m_coordinates; }
    [[nodiscard]] SDL_Surface* getSDLSurface() const override { return m_image; }

protected:
    static SDL_Surface* loadSurface(const char* path)
    {
        SDL_Surface* surface = SDL_LoadBMP(path);
        if (!surface)
            throw SDLImageLoadException(SDL_GetError());
        return surface;
    }

    SDL_Rect m_rect{};
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
class Sprite<RectangularCollision> : public SpriteBase<RectangularCollision>
{
public:
    Sprite(const char* path, const Observer* observer)
        : SpriteBase(path), m_observer(observer) {}

    [[nodiscard]] const Observer* getCollisionObserver() const { return m_observer; }

    [[nodiscard]] bool isCollisionSprite() const override { return true;  }

private:
    const Observer* m_observer;
};

template<>
class Sprite<PolygonCollision> : public SpriteBase<PolygonCollision>
{
public:
    Sprite(const char* path, const Observer* observer)
        : SpriteBase(path), m_observer(observer) {}

    [[nodiscard]] const Observer* getCollisionObserver() const { return m_observer; }

    [[nodiscard]] bool isCollisionSprite() const override { return true; }


    // TODO: need to figure out how to prevent extra pixels from being included in the last slice
	void slice(const int borderThickness)
    {
        if (borderThickness <= 0)
            return;

        m_slices.clear();
        auto a = m_rect;
        for (int y = 0; y < m_rect.h; ++y)
        {
            for (int x = 0; x < m_rect.w; ++x)
            {
                // Start slicing at place sprite border
                if (getPixelAlpha(x, y) > 0)
                {
                    SDL_Rect sliceRect;
                    sliceRect.x = x;
                    sliceRect.y = y;
                    sliceRect.h = borderThickness;

                    // Go across and find where it transitions back to transparent or reaches the image dimensions
					int width = y;
                    while (width <= m_rect.w && getPixelAlpha(x, width) != 0) ++width;
					sliceRect.w = width;
                    y += borderThickness - 1;
                    m_slices.push_back(sliceRect);
                    break;
                }
            }
        }
    }

    void printSlices()
    {
        for (const auto& [x, y, w, h] : m_slices)
            std::cout << "SDL_Rect { x: " << x << ", y: " << y << ", w: " << w << ", h: " << h << " }\n";
    }

private:
    const Observer* m_observer;
    std::vector<SDL_Rect> m_slices;
};

struct NoCollision
{
    template <typename T, typename = std::enable_if_t<std::is_base_of_v<Entity, T>>>
    static bool hasCollisionWith(
        const T& self,
        const T& other,
        Vector2<double> potentialPosition)
    {
        return false;
    }
};

struct RectangularCollision
{
    template <typename T, typename = std::enable_if_t<std::is_base_of_v<Entity, T>>>
    static bool hasCollisionWith(
        const SpriteBase<RectangularCollision>& self,
        const T& other,
        Vector2<double> potentialPosition)
    {
        return false;
    }

    static bool hasCollisionWith(
        const SpriteBase<RectangularCollision>& self,
        const SpriteBase<RectangularCollision>& other,
        const Vector2<double> potentialPosition)
    {
        if (other.isCollisionSprite())
        {
            SDL_Rect selfRect = self.getScreenPositionAndDimensions();
            selfRect.x = static_cast<int>(potentialPosition.x);
            selfRect.y = static_cast<int>(potentialPosition.y);
            const SDL_Rect otherRect = other.getScreenPositionAndDimensions();
            return SDL_HasIntersection(&selfRect, &otherRect) == SDL_TRUE;
        }
        return false;
    }
};

struct PolygonCollision
{
    template <typename T>
    static bool hasCollisionWith(
        const SpriteBase<T>& self, 
        const SpriteBase<T>& other, 
        Vector2<double> potentialPosition)
    {
        return false; // Placeholder, replace with actual logic
    }
};