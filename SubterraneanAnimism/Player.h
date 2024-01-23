#pragma once

#include "Vector2.h"
#include "Sprite.h"
#include <unordered_set>

class Player: public Entity
{
public:
    Player(const char* path, const Observer* observer = nullptr, const double speed = 1.0)
        : m_sprite(path, observer), m_speed(speed) {}
    ~Player() override = default;

    void handleEvent(const SDL_Event& event);

    void update(double deltaTime) override;

    void draw(SDL_Surface* windowSurface) override
    {
	    m_sprite.draw(windowSurface);
    }

    void setSpeed(const double speed)
    {
	    m_speed = speed;
    }

    void setCoordinates(const Vector2<double> coordinates) override
    {
	    m_sprite.setCoordinates(coordinates);
    }

    [[nodiscard]] SDL_Rect getScreenPositionAndDimensions() const override
    {
	    return m_sprite.getScreenPositionAndDimensions();
    }

    [[nodiscard]] SDL_Surface* getSDLSurface() const override
    {
        return m_sprite.getSDLSurface();
    }

    [[nodiscard]] Vector2<double> getCoordinates() override
    {
	    return m_sprite.getCoordinates();
    }

    [[nodiscard]] double getSpeed() const
    {
	    return m_speed;
    }

    [[nodiscard]] bool isSpecializedSprite() const override
    {
	    return false;
    }

    [[nodiscard]] bool hasCollisionWith(const Entity& other, const Vector2<double> potentialPosition) const override
    {
	    return m_sprite.hasCollisionWith(other, potentialPosition);
    }

private:
    std::unordered_set<SDL_Keycode> m_pressedKeys;
    Sprite<RectangularCollision> m_sprite;
    double m_speed = 1.0;
};

