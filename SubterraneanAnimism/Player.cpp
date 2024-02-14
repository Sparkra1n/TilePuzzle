#include "Player.h"
#include <iostream>

void Player::update(const double deltaTime)
{
    Vector2<double> direction{};

    for (const SDL_Keycode key : m_pressedKeys)
    {
        switch (key)
        {
        case SDLK_w:
            direction.y += -1;
            break;
        case SDLK_s:
            direction.y += 1;
            break;
        case SDLK_a:
            direction.x += -1;
            break;
        case SDLK_d:
            direction.x += 1;
            break;
        default:
            break;
        }
    }

    // Calculate the potential new position based on the direction and speed
    // Normalize the direction vector
    const double magnitude = sqrt(direction.x * direction.x + direction.y * direction.y);
    if (magnitude > 0)
        direction /= magnitude;
    else
        return;

    // Calculate the potential new position based on the normalized direction and speed
    const Vector2<double> potentialPosition = m_sprite.getCoordinates() + direction * m_speed * deltaTime;
    m_sprite.setRenderFlag();
    // Skip further checking if it is not a collision sprite
    if (m_sprite.getCollisionObserver() == nullptr)
        return;

    if (m_sprite.getCollisionObserver()->canMoveTo(m_sprite, potentialPosition))
    {
        // Update exact position
        m_sprite.setCoordinates(potentialPosition);

        // Update screen position
        //m_sprite.setDimensions(static_cast<int>(potentialPosition.x), static_cast<int>(potentialPosition.y));
    }

    // Collisions will result in the object sliding along the surface if the player comes an angle != 90 degrees
    else
    {
	    //Vector2 resultantDirection = 
    }
}

void Player::handleEvent(const SDL_Event& event)
{
    // Handle keydown and keyup events to update the set of pressed keys
    switch (event.type)
    {
    case SDL_KEYDOWN:
        m_pressedKeys.insert(event.key.keysym.sym);
        break;
    case SDL_KEYUP:
        m_pressedKeys.erase(event.key.keysym.sym);
        break;
    default:
        break;
    }
}

void Player::setSpeed(const double speed)
{
    m_speed = speed;
}

void Player::setCoordinates(const Vector2<double> coordinates)
{
    m_sprite.setCoordinates(coordinates);
}

void Player::cacheTexture(SDL_Renderer* renderer)
{
    m_sprite.cacheTexture(renderer);
}

Sprite<PolygonCollision>* Player::getSprite()
{
    return &m_sprite;
}

SDL_Rect Player::getSDLRect() const
{
    return m_sprite.getSDLRect();
}

SDL_Surface* Player::getSDLSurface() const
{
    return m_sprite.getSDLSurface();
}

Vector2<double> Player::getCoordinates() const
{
    return m_sprite.getCoordinates();
}

double Player::getSpeed() const
{
    return m_speed;
}

bool Player::isSpecializedSprite() const
{
    return false;
}

bool Player::hasCollisionWith(const Entity& other, const Vector2<double> potentialPosition) const
{
    return m_sprite.hasCollisionWith(other, potentialPosition);
}

SDL_Texture* Player::getCachedTexture() const
{
    return m_sprite.getCachedTexture();
}

bool Player::getRenderFlag() const
{
    return m_sprite.getRenderFlag();
}

void Player::clearRenderFlag()
{
	m_sprite.clearRenderFlag();
}

void Player::setRenderFlag()
{
    m_sprite.setRenderFlag();
}
