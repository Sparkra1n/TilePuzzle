/**
 * @file Sprite.h
 * @author Ani
 * @brief SDL Sprite classes
 * @version 0.1
 * @date 2023-11-26
 */

#pragma once

#include "SDLExceptions.h"
#include "Entity.h"
#include "Vector2.h"
#include "Observer.h"
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include <random>

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


template <typename T,
    std::enable_if_t<IsAllowedCollisionMethod<T>::value, bool> = true
>
class SpriteBase;

template <typename CollisionDetectionMethod,
    std::enable_if_t<IsAllowedCollisionMethod<CollisionDetectionMethod>::value, bool> = true
>
class Sprite : public SpriteBase<CollisionDetectionMethod> {};


// Base class containing shared sprite features
template <typename T,
    std::enable_if_t<IsAllowedCollisionMethod<T>::value, bool>
>
class SpriteBase : public Entity
{
public:
    SpriteBase(const char* path, const Observer* observer = nullptr)
        : m_texture(nullptr)
    {
        m_surface = loadSurface(path);
        m_rect.w = m_surface->w;
        m_rect.h = m_surface->h;
    }

    SpriteBase(const SDL_Rect rect, const SDL_Color color)
        : m_rect(rect), m_coordinates(rect.x, rect.y), m_texture(nullptr)
    {
        SDL_Surface* surface = SDL_CreateRGBSurface(0, rect.w, rect.h, 32, 0, 0, 0, 0);
        if (!surface)
            throw SDLImageLoadException(SDL_GetError());

        SDL_FillRect(surface, nullptr, SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));
        m_surface = surface;
    }

    SpriteBase(const SpriteBase& other)
        : m_rect(other.m_rect), m_coordinates(other.m_coordinates), m_texture(nullptr)
    {
        m_surface = SDL_ConvertSurface(other.m_surface, other.m_surface->format, 0);
        if (!m_surface)
            throw SDLImageLoadException(SDL_GetError());
    }

    ~SpriteBase() override { SDL_FreeSurface(m_surface); }

    void update(double deltaTime) override
    {
        // Update logic goes here
    }

    // Old render system (blitting the sprite)
    void draw(SDL_Surface* windowSurface) override
    {
        SDL_BlitSurface(m_surface, nullptr, windowSurface, &m_rect);
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
            const auto* first = static_cast<const Sprite<T>*>(&other);
            const auto* second = static_cast<const Sprite<T>*>(&other);

            // Call the correct collision detection method
            return T::hasCollisionWith(*first, *second, potentialPosition);
        }
        return false;
    }

    void cacheTexture(SDL_Renderer* renderer) override
    {
        if (m_texture != nullptr)
            SDL_DestroyTexture(m_texture);
        m_texture = SDL_CreateTextureFromSurface(renderer, m_surface);
    }

    [[nodiscard]] uint8_t getPixelAlpha(const int x, const int y) const
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

    [[nodiscard]] virtual bool isCollisionSprite() const
    {
        return false;
    }

    [[nodiscard]] bool isSpecializedSprite() const override
    {
        return true;
    }

    [[nodiscard]] SDL_Rect getScreenPositionAndDimensions() const override
    {
        return m_rect;
    }

    [[nodiscard]] Vector2<double> getCoordinates() override
    {
        return m_coordinates;
    }

    [[nodiscard]] SDL_Surface* getSDLSurface() const override
    {
        return m_surface;
    }

    [[nodiscard]] SDL_Texture* getCachedTexture() const override
    {
        return m_texture;
    }

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
    SDL_Surface* m_surface{};
    SDL_Texture* m_texture;
};

template <>
class Sprite<NoCollision> : public SpriteBase<NoCollision>
{
public:
    Sprite(const char* path) : SpriteBase(path) {}

    // Create sprite from SDL_Rect and color
    Sprite(const SDL_Rect rect, const SDL_Color color) : SpriteBase(rect, color) {}
};

template <>
class Sprite<RectangularCollision> : public SpriteBase<RectangularCollision>
{
public:
    Sprite(const char* path, const Observer* observer)
        : SpriteBase(path), m_observer(observer) {}

    // Create sprite from SDL_Rect and color
    Sprite(const SDL_Rect rect, const SDL_Color color, const Observer* observer)
        : SpriteBase(rect, color), m_observer(observer) {}

    [[nodiscard]] const Observer* getCollisionObserver() const
    {
        return m_observer;
    }

    [[nodiscard]] bool isCollisionSprite() const override
    {
        return true;
    }

private:
    const Observer* m_observer;
};

template<>
class Sprite<PolygonCollision> : public SpriteBase<PolygonCollision>
{
public:
    Sprite(const char* path, const Observer* observer)
        : SpriteBase(path), m_observer(observer) {}

    [[nodiscard]] const Observer* getCollisionObserver() const
    {
        return m_observer;
    }

    [[nodiscard]] bool isCollisionSprite() const override
    {
        return true;
    }

    [[nodiscard]] std::vector<SDL_Rect> getSlices() const
    {
        return m_slices;
    }

    void slice(const int sliceThickness)
    {
        if (sliceThickness <= 0)
            return;

        m_slices.clear();

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
                    addSlice(sliceStartX, y, sliceEndX, sliceThickness);
                    sliceStartX = -1;
                }
            }

            if (sliceStartX != -1)
                addSlice(sliceStartX, y, m_rect.w, sliceThickness);
        }
    }

    void printSlices()
    {
        for (const auto& [x, y, w, h] : m_slices)
            std::cout << "SDL_Rect { x: " << x << ", y: " << y << ", w: " << w << ", h: " << h << " }\n";
    }

    // Test function
    std::vector<std::shared_ptr<Sprite<NoCollision>>> processSlices()
    {
        std::vector<std::shared_ptr<Sprite<NoCollision>>> slices;
        for (const auto& slice : m_slices)
            slices.emplace_back(std::make_shared<Sprite<NoCollision>>(slice, generateRandomColor()));
        return slices;
    }

private:
    void addSlice(int startX, int y, int endX, int sliceThickness)
    {
        SDL_Rect sliceRect;
        sliceRect.x = startX + m_rect.x;
        sliceRect.y = y + m_rect.y;
        sliceRect.h = sliceThickness;

        while (endX < m_rect.w && getPixelAlpha(endX, y) != 0)
            ++endX;

        sliceRect.w = endX - startX;
        m_slices.push_back(sliceRect);
    }

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
        const Sprite<RectangularCollision>& self,
        const T& other,
        Vector2<double> potentialPosition)
    {
        return false;
    }

    template<>
    static bool hasCollisionWith(
        const Sprite<RectangularCollision>& self,
        const Sprite<RectangularCollision>& other,
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
        const Sprite<T>& self,
        const Sprite<T>& other,
        Vector2<double> potentialPosition)
    {
        return false;
    }

    template <>
    static bool hasCollisionWith(
        const Sprite<PolygonCollision>& self,
        const Sprite<PolygonCollision>& other,
        Vector2<double> potentialPosition)
    {
        bool hasCollision = false;

        
        
        return hasCollision;
    }
};

