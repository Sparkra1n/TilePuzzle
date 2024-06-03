#include "Player.h"
#include <iostream>

void Player::update(const double deltaTime)
{
    // Handle Click controls
    if (m_targetPosition.x != -1)
    {
        if (std::abs(m_targetPosition.x - getCoordinates().x) > 1)
        {
            Vector2<double> coordinates = getCoordinates();
            const int sign = coordinates.x < m_targetPosition.x ? 1 : -1;
            coordinates.x += m_speed * deltaTime * sign;
            setCoordinates(coordinates);
        }
        else
        {
            setXCoordinate(m_targetPosition.x);
            m_targetPosition.x = -1;
        }
    }
    if (m_targetPosition.y != -1 && m_targetPosition.x == -1)
    {
        if (std::abs(m_targetPosition.y - getCoordinates().y) > 1)
        {
            Vector2<double> coordinates = getCoordinates();
            const int sign = coordinates.y < m_targetPosition.y ? 1 : -1;
            coordinates.y += m_speed * deltaTime * sign;
            setCoordinates(coordinates);
        }
        else
        {
            setYCoordinate(m_targetPosition.y);
            m_targetPosition.y = -1;
        }
    }

    // Handle WASD controls
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
    const Vector2<double> potentialPosition = getCoordinates() + direction * m_speed * deltaTime;

    // Skip further checking if it is not a collision sprite
    if (getCollisionObserver() == nullptr)
        return;

    if (getCollisionObserver()->canMoveTo(*this, potentialPosition))
        setCoordinates(potentialPosition);
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