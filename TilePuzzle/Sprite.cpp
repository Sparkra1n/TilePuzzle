#include "Sprite.h"

void Entity::update(double deltaTime)
{
    
}

// Whether object is a plain Entity
bool Entity::isSpecializedSprite() const
{
    return false;
}

SpriteBase::SpriteBase(const char* path, const Observer* observer)
    : m_renderFlag(true), m_texture(nullptr)
{
    m_surface = loadSurface(path);
    m_rect.w = m_surface->w;
    m_rect.h = m_surface->h;
}

SpriteBase::SpriteBase(const SDL_Rect rect, const SDL_Color color)
        : m_renderFlag(true), m_rect(rect), m_coordinates(rect.x, rect.y), m_texture(nullptr)
{
    SDL_Surface* surface = SDL_CreateRGBSurface(0, rect.w, rect.h, 32, 0, 0, 0, 0);
    if (!surface)
        throw SDLImageLoadException(SDL_GetError());

    SDL_FillRect(surface, nullptr, SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a));
    m_surface = surface;
}

SpriteBase::SpriteBase(const SpriteBase& other)
    : m_renderFlag(other.m_renderFlag), m_rect(other.m_rect), m_coordinates(other.m_coordinates), m_texture(nullptr)
{
    m_surface = SDL_ConvertSurface(other.m_surface, other.m_surface->format, 0);
    if (!m_surface)
        throw SDLImageLoadException(SDL_GetError());
}

SpriteBase::~SpriteBase()
{
    SDL_FreeSurface(m_surface);
}

void SpriteBase::setCoordinates(const Vector2<double> coordinates)
{
    m_coordinates = coordinates;
    m_rect.x = static_cast<int>(coordinates.x);
    m_rect.y = static_cast<int>(coordinates.y);
}

void SpriteBase::setXCoordinate(const double value)
{
    m_coordinates.x = value;
    m_rect.x = static_cast<int>(value);
}

void SpriteBase::setYCoordinate(const double value)
{
    m_coordinates.y = value;
    m_rect.y = static_cast<int>(value);
}


void SpriteBase::cacheTexture(SDL_Renderer* renderer)
{
    if (m_texture != nullptr)
        SDL_DestroyTexture(m_texture);
    m_texture = SDL_CreateTextureFromSurface(renderer, m_surface);
}

uint8_t SpriteBase::getPixelAlpha(const int x, const int y) const
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

SDL_Rect SpriteBase::getSDLRect() const
{
    return m_rect;
}

SDL_Color SpriteBase::generateRandomColor()
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

bool SpriteBase::isCollisionSprite() const
{
    return false;
}

bool SpriteBase::isSpecializedSprite() const
{
    return true;
}

Vector2<double> SpriteBase::getCoordinates() const
{
    return m_coordinates;
}

SDL_Surface* SpriteBase::getSDLSurface() const
{
    return m_surface;
}

SDL_Texture* SpriteBase::getCachedTexture() const
{
    return m_texture;
}

SDL_Surface* SpriteBase::loadSurface(const char* path)
{
    SDL_Surface* surface = SDL_LoadBMP(path);
    if (!surface)
        throw SDLImageLoadException(SDL_GetError());
    return surface;
}

void SpriteBase::setRenderFlag()
{
    m_renderFlag = true;
}

void SpriteBase::clearRenderFlag()
{
    m_renderFlag = false;
}

bool SpriteBase::getRenderFlag() const
{
    return m_renderFlag;
}

// Sprite<NoCollision>

Sprite<NoCollision>::Sprite(const char* path)
    : SpriteBase(path) {}

Sprite<NoCollision>::Sprite(const SDL_Rect rect, const SDL_Color color)
    : SpriteBase(rect, color) {}

// Sprite<RectangularCollision>

Sprite<RectangularCollision>::Sprite(const char* path, const Observer* observer)
    : SpriteBase(path), m_observer(observer) {}

Sprite<RectangularCollision>::Sprite(const SDL_Rect rect, const SDL_Color color, const Observer* observer)
    : SpriteBase(rect, color), m_observer(observer) {}

