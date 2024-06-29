#pragma once

#include "Vector2.h"
#include "Sprite.h"
#include <unordered_set>

class Player : public ExtendedSprite
{
public:
    Player(const char* path, const Observer* observer = nullptr, const double speed = 1)
        : ExtendedSprite(path, speed, observer) {}
    void handleEvent(const SDL_Event& event) override;
protected:
    std::unordered_set<SDL_Keycode> m_pressedKeys;
};

