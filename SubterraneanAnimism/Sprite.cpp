#include "Sprite.h"

#include <iostream>

Sprite::Sprite(const char* path)
{
    m_image = loadSurface(path);

    // Set dimensions of bitmap
    m_position.w = m_image->w;
    m_position.h = m_image->h;
}

void Sprite::draw(SDL_Surface* windowSurface)
{
    SDL_BlitSurface(m_image, nullptr, windowSurface, &m_position);
}

SDL_Surface* Sprite::loadSurface(const char* path)
{
    SDL_Surface* surface = SDL_LoadBMP(path);
    if (!surface)
        throw SDLImageLoadException(SDL_GetError());
    return surface;
}

bool CollisionSprite::hasCollisionWith(const Sprite& other) const
{
    const SDL_Rect otherRect = other.getPosition();
    return SDL_HasIntersection(&m_position, &otherRect) == SDL_TRUE;
}

void PlayerSprite::update(const double deltaTime)
{
    int verticalDirection = 0;
    int horizontalDirection = 0;

    for (const SDL_Keycode key : m_pressedKeys)
    {
        switch (key)
        {
        case SDLK_w:
            verticalDirection += -1;
            break;
        case SDLK_s:
            verticalDirection += 1;
            break;
        case SDLK_a:
            horizontalDirection += -1;
            break;
        case SDLK_d:
            horizontalDirection += 1;
            break;
        default:
            break;
        }
    }

    // Calculate the potential new position based on the direction and speed
    const double potentialX = m_x + m_speed * deltaTime * horizontalDirection;
    const double potentialY = m_y + m_speed * deltaTime * verticalDirection;

    // Check for collision with obstacles or other sprites
    if (!m_observer)
        return;

#if 0
    bool b = m_observer->canMoveTo(*this, potentialX, potentialY);
    std::cout << "canMoveTo: " << b
	          << "Potential: " << "(" << potentialX << ", " << potentialY << ")"
			  << "Current: " << "(" << m_position.x << ", " << m_position.y << ")" << "\r";
#endif
    if (m_observer->canMoveTo(*this, potentialX, potentialY))
    {
        m_x = potentialX;
        m_y = potentialY;
        m_position.x = static_cast<int>(m_x);
        m_position.y = static_cast<int>(m_y);
    }
}

void PlayerSprite::handleEvent(const SDL_Event& event)
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