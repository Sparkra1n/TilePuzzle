#pragma once

#include "SDL.h"
#include "SDL_image.h"
#include "SDLExceptions.h"
#include "Vector2.h"

class Entity
{
public:
    virtual ~Entity() = default;
    virtual void update(double deltaTime) = 0;
    virtual void draw(SDL_Surface* windowSurface) = 0;
    virtual void setDimensions(int x, int y) = 0;
    virtual void setCoordinates(const Vector2<double> coordinates) = 0;
    [[nodiscard]] virtual bool isSpecializedSprite() const { return false; }
    [[nodiscard]] virtual SDL_Rect getScreenPosition() const = 0;
    [[nodiscard]] virtual Vector2<double> getCoordinates() = 0;
    [[nodiscard]] virtual bool hasCollisionWith(const Entity& other, Vector2<double> potentialPosition) const = 0;
};