const Observer* Sprite<RectangularCollision>::getCollisionObserver() const
{
    return m_observer;
}

bool Sprite<RectangularCollision>::isCollisionSprite() const
{
    return true;
}

bool Sprite<RectangularCollision>::hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const
{
    return other.hasCollisionWithImpl(*this, potentialPosition);
}

bool Sprite<RectangularCollision>::hasCollisionWithImpl(const Sprite<RectangularCollision>& other, Vector2<double> potentialPosition) const
{
    //puts("rectangular and rectangular");
    SDL_Rect selfRect = m_rect;
    selfRect.x = static_cast<int>(potentialPosition.x);
    selfRect.y = static_cast<int>(potentialPosition.y);
    const SDL_Rect otherRect = other.getSDLRect();
    return SDL_HasIntersection(&selfRect, &otherRect) == SDL_TRUE;
}

bool Sprite<RectangularCollision>::hasCollisionWithImpl(const Sprite<PolygonCollision>& other, Vector2<double> potentialPosition) const
{
    //puts("rectangular and polygon");
    return false;
}

// Sprite<PolygonCollision>

Sprite<PolygonCollision>::Sprite(const char* path, const Observer* observer)
    : SpriteBase(path), m_observer(observer) {}

const Observer* Sprite<PolygonCollision>::getCollisionObserver() const
{
    return m_observer;
}

bool Sprite<PolygonCollision>::isCollisionSprite() const
{
    return true;
}

std::vector<SDL_Rect> Sprite<PolygonCollision>::slice(const int sliceThickness ) const
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

//void Sprite<PolygonCollision>::printSlices()
//{
//    for (const auto& [x, y, w, h] : m_slices)
//        std::cout << "SDL_Rect { x: " << x << ", y: " << y << ", w: " << w << ", h: " << h << " }\n";
//}

// Test function
std::vector<std::shared_ptr<Sprite<NoCollision>>> Sprite<PolygonCollision>::processSlices()
{
    std::vector<std::shared_ptr<Sprite<NoCollision>>> rects;
    auto slices = slice(5);
    for (const auto& slice : slices)
        rects.emplace_back(std::make_shared<Sprite<NoCollision>>(slice, generateRandomColor()));
    return rects;
}

bool Sprite<PolygonCollision>::hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const
{
    return other.hasCollisionWithImpl(*this, potentialPosition);
}

bool Sprite<PolygonCollision>::hasCollisionWithImpl(const Sprite<PolygonCollision>& other, Vector2<double> potentialPosition) const
{
    // Check if the bounding boxes of the two sprites intersect
    const SDL_Rect selfRect = m_rect;
    SDL_Rect otherRect = other.getSDLRect();
    otherRect.x = static_cast<int>(potentialPosition.x);
    otherRect.y = static_cast<int>(potentialPosition.y);
    if (SDL_HasIntersection(&selfRect, &otherRect) == SDL_FALSE)
        return false;

    const auto selfSlices = slice(2);
    const auto otherSlices = other.slice(2);

    // TODO: cache slices and determine relevant slices to check instead of looping through all of them
    for (const auto& selfSlice : selfSlices)
    {
        const SDL_Rect rect1 = selfSlice;
        for (const auto& otherSlice : otherSlices)
        {
            // Adjust the position of otherSlice based on potentialPosition
            SDL_Rect adjustedOtherSlice = otherSlice;
            adjustedOtherSlice.x += static_cast<int>(potentialPosition.x - other.getSDLRect().x);
            adjustedOtherSlice.y += static_cast<int>(potentialPosition.y - other.getSDLRect().y);

            // Check for intersection between slices
            if (SDL_HasIntersection(&rect1, &adjustedOtherSlice) == SDL_TRUE)
                return true;
        }
    }
    return false;
}

bool Sprite<PolygonCollision>::hasCollisionWithImpl(const Sprite<RectangularCollision>& other, Vector2<double> potentialPosition) const
{
    //puts("polygon and rectangular");
    return false;
}