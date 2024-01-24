#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include "SDLExceptions.h"
#include "Vector2.h"

class Entity
{
public:
    Entity() = default;
    virtual ~Entity() = default;
    virtual void update(double deltaTime) = 0;
    virtual void draw(SDL_Surface* windowSurface) = 0;
    virtual void setCoordinates(Vector2<double> coordinates) = 0;
    virtual void cacheTexture(SDL_Renderer* renderer) = 0;
    [[nodiscard]] virtual bool isSpecializedSprite() const { return false; }
    [[nodiscard]] virtual SDL_Rect getScreenPositionAndDimensions() const = 0;
    [[nodiscard]] virtual SDL_Surface* getSDLSurface() const = 0;
    [[nodiscard]] virtual SDL_Texture* getCachedTexture() const = 0;
    [[nodiscard]] virtual Vector2<double> getCoordinates() = 0;
    [[nodiscard]] virtual bool hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const { return false; }
};