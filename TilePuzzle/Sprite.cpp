/**
 * @file Sprite.h
 * @brief SDL Sprite classes
 * @author Ani
 * @version 0.1
 * @date 2023-11-26
 */

#include "Sprite.h"
#include <iostream>
#include <cmath>
#include <iomanip>

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

void Sprite::setPosition(const Vector2<double> coordinates)
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

// Old cache function
#if 0
void Sprite::cacheTexture(SDL_Renderer* renderer)
{
    if (m_texture != nullptr)
        SDL_DestroyTexture(m_texture);

    m_texture = SDL_CreateTextureFromSurface(renderer, m_surface);
}
#endif

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

bool Sprite::isCollisionSprite() const
{
    return false;
}

bool Sprite::isDummy() const
{
    return true;
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

void Sprite::applyModifier(SpriteModifier modifier)
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

void GameObject::update(const double deltaTime)
{
    if (!m_checkpoints.empty())
    {
        Vector2<double> target = m_checkpoints.front();

        // Move horizontally if x coordinates are different
        if (std::abs(target.x - getPosition().x) > 1)
        {
            Vector2<double> coordinates = getPosition();
            const int sign = coordinates.x < target.x ? 1 : -1;
            coordinates.x += m_speed * deltaTime * sign;
            setPosition(coordinates);
        }

        // Move vertically if y coordinates are different
        else if (std::abs(target.y - getPosition().y) > 1) 
        {
            Vector2<double> coordinates = getPosition();
            const int sign = coordinates.y < target.y ? 1 : -1;
            coordinates.y += m_speed * deltaTime * sign;
            setPosition(coordinates);
        }

        // Reached the checkpoint
        else
        {
            setPosition(target);
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

