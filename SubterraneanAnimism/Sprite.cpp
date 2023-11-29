#include "Sprite.h"

#include <iostream>

Sprite::Sprite(const char* path)
{
    m_image = loadSurface(path);

    // Set dimensions of bitmap
    m_position.w = m_image->w;
    m_position.h = m_image->h;
}

void Sprite::update(const double deltaTime)
{
    const double speed = 5.0;

    switch (m_direction)
    {
    case Direction::Nowhere:
        m_x += 0.0;
        m_y += 0.0;
        break;
    case Direction::Up:
        m_y = m_y - (speed * deltaTime);
        break;
    case Direction::Down:
        m_y = m_y + (speed * deltaTime);
        break;
    case Direction::Left:
        m_x = m_x - (speed * deltaTime);
        break;
    case Direction::Right:
        m_x = m_x + (speed * deltaTime);
        break;
    case Direction::UpLeft:
        m_x = m_x - (speed * deltaTime);
        m_y = m_y - (speed * deltaTime);
        break;
    case Direction::UpRight:
        m_x = m_x + (speed * deltaTime);
        m_y = m_y - (speed * deltaTime);
        break;
    case Direction::DownLeft:
        m_x = m_x - (speed * deltaTime);
        m_y = m_y + (speed * deltaTime);
        break;
    case Direction::DownRight:
        m_x = m_x + (speed * deltaTime);
        m_y = m_y + (speed * deltaTime);
        break;
    }

    m_position.x = static_cast<int>(m_x);
    m_position.y = static_cast<int>(m_y);
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

void Sprite::handleEvent(const SDL_Event& event)
{
    const uint8_t* keys = SDL_GetKeyboardState(nullptr);

    switch (event.type)
    {
    case SDL_KEYDOWN:
    {
        switch (event.key.keysym.sym)
        {
        case SDLK_w:
            m_direction = Direction::Up;
            break;
        case SDLK_s:
            m_direction = Direction::Down;
            break;
        case SDLK_a:
            m_direction = Direction::Left;
            break;
        case SDLK_d:
            m_direction = Direction::Right;
            break;
        default:
            break;
        }

        // Handle combinations for diagonal movement
        if (keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_A])
            m_direction = Direction::UpLeft;
        else if (keys[SDL_SCANCODE_W] && keys[SDL_SCANCODE_D])
            m_direction = Direction::UpRight;
        else if (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_A])
            m_direction = Direction::DownLeft;
        else if (keys[SDL_SCANCODE_S] && keys[SDL_SCANCODE_D])
            m_direction = Direction::DownRight;
        break;
    }

    case SDL_KEYUP:
        m_direction = Direction::Nowhere;
        break;

        // Other cases

    default:
        break;
    }
}



